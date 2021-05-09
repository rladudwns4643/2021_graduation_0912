#include "Dummy.h"

#define LOG_ON

Dummy::Dummy()
{
	for (int i = 0; i < MAX_DUMMY; ++i) {
		dummy[i].connect = false;
		dummy[i].loginok = false;
		dummy[i].battle_connect = false;
		dummy[i].id = -1;
		dummy[i].battle_id = -1;
	}
	num_connections = 0;
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);
}

Dummy::~Dummy()
{
}

void Dummy::DoWorker() {
	while (true) {
		DWORD io_size;
		unsigned long long ci;
		EXOVER* over;
		BOOL retval = GetQueuedCompletionStatus(
			iocp, 
			&io_size, 
			(PULONG_PTR)&ci, 
			reinterpret_cast<LPWSAOVERLAPPED*>(&over),
			INFINITE);

		int client_id = static_cast<int>(ci);
		if (retval == false) {
			int err_no = WSAGetLastError();
			if (err_no == 64) DisconnectClient(client_id);
			else error_display("GQCS: ", WSAGetLastError());
		}
		if (io_size == 0) {
			DisconnectClient(client_id);
			continue;
		}
		SERVER_TYPE sv = over->sv_type;
		if (over->ev_type == EV_RECV) {
			unsigned char* buf = over->io_buf;
			unsigned int psize = dummy[ci].connectSocket[sv].curr_packet_size;
			unsigned int pr_size = dummy[ci].connectSocket[sv].prev_packet_data;

			while (io_size > 0) {
				if (psize == 0) {
					psize = buf[0];
				}
				if (io_size + pr_size >= psize) { //패킷 완성 가능
					unsigned char p[MAX_PACKET_SIZE];
					memcpy(p, dummy[ci].connectSocket[sv].packet_buf, pr_size);
					memcpy(p + pr_size, buf, psize - pr_size);
					ProcessPacket(static_cast<int>(ci), p);
					io_size -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0;
					pr_size = 0;
				}
				else {
					memcpy(dummy[ci].connectSocket[sv].packet_buf + pr_size, buf, io_size);
					pr_size += io_size;
					io_size = 0;
				}
			}
			dummy[ci].connectSocket[sv].curr_packet_size = psize;
			dummy[ci].connectSocket[sv].prev_packet_data = pr_size;

			DWORD recv_flag = 0;
			int ret_recv = WSARecv(dummy[ci].connectSocket[sv].socket, &dummy[ci].connectSocket[sv].recv_over.wsabuf, 1, NULL, &recv_flag, &dummy[ci].connectSocket[sv].recv_over.over, NULL);
			if (ret_recv == SOCKET_ERROR) {
				int err_no = WSAGetLastError();
				if (err_no != WSA_IO_PENDING) error_display("RECV ERROR", err_no);
			}
		}
		else if (over->ev_type == EV_SEND) {
			if (io_size != over->wsabuf.len) {
				closesocket(dummy[client_id].connectSocket[ST_LOBBY].socket);
				closesocket(dummy[client_id].connectSocket[ST_BATTLE].socket);
				dummy[client_id].connect = false;
				dummy[client_id].loginok = false;
			}
			delete over;
		}
		else {
			cout << "GQCS error!\n" << (int)sv;
			while (true);
		}
	}
		
}

void Dummy::ProcessPacket(int id, unsigned char packet[])
{
	switch (packet[1]) {
	case LC_LOGIN_OK: {
		lc_packet_login_ok* p = reinterpret_cast<lc_packet_login_ok*>(packet);
		int my_id = dummy_count++;
		dummy[my_id].id = p->id;
		dummy[my_id].loginok = true;
		SendRequestUserInfo(id);
		break;
	}
	case LC_LOGIN_FAIL: cout << "login_fail\n"; break;
	case LC_USERINFO: {
		lc_packet_userinfo* p = reinterpret_cast<lc_packet_userinfo*>(packet);
		dummy[id].mmr = p->mmr;
		dummy[id].name = p->id_str;

		cout << dummy[id].mmr << " "<<dummy[id].name << endl;
		//원래는 버튼으로 오토매칭
		SendAutoMatchPacket(id);
		break;
	}
	case BC_ACCEPT_OK: {
		bc_packet_accept_ok* p = reinterpret_cast<bc_packet_accept_ok*>(packet);
		dummy[id].battle_id = p->id;
		dummy[id].battle_connect = true;
		dummy[id].room_num = 1;

		SendJoinPacket(id, dummy[id].room_num);
		break;
	}
	case LC_MATCHSTART: {
		//scene change
#ifdef LOG_ON
		cout << "MATCHSTART" << endl;
#endif
		lc_packet_startMatch* p = reinterpret_cast<lc_packet_startMatch*>(packet);
		dummy[id].room_num = p->roomNum;
		dummy[id].is_host = p->is_host;
		ConnectBattleServer(id);
		break;
	}
	case BC_PLAYER_ROT: break;
	case BC_PLAYER_POS: {
		bc_packet_player_pos* p = reinterpret_cast<bc_packet_player_pos*>(packet);
		cout << p->pos.x << " " << p->pos.y << " " << p->pos.z << endl;
		break;
	}
	case BC_JOIN_OK: {
		bc_packet_join_ok* p = reinterpret_cast<bc_packet_join_ok*>(packet);
		SendReadyPacket(id);
		break;
	}
	case BC_JOIN_FAIL: cout << "join fail\n"; break;
	case BC_ACCEPT_FAIL: cout << "auto accept fail\n"; break;
	case BC_ROOM_ENTERED: cout << "room entered\n"; break;
	case BC_NEW_ROOM_HOST: cout << "new room host\n"; break;
	case BC_LEFT_TIME: {
		cout << "left time\n"; 
		SendKeyDownW(id);
		break;
	}
	case BC_READY: {
		SendGameStartPacket(id);
		break;
	}
	case BC_GAME_START: cout << "start\n"; break;
	case BC_GAME_START_AVAILABLE: {
		//SendGameStartPacket(id);
		break;
	}
	case BC_UPDATED_USER_INFO: {
		bc_packet_updated_user_info* p = reinterpret_cast<bc_packet_updated_user_info*>(packet);
		SendUpdateUserInfo(id, p->mmr);
	}
	default: {
		cout << "unknown packet" << (int)packet[1] << endl;
		while (1);
	}
	}
}

