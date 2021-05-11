#include "pch.h"
#include "battleServer.h"
#include "extern.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define LOG_ON

BattleServer::BattleServer() {
	Initialize();
}

BattleServer::~BattleServer() {
	m_ThreadHandler->JoinThreads();

	for (int i = 0; i < MAX_CLIENT; ++i) {
		delete SR::g_clients[i];
		SR::g_clients[i] = nullptr;
	}
	m_sockUtil.CleanUp();
}

void BattleServer::Initialize() {
	//0: lobby_server, 1: m_userid

	//room clear
	for (int i = 0; i < MAX_ROOM; ++i) SR::g_room_no[i] = -1;

	//wsa init
	if (!m_sockUtil.StaticInit()) while (true); //error, 일어날일 없음

	//CICP
	SR::g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	m_listen = m_sockUtil.CreateTCPSocket(SocketAddressFamily::INET);
	SocketAddress serverAddr{ INADDR_ANY, BATTLE_SERVER_PORT };
	m_listen->Bind(serverAddr);
	m_listen->Listen();
}

void BattleServer::ConncetLobbyServer() {
	TCPSocketPtr LobbySocket;
	LobbySocket = m_sockUtil.CreateTCPSocket(SocketAddressFamily::INET);
	DWORD flags = 0;
	SocketAddress s{ inet_addr(LOBBY_SERVER_IP_PUBLIC) , LOBBY_SERVER_PORT};
	int ret = LobbySocket->Connect(s);
	if (ret == 0) {
		CLIENT* LobbyServer = new CLIENT;
		LobbyServer->user_id = LOBBY_SERVER_KEY; //0: lobby_server
		LobbyServer->m_s = LobbySocket;

		LobbyServer->m_recv_over.wsabuf[0].len = MAX_BUFFER;
		LobbyServer->m_recv_over.wsabuf[0].buf = LobbyServer->m_recv_over.net_buf;

		LobbyServer->curr_packet_size = 0;
		LobbyServer->m_recv_over.ev_type = EV_RECV;
		LobbyServer->isConnected = true;
		LobbyServer->prev_packet_data = 0;
		SR::g_clients[LOBBY_SERVER_KEY] = LobbyServer;
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(LobbySocket->GetSocket()), SR::g_iocp, LOBBY_SERVER_KEY, 0);
		memset(&SR::g_clients[LOBBY_SERVER_KEY]->m_recv_over, 0, sizeof(WSAOVERLAPPED));
		LobbySocket->WSAReceive(SR::g_clients[LOBBY_SERVER_KEY]->m_recv_over.wsabuf, 1, NULL, &flags, &SR::g_clients[LOBBY_SERVER_KEY]->m_recv_over.over);
		//SendCheckConnect();
	}
#ifdef LOG_ON
	if (ret == 0) std::cout << "Connnect\n";
#endif
}

//unused
void BattleServer::AcceptLobbyServer() {
	SocketAddress LobbyServerAddr;
	TCPSocketPtr clientSocket;
	DWORD flags = 0;

	clientSocket = m_listen->Accept(LobbyServerAddr);

	CLIENT* LobbyServer = new CLIENT;
	LobbyServer->user_id = LOBBY_SERVER_KEY; //0: lobby_server
	LobbyServer->m_s = clientSocket;

	LobbyServer->m_recv_over.wsabuf[0].len = MAX_BUFFER;
	LobbyServer->m_recv_over.wsabuf[0].buf = LobbyServer->m_recv_over.net_buf;

	LobbyServer->m_recv_over.ev_type = EV_RECV;
	LobbyServer->isConnected = true;
	LobbyServer->curr_packet_size = 0;
	LobbyServer->prev_packet_data = 0;

	SR::g_clients[LOBBY_SERVER_KEY] = LobbyServer;

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket->GetSocket()), SR::g_iocp, LOBBY_SERVER_KEY, 0);
	memset(&SR::g_clients[LOBBY_SERVER_KEY]->m_recv_over, 0, sizeof(WSAOVERLAPPED));
	clientSocket->WSAReceive(SR::g_clients[LOBBY_SERVER_KEY]->m_recv_over.wsabuf, 1, NULL, &flags, &SR::g_clients[LOBBY_SERVER_KEY]->m_recv_over.over);

#ifdef LOG_ON
	std::cout << "LOBBY SERVER CONNECTED\n";
#endif
}

