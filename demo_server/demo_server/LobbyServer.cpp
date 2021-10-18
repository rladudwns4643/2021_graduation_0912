#include "LobbyServer.h"
#include "protocol.h"
#include "pch.h"
#include "DataBase.h"

#define LOG_ON

LobbyServer::LobbyServer()
{
	auto ret = WSAStartup(MAKEWORD(2, 2), &WSAData);
	socket_listen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(LOBBY_SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(socket_listen, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(SOCKADDR_IN));
	listen(socket_listen, SOMAXCONN);

	memset(&addr_battle, 0, addrlen);
	memset(&addr_client, 0, addrlen);
	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
}

LobbyServer::~LobbyServer() {
	closesocket(socket_listen);
	WSACleanup();
}

void LobbyServer::ClientAccept(int id) {
#ifdef LOG_ON
	std::cout << "Client Connect..\n";
#endif
	socket_client = accept(socket_listen, reinterpret_cast<sockaddr*>(&addr_client), &addrlen);
	CLIENT* new_client = new CLIENT;
	new_client->id = id;
	new_client->m_s = socket_client;
	new_client->m_recv_over.wsabuf[0].len = MAX_BUF_SIZE;
	new_client->m_recv_over.wsabuf[0].buf = new_client->m_recv_over.io_buf;
	new_client->m_recv_over.is_recv = true;
	new_client->user_info = new User();
	new_client->isActive = true;
	new_client->m_curr_packet_size = 0;
	new_client->m_prev_packet_data = 0;
	
	userList[id] = new_client;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket_client), iocp, id, 0);
	memset(&userList[id]->m_recv_over.over, 0, sizeof(WSAOVERLAPPED));
	flags = 0;
	if (WSARecv(socket_client, userList[id]->m_recv_over.wsabuf, 1, NULL, &flags, &(userList[id]->m_recv_over.over), NULL) != 0) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) error_display("WSARecv Error: ", err_no);
	}

#ifdef LOG_ON
	std::cout << "user: " << id << "connect\n";
#endif
}

void LobbyServer::BattleServerAccept() {
#ifdef LOG_ON
	std::cout << "BattleServer Connect..\n";
#endif
	socket_battle = accept(socket_listen, reinterpret_cast<sockaddr*>(&addr_battle), &addrlen);

	CLIENT* battle_server = new CLIENT;
	battle_server->id = 0;
	battle_server->m_s = socket_battle;
	battle_server->m_recv_over.wsabuf[0].len = MAX_BUF_SIZE;
	battle_server->m_recv_over.wsabuf[0].buf = battle_server->m_recv_over.io_buf;
	battle_server->m_recv_over.is_recv = true;
	battle_server->isActive = true;
	battle_server->m_curr_packet_size = 0;
	battle_server->m_prev_packet_data = 0;

	userList[0] = battle_server;

	SR::g_battleSocket = socket_battle;

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket_battle), iocp, 0, 0);
	memset(&userList[0]->m_recv_over.over, 0, sizeof(WSAOVERLAPPED));
	flags = 0;
	int ret = WSARecv(socket_battle, userList[0]->m_recv_over.wsabuf, 1, NULL, &flags, &(userList[0]->m_recv_over.over), NULL);
	if (ret != 0) {
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING) error_display("WSARecv Error: ", err);
	};

#ifdef LOG_ON
	std::cout << "BattleServer Connect Complet\n";
#endif
}