void Dummy::ConnectLobbyServer() {
	if (num_connections >= MAX_DUMMY) return;

	dummy[num_connections].connectSocket[ST_LOBBY].socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	sockaddr_in ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(sockaddr_in));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(LOBBY_SERVER_PORT);
	ServerAddr.sin_addr.S_un.S_addr = inet_addr(LOBBY_SERVER_IP_PUBLIC);

	int ret_con = WSAConnect(
		dummy[num_connections].connectSocket[ST_LOBBY].socket,
		(sockaddr*)&ServerAddr,
		sizeof(ServerAddr),
		NULL, NULL, NULL, NULL);
	if (ret_con != 0) {
		cout << "WSAConnect: ";
		cout << WSAGetLastError() << endl;
		//error_display("WSAConnect: ", WSAGetLastError());
	}

	dummy[num_connections].connectSocket[ST_LOBBY].curr_packet_size = 0;
	dummy[num_connections].connectSocket[ST_LOBBY].prev_packet_data = 0;
	ZeroMemory(&dummy[num_connections].connectSocket[ST_LOBBY].recv_over, sizeof(dummy[num_connections].connectSocket[ST_LOBBY].recv_over));
	dummy[num_connections].connectSocket[ST_LOBBY].recv_over.sv_type = ST_LOBBY;
	dummy[num_connections].connectSocket[ST_LOBBY].recv_over.ev_type = EV_RECV;

	dummy[num_connections].connectSocket[ST_LOBBY].recv_over.wsabuf.buf = reinterpret_cast<char*>(dummy[num_connections].connectSocket[ST_LOBBY].recv_over.io_buf);
	dummy[num_connections].connectSocket[ST_LOBBY].recv_over.wsabuf.len = sizeof(dummy[num_connections].connectSocket[ST_LOBBY].recv_over.io_buf);

	DWORD rcv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(dummy[num_connections].connectSocket[ST_LOBBY].socket), iocp, num_connections, 0);
	int ret_rcv = WSARecv(
		dummy[num_connections].connectSocket[ST_LOBBY].socket,
		&dummy[num_connections].connectSocket[ST_LOBBY].recv_over.wsabuf,
		1, NULL, &rcv_flag,
		&dummy[num_connections].connectSocket[ST_LOBBY].recv_over.over,
		NULL);
	if (SOCKET_ERROR == ret_rcv) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) error_display("RECV ERROR", err_no);
	}
	dummy[num_connections].connect = true;
	SendLoginPacket(num_connections);
	//std::cout << "num connection: " << num_connections << std::endl;
	//num_connections++;
	//while (!dummy[num_connections - 1].loginok) {};
	//SendAutoMatchPacket(num_connections - 1);
}

void Dummy::ConnectBattleServer(int id) {
	dummy[id].connectSocket[ST_BATTLE].socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(BATTLE_SERVER_PORT);
	addr.sin_addr.S_un.S_addr = inet_addr(BATTLE_SERVER_IP_PUBLIC);

	int result = WSAConnect(dummy[id].connectSocket[ST_BATTLE].socket, (sockaddr*)&addr, sizeof(addr), NULL, NULL, NULL, NULL);
	if (result != 0) error_display("WSA TCP connect: ", WSAGetLastError());

	dummy[id].connectSocket[ST_BATTLE].curr_packet_size = 0;
	dummy[id].connectSocket[ST_BATTLE].prev_packet_data = 0;
	ZeroMemory(&dummy[id].connectSocket[ST_BATTLE].recv_over, sizeof(dummy[id].connectSocket[ST_BATTLE].recv_over));
	dummy[id].connectSocket[ST_BATTLE].recv_over.ev_type = EV_RECV;
	dummy[id].connectSocket[ST_BATTLE].recv_over.sv_type = ST_BATTLE;
	dummy[id].connectSocket[ST_BATTLE].recv_over.wsabuf.buf = reinterpret_cast<CHAR*>(dummy[id].connectSocket[ST_BATTLE].recv_over.io_buf);
	dummy[id].connectSocket[ST_BATTLE].recv_over.wsabuf.len = sizeof(dummy[id].connectSocket[ST_BATTLE].recv_over.io_buf);

	DWORD recv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(dummy[id].connectSocket[ST_BATTLE].socket), iocp, id, 0);
	int ret = WSARecv(dummy[id].connectSocket[ST_BATTLE].socket, &dummy[id].connectSocket[ST_BATTLE].recv_over.wsabuf, 1, NULL, &recv_flag, &dummy[id].connectSocket[ST_BATTLE].recv_over.over, NULL);
	if (ret == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) { error_display("RECV ERROR: ", err_no); }
	}

	SendBattleLoginPacket(id);
}

