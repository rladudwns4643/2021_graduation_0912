#include "Dummy.h"

Dummy::Dummy()
{
	for (int i = 0; i < MAX_DUMMY; ++i) {
		dummy[i].connect = false;
		dummy[i].loginok = false;
		dummy[i].id = -1;
	}
	num_connections = 0;
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);
}

Dummy::~Dummy()
{
}

void Dummy::DoWorker()
{
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

		SERVER_TYPE st = ST_LOBBY;
		unsigned char* buf = over->io_buf;
		unsigned int psize = dummy[ci].connectSocket[st].curr_packet_size;
		unsigned int pr_size = dummy[ci].connectSocket[st].prev_packet_data;

		while (io_size > 0) {
			if (psize == 0) {
				psize = buf[0];
			}
			if (io_size + pr_size >= psize) { //패킷 완성 가능
				unsigned char p[MAX_PACKET_SIZE];
				memcpy(p, dummy[ci].connectSocket[st].packet_buf, pr_size);
				memcpy(p + pr_size, buf, psize - pr_size);
				ProcessPacket(static_cast<int>(ci), p);
				io_size -= psize - pr_size;
				buf += psize - pr_size;
				psize = 0;
				pr_size = 0;
			}
			else {
				memcpy(dummy[ci].connectSocket[st].packet_buf + pr_size, buf, io_size);
				pr_size += io_size;
				io_size = 0;
			}
		}
		dummy[ci].connectSocket[st].curr_packet_size = psize;
		dummy[ci].connectSocket[st].prev_packet_data = pr_size;

		DWORD recv_flag = 0;
		int ret_recv = WSARecv(dummy[ci].connectSocket[st].socket, &dummy[ci].connectSocket[st].recv_over.wsabuf, 1, NULL, &recv_flag, &dummy[ci].connectSocket[st].recv_over.over, NULL);
		if (ret_recv == SOCKET_ERROR) {
			int err_no = WSAGetLastError();
			if (err_no != WSA_IO_PENDING) error_display("RECV ERROR", err_no);
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
		cout << "LOGIN OK ID: "<< p->id << endl;
		break;
	}
	case LC_LOGIN_FAIL: {
		cout << "login_fail" << endl;
		break;
	}
	case LC_USERINFO: {

		break;
	}
	}
}

void Dummy::ConnectLobbyServer()
{
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
	dummy[num_connections].connectSocket[ST_LOBBY].recv_over.st_type = ST_LOBBY;

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
	std::cout << "num connection: " << num_connections << std::endl;
	num_connections++;
	while (!dummy[num_connections - 1].loginok) {};
	//SendAutoMatchPacket(num_connections - 1);
}

void Dummy::ConnectBattleServer()
{
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
	cout << "!!" << endl;
}

void Dummy::SendUpdateUserInfo(int id, int mmr, int winrate)
{
}

void Dummy::SendPacket(int id, void* packet, SERVER_TYPE st)
{
	int psize = reinterpret_cast<unsigned char*>(packet)[0];
	int ptype = reinterpret_cast<unsigned char*>(packet)[1];
	EXOVER* over = new EXOVER;
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
