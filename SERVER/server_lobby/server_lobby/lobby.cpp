#include "lobby.h"
#define LOG_ON
Lobby::Lobby(short _lobby_id)
{
	lobby_id = _lobby_id;

	int retval{ 0 };

	retval = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (retval != 0) assert(!"Lobby::Lobby");
	socket_listen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	memset(&addr_listen, 0, sizeof(SOCKADDR_IN));
	addr_listen.sin_family = AF_INET;
	addr_listen.sin_port = htons(LOBBY_SERVER_PORT);
	addr_listen.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(socket_listen, reinterpret_cast<sockaddr*>(&addr_listen), sizeof(SOCKADDR_IN));
	listen(socket_listen, SOMAXCONN);

	memset(&addr_battle_server, 0, addrlen);
	memset(&addr_listen, 0, addrlen);
	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
}

Lobby::~Lobby()
{
	closesocket(socket_listen);
	WSACleanup();
}

void Lobby::ClientAccept(int id)
{
	socket_client = accept(socket_listen, reinterpret_cast<sockaddr*>(&addr_client), &addrlen);
	CLIENT* new_user = new CLIENT;
	new_user->id = id;
	new_user->m_s = socket_client;
	new_user->m_recv_over.wsabuf[0].len = MAX_BUF_SIZE;
	new_user->m_recv_over.wsabuf[0].buf = new_user->m_recv_over.io_buf;
	new_user->m_recv_over.is_recv = true;
	new_user->user = new User;
	new_user->is_active = true;
	new_user->m_curr_packet_size = 0;
	new_user->m_prev_packet_data = 0;

	user_list[id] = new_user;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket_client), iocp, id, 0);
	ZeroMemory(&user_list[id]->m_recv_over.over, sizeof(WSAOVERLAPPED));
	flags = 0;
	int retval = WSARecv(socket_client, user_list[id]->m_recv_over.wsabuf, 1, NULL, &flags, &user_list[id]->m_recv_over.over, NULL);
	if (retval != 0) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) error_display("[Lobby::ClientAccept] WSARecv Error: ", err_no);
	}

#ifdef LOG_ON
	std::cout << "user: " << id << "connect\n";
#endif
}

void Lobby::BattleServerAccept()
{
	socket_battle_server = accept(socket_listen, reinterpret_cast<sockaddr*>(&addr_battle_server), &addrlen);

	CLIENT* new_bt_server = new CLIENT;

	new_bt_server->id = 0;
	new_bt_server->m_s = socket_battle_server;
	new_bt_server->m_recv_over.wsabuf[0].len = MAX_BUF_SIZE;
	new_bt_server->m_recv_over.wsabuf[0].buf = new_bt_server->m_recv_over.io_buf;
	new_bt_server->m_recv_over.is_recv = true;
	new_bt_server->is_active = true;
	new_bt_server->m_curr_packet_size = 0;
	new_bt_server->m_prev_packet_data = 0;

	user_list[0] = new_bt_server;

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket_battle_server), iocp, 0, 0);
	ZeroMemory(&user_list[0]->m_recv_over.over, sizeof(WSAOVERLAPPED));
	flags = 0;
	int retval = WSARecv(socket_battle_server, user_list[0]->m_recv_over.wsabuf, 1, NULL, &flags, &user_list[0]->m_recv_over.over, NULL);
	if (retval != 0) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) error_display("[Lobby::BattleServerAccept] WSARecv Error: ", err_no);
	}

#ifdef LOG_ON
	std::cout << "BattleServer Connect\n";
#endif
}

void Lobby::do_worker() {
	while (true) {
		DWORD io_byte;
		ULONG key;
		PULONG p_key = &key;
		WSAOVERLAPPED* p_over;

		BOOL ret = GetQueuedCompletionStatus(iocp, &io_byte, reinterpret_cast<PULONG_PTR>(p_key), &p_over, INFINITE);
		SOCKET clientSocket = user_list[key]->m_s;

		if (ret == false) {
			int err_no = WSAGetLastError();
			if (err_no == ERROR_NETNAME_DELETED) closesocket(clientSocket); //지정된 네트워크 이름을 더 이상 사용할 수 없습니다.
			else error_display("GQCS: ", err_no);
			WSA_IO_PENDING;
		}

		//클라이언트에서 closesocket 했을 경우
		if (io_byte == 0 && key != 0) {
			closesocket(clientSocket);
			user_list[key]->is_active = false;
			user_list[key]->user->SetPlayerLoginOk("");
			if (user_list[key]->user->GetPlayerMatch() == true) {
				//SendClientDisconnectPacket(key);
			}
			delete user_list[key]->user;
			continue;
		}

		EXOVER* exover = reinterpret_cast<EXOVER*>(p_over);

		//recv
		if (exover->is_recv == true) {
			char* buf = user_list[key]->m_recv_over.io_buf;
			unsigned psize = user_list[key]->m_curr_packet_size;
			unsigned pr_size = user_list[key]->m_prev_packet_data;

			while (io_byte > 0) {
				if (psize == 0) {
					psize = (BYTE)buf[0];
				}
				if (io_byte + pr_size >= psize) {
					unsigned char p[MAX_PACKET_SIZE];
					memcpy(p, user_list[key]->io_buf, pr_size);
					memcpy(p + pr_size, buf, psize - pr_size);
					ProcessPacket(static_cast<int>(key), p);
					io_byte -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0;
					pr_size = 0;
				}
				else {
					memcpy(user_list[key]->io_buf + pr_size, buf, io_byte);
					pr_size += io_byte;
					io_byte = 0;
				}
			}
			user_list[key]->m_curr_packet_size = psize;
			user_list[key]->m_prev_packet_data = pr_size;

			DWORD flags = 0;
			memset(&exover->over, 0, sizeof(WSAOVERLAPPED));
			WSARecv(clientSocket, exover->wsabuf, 1, 0, &flags, &exover->over, 0);
		}
		else {
			delete exover;
		}
	}
}