void LobbyServer::DoWorker() {
	while (true) {
		DWORD io_byte;
		ULONG key;
		PULONG p_key = &key;
		WSAOVERLAPPED* p_over;

		BOOL ret = GetQueuedCompletionStatus(iocp, &io_byte, reinterpret_cast<PULONG_PTR>(p_key), &p_over, INFINITE);
		SOCKET clientSocket = userList[key]->m_s;

		if (ret == false) {
			int err_no = WSAGetLastError();
			if (err_no == 64) closesocket(clientSocket);
			else error_display("GQCS: ", err_no);
		}

		//클라이언트에서 closesocket 했을 경우
		if (io_byte == 0 && key != 0) {
			closesocket(clientSocket);
			userList[key]->isActive = false;
			userList[key]->user_info->SetPlayerLoginOK("");
			if (userList[key]->user_info->GetPlayerMatch() == true) {
				//SendClientDisconnectPacket(key);
			}
			delete userList[key]->user_info;
			continue;
		}

		EXOVER* exover = reinterpret_cast<EXOVER*>(p_over);

		//recv
		if (exover->is_recv == true) {
			char* buf = userList[key]->m_recv_over.io_buf;
			unsigned psize = userList[key]->m_curr_packet_size;
			unsigned pr_size = userList[key]->m_prev_packet_data;

			while (io_byte > 0) {
				if (psize == 0) {
					psize = (BYTE)buf[0];
				}
				if (io_byte + pr_size >= psize) {
					unsigned char p[MAX_PACKET_SIZE];
					memcpy(p, userList[key]->io_buf, pr_size);
					memcpy(p + pr_size, buf, psize - pr_size);
					ProcessPacket(static_cast<int>(key), p);
					io_byte -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0;
					pr_size = 0;
				}
				else {
					memcpy(userList[key]->io_buf + pr_size, buf, io_byte);
					pr_size += io_byte;
					io_byte = 0;
				}
			}
			userList[key]->m_curr_packet_size = psize;
			userList[key]->m_prev_packet_data = pr_size;

			DWORD flags = 0;
			memset(&exover->over, 0, sizeof(WSAOVERLAPPED));
			WSARecv(clientSocket, exover->wsabuf, 1, 0, &flags, &exover->over, 0);
		}
		else {
			delete exover;
		}
	}
}

void LobbyServer::ProcessPacket(int id, void* buf)
{
	char* packet = reinterpret_cast<char*>(buf); 
#ifdef LOG_ON
	cout << "ProcessPacket: "<<(int)packet[1] << endl;
#endif
	switch (packet[1]) {		//packet[0] = size/ packet[1] = type
	case CL_LOGIN:{
#ifdef LOG_ON
		cout << "GET CL_LOGIN" << endl;
#endif
		cl_packet_login* p = reinterpret_cast<cl_packet_login*>(packet);
		
		int mmr{};

		cout << "get ID: " << p->id << " PW: " << p->pw << endl;
		if (DataBase::GetInstance()->LoginPlayer(p->id, p->pw, &mmr)) {
			cout << "!!" << mmr << endl;
			for (int i = 1; i < new_user_id; ++i) {
				if (userList[i]->user_info->GetPlayerID() == p->id) {
					cout << "cant find login info" << endl;
					SendLoginFailPacket(id);
					return;
				}
			}
			userList[id]->user_info->SetPlayerLoginOK(p->id);
			userList[id]->user_info->SetPlayerMMR(mmr);

			SendLoginOKPacket(id);
		}
		else { //db에 정보가 없으면 
			cout << "cant find db" << endl;
			SendLoginFailPacket(id);
		}
		break;
	}
	case CL_DUMMY_LOGIN: {
		userList[id]->isActive = true;
		userList[id]->id = id;
		User* newUser = new User();
		newUser->SetPlayerMatch(false);
		newUser->SetPlayerLoginOK("dummy");

#ifdef LOG_ON
		cout << "GET_DUMMY_LOGIN: "<< id << endl;
#endif
		SendLoginOKPacket(id);
		break;
	}
	case CL_SIGNUP: {
		cl_packet_signup* p = reinterpret_cast<cl_packet_signup*>(packet);
		int is_overlap{ false };
		DataBase::GetInstance()->CheckId(p->id, &is_overlap);
		if (is_overlap == false) {
			if (DataBase::GetInstance()->SignUpPlayer(p->id, p->pw)) {
				SendSignUpOkPacket(id, MMRDEFAULT);
				break;
			}
		}
		SendSignUpFailPacket(id);
		break;
	}
	case CL_REQUEST_USER_INFO: {
#ifdef LOG_ON
		cout << "GET CL_REQUEST_USER_INFO" << endl;
#endif
		//do check db info
		SendUserInfoPacket(id);
		break;
	}
	case BL_UPDATE_DB: {
#ifdef LOG_ON
		cout << "BL_UPDATE_DB" << endl;
#endif
		bl_packet_update_db* p = reinterpret_cast<bl_packet_update_db*>(packet);
		string t(p->id);
		DataBase::GetInstance()->SetUserInfo(t, p->mmr);
		break;
	}
	case CL_FIND_ROOM: { //22
		cout << "GET CL_FIND_ROOM" << endl;
		cl_packet_find_room p;
		memcpy(&p, packet, sizeof(p));
		if (id != 0) {
			userList[id]->user_info->SetPlayerMatch(true);
				//todo matching
			SR::g_match_list.insert(
				pair<ID, MatchInfo>(id, MatchInfo(userList[id]->user_info->GetPlayerMMR(),
					chrono::high_resolution_clock::now())));
		}
		break;
	}
	case BL_ROOMREADY: {
		bl_packet_room_ready p;
		memcpy(&p, packet, sizeof(p));
		cout << "SEND READY ROOM: " << p.room_no << " (id_1: " << p.id_1 << ") (id_2: " << p.id_2 << ")" << endl;
		SendFindRoomPacket(p.id_1, p.room_no);
		SendFindRoomPacket(p.id_2, p.room_no);
		break;
	}
	default: {
		std::cout << "DONT KNOW PACKET TYPE" << (int)packet[1] << "\n";
		break;
	}
	}
}