void Dummy::DisconnectClient(int id)
{
	closesocket(dummy[id].connectSocket[ST_LOBBY].socket);
	closesocket(dummy[id].connectSocket[ST_LOBBY].socket);
	dummy[id].connect = false;
	std::cout << "Client[" << id << "] Disconnect" << endl;
}

void Dummy::SendLoginPacket(int id)
{
	//cl_pakcet_login pakcet;
	cl_packet_dummy_login p;
	p.size = sizeof(p);
	p.type = CL_DUMMY_LOGIN;
	std::string id_str{ "Dummy" };
	id_str += std::to_string(id);
	memcpy(p.id, id_str.c_str(), sizeof(char) * MAX_ID_LEN);
	SendPacket(id, &p, ST_LOBBY);
}

void Dummy::SendBattleLoginPacket(int id) {
	cout << "Send BattleLogin\n";
	cb_packet_login p;
	p.size = sizeof(p);
	p.type = CB_LOGIN;
	string id_str{ "D_" };
	id_str += std::to_string(id);
	strcpy(p.name, id_str.c_str());
	p.name[strlen(id_str.c_str())] = '\0';
	p.mmr = dummy[id].mmr;
	SendPacket(id, &p, ST_BATTLE);
}

void Dummy::SendRequestUserInfo(int id) {
	cout << "Send RequestUser\n";
	cl_packet_request_userinfo p;
	p.size = sizeof(p);
	p.type = CL_REQUEST_USER_INFO;
	SendPacket(id, &p, ST_LOBBY);
}

void Dummy::SendUpdateUserInfo(int id, int mmr) {
	cout << "Send Update\n";
	cl_packet_update_user_info p;
	p.size = sizeof(p);
	p.type = CL_UPDATE_USER_INFO;
	p.mmr = mmr;
	SendPacket(id, &p, ST_LOBBY);
}

void Dummy::SendAutoMatchPacket(int id) {
	cout << "Send AutoMath\n";
	cl_packet_automatch p;
	p.size = sizeof(p);
	p.type = CL_AUTOMATCH;

	SendPacket(id, &p, ST_LOBBY);
}

void Dummy::SendGameStartPacket(int id) {
	cout << "Send GameStart\n";
	cb_packet_start p;
	p.size = sizeof(p);
	p.type = CB_START;
	SendPacket(id, &p, ST_BATTLE);
}

void Dummy::SendReadyPacket(int id) {
	cout << "Send Ready\n";
	cb_packet_ready p;
	p.size = sizeof(p);
	p.type = CB_READY;
	SendPacket(id, &p, ST_BATTLE);

	//gameScene change
}

void Dummy::SendJoinPacket(int id, int room_no) {
	cout << "Send Join\n";
	cb_packet_join p;
	p.size = sizeof(p);
	p.type = CB_JOIN;
	p.room_no = room_no;
	p.is_roomMnr = dummy[id].is_host;
	SendPacket(id, &p, ST_BATTLE);
}


void Dummy::SendKeyDownW(int id) {
	cb_packet_move_key_status p;
	p.size = sizeof(p);
	p.type = CB_KEY_W_DOWN;

	SendPacket(id, &p, ST_BATTLE);
}


void Dummy::SendPacket(int id, void* packet, SERVER_TYPE st)
{
	int psize = reinterpret_cast<unsigned char*>(packet)[0];
	int ptype = reinterpret_cast<unsigned char*>(packet)[1];
	EXOVER* over = new EXOVER;
	over->ev_type = EV_SEND;
	memcpy(over->io_buf, packet, psize);
	ZeroMemory(&over->over, sizeof(over->over));
	over->wsabuf.buf = reinterpret_cast<char*>(over->io_buf);
	over->wsabuf.len = psize;

	int ret_snd = WSASend(dummy[id].connectSocket[st].socket, &over->wsabuf, 1, NULL, 0, &over->over, NULL);
	if (ret_snd != 0) {
		int error_no = WSAGetLastError();
		if (error_no != WSA_IO_PENDING) error_display("Error in SendPacket: ", error_no);
	}
}

void Dummy::error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"에러" << lpMsgBuf << std::endl;

	LocalFree(lpMsgBuf);
	while (true);
}