void Lobby::ProcessPacket(int id, void* buf)
{
	char* packet = reinterpret_cast<char*>(buf);
#ifdef LOG_ON
	std::cout << "ProcessPacket: " << (int)packet[1] << std::endl;
#endif
	switch (static_cast<PacketType>(packet[1])) {
	case PacketType::CL_LOGIN: {
		break;
	}
	case PacketType::CL_DUMMY_LOGIN: {
		user_list[id]->is_active = true;
		user_list[id]->id = id;
		User* newUser = new User();
		newUser->SetPlayerMatch(false);
		newUser->SetPlayerLoginOk("dummy");
		SendLoginOkPacket(id);
		break;
	}
	case PacketType::CL_REQUEST_USER_INFO: {
		//get db info
		SendUserInfoPacket(id);
		break;
	}
	case PacketType::CL_UPDATE_USER_INFO: {
		cl_packet_update_user_info p;
		//update db info
		break;
	}
	case PacketType::CL_FIND_ROOM:{
		cl_packet_find_room p;
		memcpy(&p, packet, sizeof(p));
		if (id != 0) {
			user_list[id]->user->SetPlayerMatch(true);

			if (id == 2) {
				SendRequestFindRoomPacket(0);
			}
		}
		break;
	}
	case PacketType::BL_ROOMREADY: {
		bl_packet_room_ready p;
		memcpy(&p, packet, sizeof(p));
		cout << "SEND READY ROOM: " << p.room_no << endl;
		SendFindRoomPacket(1, p.room_no);
		SendFindRoomPacket(2, p.room_no);
		break;
	}
	}
}
#pragma region Sender
void Lobby::SendLoginOkPacket(int to_id)
{
	lc_packet_login_ok p;
	p.size = sizeof(p);
	p.type = PacketType::LC_LOGIN_OK;
	p.id = to_id;

	SendPacket(to_id, &p);
}

void Lobby::SendLoginFailPacket(int to_id)
{
	lc_packet_login_fail p;
	p.size = sizeof(p);
	p.type = PacketType::LC_LOGIN_FAIL;

	SendPacket(to_id, &p);
}

void Lobby::SendUserInfoPacket(int to_id)
{
	lc_packet_userinfo p;
	p.size = sizeof(p);
	p.type = PacketType::LC_USERINFO;

	memcpy(p.id_str, "kkk", 10);//db에서 불러옴
	p.mmr = MMRDEFAULT + to_id; //db에서 불러옴
	SendPacket(to_id, &p);
}

//matching server 만들기 전 임시
void Lobby::SendFindRoomPacket(int id, short room_no) {
	lc_packet_find_room p;
	p.size = sizeof(p);
	p.type = PacketType::LC_FIND_ROOM;
	p.roomNum = room_no;
	if (id == 1) p.isHost = true;
	else p.isHost = false;

	SendPacket(id, &p);
}

void Lobby::SendRequestFindRoomPacket(int mmr)
{
	lb_packet_request_room p;
	p.size = sizeof(p);
	p.type = PacketType::LB_REQUEST_ROOM;

	SendPacket(0, &p); //battle server로
}

void Lobby::SendCancelFindRoomPacket(int to_id)
{
}

void Lobby::SendPacket(int id, void* buf)
{
	char* p = reinterpret_cast<char*>(buf);

	EXOVER* send_over = new EXOVER;
	ZeroMemory(send_over, sizeof(EXOVER));
	send_over->is_recv = false;
	memcpy(send_over->io_buf, p, p[0]);
	send_over->wsabuf[0].buf = send_over->io_buf;
	send_over->wsabuf[0].len = p[0];
	
	flags = 0;
	int retval = WSASend(user_list[id]->m_s, send_over->wsabuf, 1, NULL, flags, &send_over->over, NULL);
	if (retval != 0) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) error_display("[Lobby::SendPacket] WSASend Error: ", err_no);
	}

#ifdef LOG_ON
	std::cout << "Send ID: " << id << " Packet Type: " << (int)p[1] << std::endl;
#endif
}
#pragma endregion
void Lobby::error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wcout << L"error " << lpMsgBuf << endl;
	while (true);
	LocalFree(lpMsgBuf);
}