void BattleServer::Run() {
	m_ThreadHandler = new ThreadHandler;

	//worker, timer threads create
	m_ThreadHandler->CreateThreads();

	SocketAddress clientAddress;
	TCPSocketPtr clientSocket;
	DWORD flags = 0;

	while (true) {
		clientSocket = m_listen->Accept(clientAddress);

		//isConnected == false인 id을 user_id로
		int user_id;
		for (int i = LOBBY_SERVER_KEY + 1; i < MAX_CLIENT; ++i) {
			if (!SR::g_clients[i]->isConnected) {
				user_id = i;
				break;
			}
		}

		CLIENT* new_client = new CLIENT;
		new_client->user_id = user_id;
		new_client->room_id = -1; //기본값 무소속
		new_client->m_s = clientSocket;

		new_client->m_recv_over.wsabuf[0].len = MAX_BUFFER;
		new_client->m_recv_over.wsabuf[0].buf = new_client->m_recv_over.net_buf;

		new_client->m_recv_over.ev_type = EV_RECV;
		new_client->isConnected = true;

		new_client->curr_packet_size = 0;
		new_client->prev_packet_data = 0;

		SR::g_clients[user_id] = new_client;
#ifdef LOG_ON
		std::cout << user_id << " : accept\n";
#endif
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket->GetSocket()), SR::g_iocp, user_id, 0);
		memset(&SR::g_clients[user_id]->m_recv_over.over, 0, sizeof(WSAOVERLAPPED));
		clientSocket->WSAReceive(SR::g_clients[user_id]->m_recv_over.wsabuf, 1, NULL, &flags, &SR::g_clients[user_id]->m_recv_over.over);
		clientSocket = nullptr;
	}
}

void BattleServer::error_display(const char* msg, int err_no) {
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"error: " << err_no << lpMsgBuf << std::endl;
	//while (true); //error
	LocalFree(lpMsgBuf);
}

void BattleServer::AddTimer(EVENT& ev) {
	ATOMIC::g_timer_lock.lock();
	SR::g_timer_queue.push(ev);
	ATOMIC::g_timer_lock.unlock();
}

SOCKADDR_IN BattleServer::GetServerAdder() {
	return m_ServerAddr;
}

void BattleServer::SendPacket(int id, void* buff) {
	char* p = reinterpret_cast<char*>(buff);
	BYTE packet_size = (BYTE)p[0];
	cout << "[main]: Send: " << (int)p[1] << " id: " << id << endl;
	EX_OVER* send_over = new EX_OVER;
	//memset(send_over, 0, sizeof(EX_OVER));
	ZeroMemory(send_over, sizeof(EX_OVER));
	send_over->ev_type = EV_SEND;
	memcpy(send_over->net_buf, p, packet_size);
	send_over->wsabuf[0].len = packet_size;
	send_over->wsabuf[0].buf = send_over->net_buf;

	SR::g_clients[id]->m_s->WSASend(send_over->wsabuf, 1, 0, 0, &send_over->over);
}

void BattleServer::SendCheckConnect() {
	bl_pakcet_check_connect p;
	p.size = sizeof(p);
	p.type = BL_CHECK;
	SendPacket(LOBBY_SERVER_KEY, &p);
}

void BattleServer::SendBattleLoginOKPacket(int id) {
#ifdef LOG_ON
	cout << "SendBattleLoginOKPacket: " << id << endl;
#endif
	bc_packet_battle_login_ok p;
	p.size = sizeof(p);
	p.type = BC_BATTLE_LOGIN_OK;
	p.id = id;
	SendPacket(id, &p);
}

void BattleServer::SendBattleLoginFailPacket(int id) {
#ifdef LOG_ON
	cout << "SendBattleLoginFailPacket: " << id << endl;
#endif
	bc_packet_battle_login_fail p;
	p.size = sizeof(p);
	p.type = BC_BATTLE_LOGIN_FAIL;
	SendPacket(id, &p);
}

void BattleServer::SendAutoRoomReadyPacket(int id, int room_no) {
#ifdef LOG_ON
	cout << "SendAutoRoomReadyPacket: " << id << " room: "<< room_no << endl;
#endif
	bl_packet_room_ready p;
	p.size = sizeof(p);
	p.type = BL_ROOMREADY;
	p.id = id;
	p.room_no = room_no;
	SendPacket(id, &p);
}

void BattleServer::SendRoomJoinSuccess(int id, bool isRoomMnr) {
#ifdef LOG_ON
	cout << "SendRoomJoinSuccess: " << id << " mnr: " << isRoomMnr << endl;
#endif
	bc_packet_join_ok p;
	p.size = sizeof(p);
	p.type = BC_JOIN_OK;
	SendPacket(id, &p);
}

