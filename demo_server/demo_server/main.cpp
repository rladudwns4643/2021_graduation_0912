#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <vector>
#include <thread>
#include <mutex>
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "mswsock.lib")
using namespace std;

#include "protocol.h"
constexpr auto MAX_PACKET_SIZE = 255;
constexpr auto MAX_BUFFER_SIZE = 1024;
constexpr auto MAX_USER = 10;
constexpr auto MAX_THREAD = 4;

constexpr auto ACCEPT_KEY = 999;

enum ENUMOP { OP_RECV, OP_SEND, OP_ACCEPT };
enum C_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE };

struct EXOVER {
	WSAOVERLAPPED over;
	ENUMOP op; //send인지 recv인지
	char io_buf[MAX_BUFFER_SIZE];
	union {
		WSABUF wsabuf;		//send, recv
		SOCKET c_socket;	//accept
	};
};

struct CLIENT {
	mutex m_cl;
	SOCKET m_s;					//mutex
	int m_id;					//mutex(한번 선언하고 바꾸지 않음
	EXOVER m_recv_over;
	int m_prev_size;
	char m_packet_buffer[MAX_PACKET_SIZE];
	//bool m_connected;			//mutex
	C_STATUS m_status;

	short x, y;					//성능을 위해 오류 감수 (실전에서는 걸어야함)
	char m_name[MAX_ID_LEN + 1];	//mutex
};

CLIENT g_clients[MAX_USER];
HANDLE g_iocp;
SOCKET l_socket;

void send_packet(int user_id, void* p) {
	char* buf = reinterpret_cast<char*>(p);
	CLIENT& u = g_clients[user_id];

	//u.m_recv_over.wsabuf : 이건 Recv에서 이미 사용하고 있기 때문에 쓸 수 없음
	EXOVER* exover = new EXOVER;
	exover->op = OP_SEND;
	ZeroMemory(&exover->over, sizeof(exover->over));
	exover->wsabuf.buf = exover->io_buf;
	exover->wsabuf.len = buf[0];
	memcpy(exover->io_buf, buf, buf[0]);
	DWORD flags = 0;
	WSASend(u.m_s, &exover->wsabuf, 1, NULL, flags, &exover->over, NULL);
}

void send_login_ok_packet(int user_id) {
	sc_packet_login_ok p;
	p.size = sizeof(p);
	p.type = S2C_LOGIN_OK;
	p.exp = 0;
	p.hp = 0;
	p.id = user_id;
	p.level = 0;
	p.y = g_clients[user_id].y;
	p.x = g_clients[user_id].x;

	send_packet(user_id, &p);
}

void send_enter_packet(int user_id, int o_id) {
	sc_packet_enter p;
	p.size = sizeof(p);
	p.type = S2C_ENTER;
	p.id = o_id;
	p.y = g_clients[o_id].y;
	p.x = g_clients[o_id].x;
	strcpy_s(p.name, g_clients[o_id].m_name);
	p.o_type = O_PLAYER;
	send_packet(user_id, &p);
}

void send_move_packet(int user_id, int mover) {
	sc_packet_move p;
	p.size = sizeof(p);
	p.type = S2C_MOVE;
	p.id = mover;
	p.x = g_clients[mover].x;
	p.y = g_clients[mover].y;

	send_packet(user_id, &p);
}

void send_leave_packet(int user_id, int o_id) {
	sc_packet_leave p;
	p.size = sizeof(p);
	p.type = S2C_LEAVE;
	p.id = o_id;

	send_packet(user_id, &p);
}

void do_move(int user_id, int direction) {
	CLIENT& u = g_clients[user_id];
	int x = u.x;
	int y = u.y;
	switch (direction) {
	case D_UP:
		if (y > 0) y--;
		break;
	case D_DOWN:
		if (y < (WORLD_HEIGHT - 1)) y++;
		break;
	case D_LEFT:
		if (x > 0) x--;
		break;
	case D_RIGHT:
		if (x < (WORLD_WIDTH - 1)) x++;
		break;
	default:
		cout << "Unknown Direction form Client move packet!" << endl;
		DebugBreak();
		exit(-1);
	}
	u.x = x;
	u.y = y;
	for (auto& c : g_clients) {
		c.m_cl.lock();
		if (c.m_status == ST_ACTIVE) {
			send_move_packet(c.m_id, user_id);
		}
		c.m_cl.unlock();
	}
}