void LobbyServer::SendDummyLoginOKPacket(int id)
{
}

void LobbyServer::SendLoginOKPacket(int id) {
	lc_packet_login_ok p;
	p.size = sizeof(p);
	p.type = LC_LOGIN_OK;
	p.id = id;

	SendPacket(id, &p);
}

void LobbyServer::SendLoginFailPacket(int id)
{
	lc_packet_login_fail p;
	p.size = sizeof(p);
	p.type = LC_LOGIN_FAIL;

	SendPacket(id, &p);
}

void LobbyServer::SendSignUpOkPacket(int id, int mmr) {
	lc_packet_signup_ok p;
	p.size = sizeof(p);
	p.type = LC_SIGNUP_OK;
	SendPacket(id, &p);
}

void LobbyServer::SendSignUpFailPacket(int id) {
	lc_packet_signup_fail p;
	p.size = sizeof(p);
	p.type = LC_SIGNUP_FAIL;
	SendPacket(id, &p);
}

void LobbyServer::SendUserInfoPacket(int id) {
	lc_packet_userinfo p;
	p.size = sizeof(p);
	p.type = LC_USERINFO;
	
	cout << "ID: "<< userList[id]->user_info->GetPlayerID().c_str() <<
		"MMR: " << userList[id]->user_info->GetPlayerMMR() << endl;
	
	memcpy(p.id_str, userList[id]->user_info->GetPlayerID().c_str(), sizeof(char) * MAX_ID_LEN);//db에서 불러옴
	p.mmr = userList[id]->user_info->GetPlayerMMR(); //db에서 불러옴
	SendPacket(id, &p);
}

void LobbyServer::SendFindRoomPacket(int id, short room_no) {
	lc_packet_find_room p;
	p.size = sizeof(p);
	p.type = LC_FIND_ROOM;
	p.roomNum = room_no;
	if (id == 1) p.isHost = true;
	else p.isHost = false;
	
	SendPacket(id, &p);
}

void LobbyServer::SendCancelFindRoomPacket(int id)
{
}

void LobbyServer::SendRequestRoomPacket() {
	lb_packet_request_room p;
	p.size = sizeof(p);
	p.type = LB_REQUEST_ROOM;
	
	SendPacket(0, &p); //battle server로
}

void LobbyServer::SendPacket(int id, void* buf) {
	char* p = reinterpret_cast<char*>(buf);

	EXOVER* sendover = new EXOVER;
	memset(sendover, 0x00, sizeof(EXOVER));
	sendover->is_recv = false;
	memcpy(sendover->io_buf, p, p[0]);
	sendover->wsabuf[0].buf = sendover->io_buf;
	sendover->wsabuf[0].len = p[0];

	cout << "Send ID:" << id << "  Packet: " << (int)p[1] << endl;

	DWORD flags = 0;
	int retval = WSASend(userList[id]->m_s, sendover->wsabuf, 1, NULL, flags, &sendover->over, NULL);
	if (retval != 0) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) error_display("WSASend Error: ", err_no);
	}
}

void LobbyServer::error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wcout << L"error " << lpMsgBuf << endl;
	while (true);
	LocalFree(lpMsgBuf);
}