void BattleServer::SendRoomJoinFail(int id, int code) {
#ifdef LOG_ON
	cout << "SendRoomJoinFail: " << id << " code: " << code << endl;
#endif
	bc_packet_join_fail p;
	p.size = sizeof(p);
	p.type = BC_JOIN_FAIL;
	p.code = code;
	SendPacket(id, &p);
}

void BattleServer::SendRoomEnterPacket(int to, int enterer, bool ready, char playerNo, char* name, int mmr, bool isManager) {
#ifdef LOG_ON
	cout << "SendRoomEnterPacket: " <<
		"to: " << to <<
		" enterer: " << enterer <<
		" ready: " << ready <<
		" player_no: " << playerNo <<
		" name: " << name <<
		" mmr: " << mmr <<
		" isMnr: " << isManager << endl;
#endif
	bc_packet_room_entered p;
	p.size = sizeof(p);
	p.type = BC_ROOM_ENTERED;
	p.id = enterer;
	p.ready = ready;
	p.player_no = playerNo;
	strcpy(p.name, name);
	p.mmr = mmr;
	p.isManager = isManager;
	SendPacket(to, &p);
}

void BattleServer::SendRoomLeavePacket(int to, int leave) {
#ifdef LOG_ON
	cout << "SendRoomLeavePacket: " <<
		"to: " << to <<
		" leaver: " << leave << endl;
#endif
	bc_packet_room_leaved p;
	p.size = sizeof(p);
	p.type = BC_ROOM_LEAVED;
	p.id = leave;
	SendPacket(to, &p);
}

void BattleServer::SendGameStartPacket(int id, PTC_START_INFO* player_info) {
#ifdef LOG_ON
	cout << "SendGameStartPacket: " << id << endl;
#endif
	bc_packet_room_start p;
	p.size = sizeof(p);
	p.type = BC_ROOM_START;
	memcpy(&p.start_info, player_info, sizeof(PTC_START_INFO));
	SendPacket(id, &p);
}

void BattleServer::SendRoundStartPacket(int id) {
#ifdef LOG_ON
	cout << "SendRoundStartPacket: " << id << endl;
#endif
	bc_packet_round_start p;
	p.size = sizeof(p);
	p.type = BC_ROUND_START;
	SendPacket(id, &p);
}

void BattleServer::SendPlayerRotation(int to, int from, PTC_VECTOR look) {
#ifdef LOG_ON
	cout << "SendPlayerRotation: " <<
		"to: " << to <<
		" from: " << from << endl;
#endif
	bc_packet_player_rot p;
	p.size = sizeof(p);
	p.type = BC_PLAYER_ROT;
	p.id = from;
	p.look = look;
	SendPacket(to, &p);
}

void BattleServer::SendLeftTimePacket(int id, char left_time) {
#ifdef LOG_ON
	cout << "SendLeftTimePacket: " << id << " lefttime: " << left_time << endl;
#endif
	bc_packet_left_time p;
	p.size = sizeof(p);
	p.type = BC_LEFT_TIME;
	p.left_time = left_time;
	SendPacket(id, &p);
}

void BattleServer::SendShootPacket(int id, int bullet_id, PTC_VECTOR look) {
#ifdef LOG_ON
	cout << "SendShootPacket: " << id <<
		"bulletid: " << bullet_id << endl;
#endif
	bc_packet_shoot_bullet p;
	p.size = sizeof(p);
	p.type = BC_SHOOT_BULLET;
	p.bullet_id = bullet_id;
	p.pos = look;
	SendPacket(id, &p);
}

void BattleServer::SendGameOverPacket(int id, int winner_id) {
#ifdef LOG_ON
	cout << "SendGameOverPacket: " << id << " winner: " << winner_id << endl;
#endif
	bc_packet_game_over p;
	p.size = sizeof(p);
	p.type = BC_GAME_OVER;
	p.win_team = winner_id;
	SendPacket(id, &p);
}

void BattleServer::SendUpdateUserInfoPacket(const int& id, const int& mmr) {
#ifdef LOG_ON
	cout << "SendUpdateUserInfoPacket: " << id << " newmmr: " << mmr << endl;
#endif
	bc_packet_updated_user_info p;
	p.size = sizeof(p);
	p.type = BC_UPDATED_USER_INFO;
	p.mmr = mmr;
	SendPacket(id, &p);
}