void enter_game(int user_id, char m_name[]) {
	g_clients[user_id].m_cl.lock();
	strcpy_s(g_clients[user_id].m_name, m_name);
	g_clients[user_id].m_name[MAX_ID_LEN] = NULL;
	send_login_ok_packet(user_id);

	for (int i = 0; i < MAX_USER; i++) {
		if (user_id == i) continue;
		g_clients[i].m_cl.lock();
		if (g_clients[i].m_status == ST_ACTIVE) {
			send_enter_packet(user_id, i);
			send_enter_packet(i, user_id);
		}
		g_clients[i].m_cl.unlock();
	}
	g_clients[user_id].m_status = ST_ACTIVE; //data race가 일어나도 문제가 생기지 않음(덮어써도 문제가 없음)
	g_clients[user_id].m_cl.unlock(); //mutex의 범위가 넓음(병렬성 저하)
}

void process_packet(int user_id, char* buf) {
	switch (buf[1]) { //buf[0] = size / buf[1] = type
	case C2S_LOGIN: {
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(buf);
		enter_game(user_id, packet->name);
		break;
	}
	case C2S_MOVE: {
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(buf);
		do_move(user_id, packet->direction);
		break;
	}
	default: {
		cout << "unknown packet type error" << endl;
		DebugBreak(); //breakpoint와 같은 역할
		exit(-1);
	}
	}
}

void recv_packet_construct(int user_id, int io_byte) {
	CLIENT& cu = g_clients[user_id];
	EXOVER& r_o = cu.m_recv_over;

	int rest_byte = io_byte;
	char* p = r_o.io_buf;
	int packet_size = 0;
	if (cu.m_prev_size != 0) { //남아있는 버퍼에 뭔가 들어있다면
		packet_size = cu.m_packet_buffer[0]; //pakcet_size는 cu.m_pakcet_buffer[0]에 정해져 있음
	}
	while (rest_byte > 0) { //처리할 데이터가 남아있다면?
		if (packet_size == 0) { //pakcet_size가 지정되어 있지 않다면
			packet_size = p[0]; //pakcet_size는 받은 io_buf[0]에 정해져 있음
		}
		if (packet_size <= rest_byte + cu.m_prev_size) { //들어온 패킷 + 남아있는 패킷으로 하나의 패킷이 완성되면
			memcpy(cu.m_packet_buffer + cu.m_prev_size, p, packet_size - cu.m_prev_size); // 들어온 패킷 + 남아있는 패킷으로 하나의 패킷을 완성함
			process_packet(user_id, cu.m_packet_buffer);
			//--패킷 전달 이후 지역변수 초기화--
			p += packet_size - cu.m_prev_size;
			rest_byte -= (packet_size - cu.m_prev_size);
			packet_size = 0;
			cu.m_prev_size = 0;
			//--끝--
		}
		else { //들어온 패킷 + 남아있던 패킷으로 하나의 패킷이 완성이 안된다면
			memcpy(cu.m_packet_buffer + cu.m_prev_size, p, rest_byte); //packet_buffer 뒤에 rest_byte 추가 해서 남아있는 패킷에 들어온 패킷을 저장
			cu.m_prev_size += rest_byte;
			rest_byte = 0;
			p += rest_byte; //io_buf을 pakcet_buffer에 넣은만큼 뒤로 밀어줌
		}
	}
	//process_packet(user_id, r_o.io_buf); //받은 패킷 처리
}

void initialize_clients() {
	for (int i = 0; i < MAX_USER; ++i) {
		g_clients[i].m_id = i;
		g_clients[i].m_status = ST_FREE; //싱글 스레드로 돌아가는 함수(mutex가 필요없음)
	}
}

void disconnect(int user_id) {
	g_clients[user_id].m_cl.lock();
	g_clients[user_id].m_status = ST_ALLOC;
	send_leave_packet(user_id, user_id);
	closesocket(g_clients[user_id].m_s);
	for (auto& c : g_clients) {
		if (c.m_id == user_id) continue;
		c.m_cl.lock();
		if (c.m_status == ST_ACTIVE) {
			send_leave_packet(c.m_id, user_id);
		}
		c.m_cl.unlock();
	}
	g_clients[user_id].m_status = ST_FREE;
	g_clients[user_id].m_cl.unlock();
}

void worker_thread() {
	while (true) {
		DWORD io_byte;
		ULONG_PTR key;
		WSAOVERLAPPED* over;
		GetQueuedCompletionStatus(g_iocp, &io_byte, &key, &over, INFINITE); //블로킹 함수

		EXOVER* exover = reinterpret_cast<EXOVER*>(over);
		int user_id = static_cast<int>(key);
		CLIENT& cl = g_clients[user_id];

		switch (exover->op) {
		case OP_RECV: {
			if (io_byte == 0) { //연결 종료 시
				disconnect(user_id);
			}
			else {
				recv_packet_construct(user_id, io_byte);
				ZeroMemory(&cl.m_recv_over.over, sizeof(cl.m_recv_over.over));
				DWORD flags = 0;
				WSARecv(cl.m_s, &cl.m_recv_over.wsabuf, 1, NULL, &flags, &cl.m_recv_over.over, NULL);
			}
			break;
		}
		case OP_SEND: {
			if (io_byte == 0) {
				disconnect(user_id);
			}
			delete exover;
			break;
		}
		case OP_ACCEPT: {
			int user_id = -1;
			for (int i = 0; i < MAX_USER; ++i) {
				lock_guard<mutex> gl{ g_clients[i].m_cl }; //lock_guard: 선언시 lock을 걸고 해당 블럭을 탈출할 때 unlock을 해준다 
				if (g_clients[i].m_status == ST_FREE) {
					user_id = i;
					g_clients[user_id].m_status = ST_ALLOC;
					break;
				}
			}

			SOCKET c_socket = exover->c_socket;
			if (user_id == -1) {
				closesocket(exover->c_socket);
				//send_login_fail_packet();
			}
			else {
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), g_iocp, user_id, 0); //IOCP에 socket 등록
				CLIENT& nc = g_clients[user_id];
				nc.m_prev_size = 0;
				nc.m_recv_over.op = OP_RECV;
				ZeroMemory(&nc.m_recv_over.over, sizeof(nc.m_recv_over.over));
				nc.m_recv_over.wsabuf.buf = nc.m_recv_over.io_buf;
				nc.m_recv_over.wsabuf.len = MAX_BUFFER_SIZE;
				nc.m_s = c_socket;
				nc.x = rand() % WORLD_WIDTH;
				nc.y = rand() % WORLD_HEIGHT;

				DWORD flags = 0;
				WSARecv(c_socket, &nc.m_recv_over.wsabuf, 1, NULL, &flags, &nc.m_recv_over.over, NULL);
			}
			c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			exover->c_socket = c_socket;	//하나의 accept가 끝나고 새로운 accept를 WSASocket을 통해 만들었으니까 exover->c_socket에 해당 c_socket 삽입
			ZeroMemory(&exover->over, sizeof(exover->over));
			AcceptEx(l_socket, c_socket, exover->io_buf, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &exover->over);
			break;
		}
		}
	}
}

int main() {
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	l_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN s_address;
	memset(&s_address, 0, sizeof(s_address));
	s_address.sin_family = AF_INET;
	s_address.sin_port = htons(SERVER_PORT);
	s_address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(l_socket, reinterpret_cast<sockaddr*>(&s_address), sizeof(s_address));
	listen(l_socket, SOMAXCONN);

	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	initialize_clients();

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(l_socket), g_iocp, ACCEPT_KEY, 0);
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	EXOVER accept_over;
	ZeroMemory(&accept_over.over, sizeof(accept_over.over));
	accept_over.op = OP_ACCEPT;
	accept_over.c_socket = c_socket;
	AcceptEx(l_socket, c_socket, accept_over.io_buf, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.over);

	vector<thread> worker_threads;
	for (int i = 0; i < MAX_THREAD; ++i) {
		worker_threads.emplace_back(worker_thread);
	}
	for (auto& th : worker_threads) {
		th.join();
	}

}