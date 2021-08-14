#include "pch.h"
#include "netCore.h"
#include "Service.h"

NetCore::NetCore() {
	Initialize();
}

NetCore::~NetCore() {
	Destory();
}

void NetCore::Initialize() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		errorDisplay("[ERROR] wsa Init");
	}
	in_packet_size = 0;
	saved_packet_size = 0;
}

void NetCore::Destory() {
	m_battle_clients.clear();
	for (int i = 0; i < SV_COUNT; ++i) {
		closesocket(m_client.socket[i].socket);
	}
	WSACleanup();
}

int NetCore::GetBattleID()
{
	return m_client.battle_id;
}

int NetCore::Receive(SOCKET sock, char* buf, int len, int flags) {
	int receivedByte{};
	char* ptr = buf;
	int left_byte{ len };

	while (left_byte > 0) {
		receivedByte = recv(sock, ptr, left_byte, flags);
		if (receivedByte == SOCKET_ERROR) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				if (receivedByte == 0) { closesocket(sock); return -1; }
				else errorDisplay("[NETWORK]Receive error: battle_server_socket");
			}
		}
		else {
			left_byte -= receivedByte;
			ptr += receivedByte;
		}
	}
	return len - left_byte;
}

void NetCore::RecvBattleServer() {
	char buf[MAX_BUF]{};
	int ret = recv(m_client.socket[SV_BATTLE].socket, buf, MAX_BUF, 0);
	if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			errorDisplay("[NETWORK]Receive error");
			if (ret == 0) {
				closesocket(m_client.socket[SV_BATTLE].socket);
			}
		}
	}
	else {
		ProcessData(buf, ret);
	}
}

void NetCore::RecvLobbyServer() {
	char buf[MAX_BUF]{};
	int ret = recv(m_client.socket[SV_LOBBY].socket, buf, MAX_BUF, 0);
	if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			return;
		}
	}
	else {
		ProcessData(buf, ret);
	}
}

bool NetCore::SendPacket(void* buf, eSERVER sv) {
	char* p = reinterpret_cast<char*>(buf);
	int psize = (BYTE)p[0];
	int retval = send(m_client.socket[sv].socket, p, psize, 0);
//#ifdef LOG_ON
//	cout << "[NETCORE] SEND: " << (int)p[1] << " for " << (int)sv << endl;
//#endif //LOG_ON
	if (retval == SOCKET_ERROR) {
		int error_no = WSAGetLastError();
		if (error_no != WSA_IO_PENDING) {
			//errorDisplay("Send error");
		}
		return false;
	}
	return true;
}

void NetCore::CreateSocket(eSERVER sv) {
	BOOL optval{ TRUE };
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(SOCKADDR_IN));
	serveraddr.sin_family = AF_INET;

	switch (sv) {
	case SV_BATTLE: {
		m_client.socket[sv].socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		setsockopt(m_client.socket[sv].socket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
		serveraddr.sin_addr.S_un.S_addr = inet_addr(BATTLE_SERVER_IP_PUBLIC);
		serveraddr.sin_port = htons(BATTLE_SERVER_PORT);
		break;
	}
	case SV_LOBBY: {
		m_client.socket[sv].socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		setsockopt(m_client.socket[sv].socket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
		serveraddr.sin_addr.S_un.S_addr = inet_addr(LOBBY_SERVER_IP_PUBLIC);
		serveraddr.sin_port = htons(LOBBY_SERVER_PORT);
		break;
	}
	}

	m_client.socket[sv].serverAddr = serveraddr;
	m_client.socket[sv].curr_packet_size = 0;
	m_client.socket[sv].prev_packet_data = 0;
	m_client.socket[sv].connect = false;
}

void NetCore::ConnectServer(eSERVER sv) {
	int retval{ 0 };

	switch (sv) {
	case eSERVER::SV_LOBBY: {
		if (!Service::GetApp()->isConnectLobby()) {
			retval = connect(m_client.socket[sv].socket, reinterpret_cast<SOCKADDR*>(&m_client.socket[sv].serverAddr), sizeof(SOCKADDR_IN));
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() != WSAEWOULDBLOCK) {
					errorDisplay("LOBBY SERVER CONNECT ERROR!");
					while (1);
				}
			}
			else {
				cout << "[NETCORE]: SV_LOBBY CONNECT\n";
				Service::GetApp()->AddEvent(EVENT_LOBBY_CONNECT_OK);
			}
		}
		break;
	}
	case eSERVER::SV_BATTLE: { //lobby automatch request
		if (!Service::GetApp()->isConnectBattle()) {
			retval = connect(m_client.socket[sv].socket, reinterpret_cast<SOCKADDR*>(&m_client.socket[sv].serverAddr), sizeof(SOCKADDR_IN));
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() != WSAEWOULDBLOCK) {
					errorDisplay("BATTLE SERVER CONNECT ERROR!");
				}
			}
			else {
				cout << "[NETCORE]: SV_BATTLE CONNECT\n";
			}
		}
		break;
	}
	}
	//set non-blocking io
	ULONG make_nonblocking{ 1 };
	ioctlsocket(m_client.socket[sv].socket, FIONBIO, &make_nonblocking);
	m_client.socket[sv].connect = true;
}

void NetCore::DisconnectServer(eSERVER sv) {
	LINGER linger{ 1, 0 };
	setsockopt(m_client.socket[sv].socket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));
	shutdown(m_client.socket[sv].socket, SD_SEND);

	ZeroMemory(&m_client.socket[sv].serverAddr, sizeof(m_client.socket[sv].serverAddr));
	m_client.socket[sv].curr_packet_size = 0;
	m_client.socket[sv].prev_packet_data = 0;
	m_client.socket[sv].connect = false;
}

void NetCore::ProcessData(char* buf, size_t io_byte) {
	char* ptr = buf;
	static unsigned short in_packet_size = 0; // buf 안의 packet size
	static unsigned short saved_packet_size = 0;
	static char packet_buffer[MAX_BUF]; // 완전한 패킷, 일부 패킷

	/*if( saved_packet_size != 0 )
		cout << "Saved PacketSize = " << saved_packet_size << endl;*/

	static char tmpBuf[MAX_BUF]{};
	memcpy(tmpBuf, buf, io_byte);
	ZeroMemory(tmpBuf, io_byte);

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = (BYTE)ptr[0]; // 처음받은 메시지 크기를 저장
		if (io_byte + saved_packet_size >= in_packet_size) {
			// 처음받은 메시지 크기보다 새로 받은 바이트 수 + 저장하고 있던 바이트 수가 크면		
			//processPacket 호출, 넘겨준 바이트 만큼 버퍼 포인터, 받은 바이트 수 갱신
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ZeroMemory(packet_buffer, MAX_BUF);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);   // 받은 것 만큼만 임시 저장 버퍼에 담아두기.
			saved_packet_size += io_byte;
			io_byte = 0;

			cout << "Saved Packet Size = " << saved_packet_size << endl;
		}
	}
	return;
}

//#define DEB
void NetCore::ProcessPacket(char* packet_buf) {
#ifdef LOG_ON
	cout << "[NETCORE] procpacket: " << (int)packet_buf[1] << " LobbyID: " << m_client.lobby_id << " BattleID: " << m_client.battle_id << endl;
#endif
	switch ((BYTE)packet_buf[1]) { //type
	case LC_LOGIN_OK: {
#ifdef DEB
		cout << "LC_LOGIN_OK\n";
#endif
		lc_packet_login_ok* p = reinterpret_cast<lc_packet_login_ok*>(packet_buf);
		m_client.lobby_id = p->id;
		cout << "Lobby ID: " << m_client.lobby_id << endl;
		Service::GetApp()->AddEvent(EVENT_LOBBY_LOGIN_OK);
		break;
	}
	case LC_LOGIN_FAIL: {
#ifdef DEB
		cout << "LC_LOGIN_FAIL\n";
#endif
		Service::GetApp()->AddEvent(EVENT_LOBBY_LOGIN_FAIL);
		break;
	}
	case LC_SIGNUP_OK: {
#ifdef DEB
		cout << "LC_SIGNUP_OK\n";
#endif
		Service::GetApp()->AddEvent(EVENT_LOBBY_SIGNUP_OK);
		break;
	}
	case LC_SIGNUP_FAIL: {
#ifdef DEB
		cout << "LC_SIGNUP_FAIL\n";
#endif
		Service::GetApp()->AddEvent(EVENT_LOBBY_SIGNUP_FAIL);
		break;
	}
	case LC_USERINFO: {
#ifdef DEB
		cout << "LC_USERINFO\n";
#endif
		lc_packet_userinfo* p = reinterpret_cast<lc_packet_userinfo*>(packet_buf);
		memcpy(m_client.id_str, p->id_str,sizeof(char) *  MAX_ID_LEN);
		m_client.mmr = p->mmr;
		Service::GetApp()->AddEvent(EVENT_LOBBY_UPDATE_CLIENT_USERINFO, 1, m_client.mmr);
		break;
	}
	case LC_FIND_ROOM: {
#ifdef DEB
		cout << "LC_FIND_ROOM\n";
#endif
		lc_packet_find_room* p = reinterpret_cast<lc_packet_find_room*>(packet_buf);
		m_battle_clients[m_client.battle_id]->m_room_num = p->roomNum;
		m_battle_clients[m_client.battle_id]->m_host = p->isHost;
		cout<< "[FIND ROOM] isHOST: " << m_battle_clients[m_client.battle_id]->m_host <<
			" roomNo: " << m_battle_clients[m_client.battle_id]->m_room_num << endl;

		Service::GetApp()->AddEvent(EVENT_ROOM_FIND_ROOM);
		break;
	}
	case LC_CANCLE_FIND_ROOM: {
#ifdef DEB
		cout << "LC_CANCLE_FIND_ROOM\n";
#endif
		Service::GetApp()->AddEvent(EVENT_LOBBY_CANCLE_FIND_ROOM);
		break;
	}
	case BC_BATTLE_LOGIN_OK: { //객체 생성, 나의 정보를 받아옴
#ifdef DEB
		cout << "BC_BATTLE_LOGIN_OK\n";
#endif
		bc_packet_battle_login_ok* p = reinterpret_cast<bc_packet_battle_login_ok*>(packet_buf);
		m_client.battle_id = p->id;
		cout << "battle ID: "<<m_client.battle_id << endl;
		if (m_client.battle_id == 0) {
			cout << "error, BUFFER ZERO / battle ID == 0\n";
			break;
		}
		m_client.is_battle_login = true;

		std::unique_ptr<BattleClient> battle_client = std::make_unique<BattleClient>(m_client.battle_id);
		memcpy(battle_client->name, m_client.id_str, sizeof(char) * MAX_ID_LEN);
		battle_client->mmr = m_client.mmr;
		m_battle_clients[m_client.battle_id] = std::move(battle_client);
		cout << "MAKE client: " << m_client.battle_id << endl;

		Service::GetApp()->AddEvent(EVENT_BATTLE_LOGIN_OK);
		break;
	}
	case BC_BATTLE_LOGIN_FAIL: {
		cout << "ACCEPT FAIL\n";
		break;
	}
	case BC_ROOM_ENTERED: {
#ifdef DEB
		cout << "BC_ROOM_ENTERED\n";
#endif
		bc_packet_room_entered* p = reinterpret_cast<bc_packet_room_entered*>(packet_buf);

		std::unique_ptr<BattleClient> battleClient = std::make_unique<BattleClient>(p->id);
		memcpy(battleClient->name, p->name, sizeof(char) * MAX_ID_LEN);
		battleClient->mmr = p->mmr;
		battleClient->m_host = p->isManager;
		battleClient->m_player_num = p->player_no;
		battleClient->m_ready = p->ready;
		m_battle_clients[p->id] = std::move(battleClient);
		Service::GetApp()->AddEvent(EVENT_ROOM_ENTER, 1, p->id);
		break;
	}
	case BC_ROOM_LEAVED: {
#ifdef DEB
		cout << "BC_ROOM_LEAVED\n";
#endif
		bc_packet_room_leaved* p = reinterpret_cast<bc_packet_room_leaved*>(packet_buf);
		if (m_client.battle_id != p->id) {
			m_battle_clients[p->id].release();
			m_battle_clients.erase(p->id);
		}
		Service::GetApp()->AddEvent(EVENT_ROOM_LEAVE, 1, p->id);
	}
	case BC_JOIN_OK: {
#ifdef DEB
		cout << "BC_JOIN_OK\n";
#endif
		bc_packet_join_ok* p = reinterpret_cast<bc_packet_join_ok*>(packet_buf);
		int& id = m_client.battle_id;
		m_battle_clients[id]->m_player_num = p->player_no;

		Service::GetApp()->AddEvent(EVENT_ROOM_JOIN_OK);
		break;
	}
	case BC_JOIN_FAIL: {
#ifdef DEB
		cout << "BC_JOIN_FAIL\n";
#endif
		bc_packet_join_fail* p = reinterpret_cast<bc_packet_join_fail*>(packet_buf);
		m_battle_clients[m_client.battle_id]->Initialize();
		Service::GetApp()->AddEvent(EVENT_ROOM_JOIN_FAIL);
		break;
	}
	case BC_READY: {
#ifdef DEB
		cout << "BC_READY\n";
#endif
		bc_packet_ready* p = reinterpret_cast<bc_packet_ready*>(packet_buf);
		//m_battle_clients[p->id]->m_ready = p->ready;

		Service::GetApp()->AddEvent(EVENT_ROOM_READY, 2, p->id, p->ready);
		break;
	}
	case BC_ROOM_START_AVAILABLE: {
#ifdef DEB
		cout << "BC_ROOM_START_AVAILABLE\n";
#endif
		bc_packet_room_start_available* p = reinterpret_cast<bc_packet_room_start_available*>(packet_buf);
		Service::GetApp()->AddEvent(EVENT_ROOM_START_AVAILABLE, 1, p->available);
		break;
	}
	case BC_ROOM_START: {
#ifdef DEB
		cout << "BC_ROOM_START\n";
#endif
		bc_packet_room_start* p = reinterpret_cast<bc_packet_room_start*>(packet_buf);
		for (auto& st : p->start_info) {
			if (m_battle_clients.count(st.id)) {
				m_battle_clients[st.id]->m_spawn_position = st.spawn_pos;
			}
		}
		//맵정보 전달
		//m_map_info = static_cast<int>(p->map_type);

		Service::GetApp()->AddEvent(EVENT_ROOM_START);
		break;
	}
	case BC_ROUND_START: {
#ifdef DEB
		cout << "BC_ROUND_START\n";
#endif
		bc_packet_round_start* p = reinterpret_cast<bc_packet_round_start*>(packet_buf);
		Service::GetApp()->AddEvent(EVENT_GAME_ROUND_START);
		break;
	}
	//in game
	case BC_ADD_COIN: {
#ifdef DEB
		cout << "BC_ADD_COIN\n";
#endif
		bc_packet_add_coin* p = reinterpret_cast<bc_packet_add_coin*>(packet_buf);
		XMFLOAT3 arg_pos;
		int arg_coin_id{ p->coin_id };
		arg_pos.x = p->pos.x;
		arg_pos.y = p->pos.y;
		arg_pos.z = p->pos.z;

		Service::GetApp()->AddEvent(EVENT_GAME_ADD_COIN, 2, arg_pos, arg_coin_id);
		break;
	}
	case BC_UPDATE_COIN: {
#ifdef DEB
		cout << "BC_UPDATE_COIN\n";
#endif
		bc_packet_update_coin* p = reinterpret_cast<bc_packet_update_coin*>(packet_buf);
		cout << "update coin" << endl;
		Service::GetApp()->AddEvent(EVENT_GAME_UPDATE_COIN, 3, p->id, p->coin_cnt, p->delete_coin_id);
	}
	case BC_PLAYER_POS: {
//#ifdef DEB
		cout << "BC_PLAYER_POS\n";
//#endif
		bc_packet_player_pos* p = reinterpret_cast<bc_packet_player_pos*>(packet_buf);

		XMFLOAT3 arg_pos;
		arg_pos.x = p->pos.x;
		arg_pos.y = p->pos.y;
		arg_pos.z = p->pos.z;

		Service::GetApp()->AddEvent(EVENT_GAME_CALLBACK_MOVE, 2, p->id, arg_pos);
		break;
	}
	case BC_PLAYER_ROT: {
//#ifdef DEB
		cout << "BC_PLAYER_ROT\n";
//#endif
		bc_packet_player_rot* p = reinterpret_cast<bc_packet_player_rot*>(packet_buf);

		XMFLOAT3 arg_look;
		arg_look.x = p->look.x;
		arg_look.y = p->look.y;
		arg_look.z = p->look.z;

		Service::GetApp()->AddEvent(EVENT_GAME_CALLBACK_MOUSE, 2, p->id, arg_look);
		break;
	}
	case BC_CALLBACK_BULLET: {
//#ifdef DEB
		cout << "BC_SHOOT_BULLET\n";
//#endif
		bc_packet_callback_bullet* p = reinterpret_cast<bc_packet_callback_bullet*>(packet_buf);
		
		XMFLOAT3 arg_cam_look;
		XMFLOAT3 arg_bullet_pos;
		arg_cam_look.x = p->cam_look.x;
		arg_cam_look.y = p->cam_look.y;
		arg_cam_look.z = p->cam_look.z;

		arg_bullet_pos.x = p->bullet_pos.x;
		arg_bullet_pos.y = p->bullet_pos.y;
		arg_bullet_pos.z = p->bullet_pos.z;

		Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_BULLET, 5, p->shootter_id, p->bullet_type, p->bullet_idx, arg_cam_look, arg_bullet_pos);
		break;
	}
	case BC_REMOVE_BULLET: {
#ifdef DEB
		cout << "BC_REMOVE_BULLET\n";
#endif
		bc_packet_remove_bullet* p = reinterpret_cast<bc_packet_remove_bullet*>(packet_buf);
		Service::GetApp()->AddEvent(EVENT_GAME_REMOVE_BULLET, 1, p->bullet_id);
		break;
	}
	case BC_HIT: {
#ifdef DEB
		cout << "BC_HIT\n";
#endif
		bc_packet_hit* p = reinterpret_cast<bc_packet_hit*>(packet_buf);

		float arg_hp = p->hp;
		arg_hp = round(arg_hp); // * 100 / 100;

		Service::GetApp()->AddEvent(EVENT_GAME_HIT, 2, p->id, arg_hp);
		break;
	}
	case BC_DIE: {
#ifdef DEB
		cout << "BC_DIE\n";
#endif
		bc_packet_die* p = reinterpret_cast<bc_packet_die*>(packet_buf);
		Service::GetApp()->AddEvent(EVENT_GAME_DIE, 1, p->id);
		break;
	}
	case BC_ANIM_PUSH: {
		bc_packet_push_anim_type* p = reinterpret_cast<bc_packet_push_anim_type*>(packet_buf);
		//#ifdef DEB
		cout << "BC_ANIM_PUSH: " << p->id << p->anim_type << endl;
		//#endif
		Service::GetApp()->AddEvent(EVENT_GAME_CALLBACK_PUSH_ANIM, 2, p->id, p->anim_type);
		break;
	}
	case BC_ANIM_POP: {
		bc_packet_pop_anim_type* p = reinterpret_cast<bc_packet_pop_anim_type*>(packet_buf);
		//#ifdef DEB
		cout << "BC_ANIM_POP: " << p->id << p->anim_type << endl;
		//#endif
		Service::GetApp()->AddEvent(EVENT_GAME_CALLBACK_POP_ANIM, 2, p->id, p->anim_type);
		break;
	}
	case BC_LEFT_TIME: {
#ifdef DEB
		cout << "BC_LEFT_TIME\n";
#endif
		bc_packet_left_time* p = reinterpret_cast<bc_packet_left_time*>(packet_buf);
		int time{ p->left_time }; //unsigned char 로 type cast 필요할까
		Service::GetApp()->AddEvent(EVENT_GAME_TIMER, 1, time);
		break;
	}
	case BC_GAME_OVER: {
#ifdef DEB
		cout << "BC_GAME_OVER\n";
#endif
		bc_packet_game_over* p = reinterpret_cast<bc_packet_game_over*>(packet_buf);
		m_winner = p->win_team;

		Service::GetApp()->AddEvent(EVENT_GAME_GAMEOVER, 1, p->win_team);
		break;
	}
	case BC_UPDATED_USER_INFO: {
#ifdef DEB
		cout << "BC_UPDATED_USER_INFO\n";
#endif
		bc_packet_updated_user_info* p = reinterpret_cast<bc_packet_updated_user_info*>(packet_buf);
		Service::GetApp()->AddEvent(EVENT_GAME_UPDATE_SERVER_USERINFO, 1, p->mmr);
		break;
	}

	default: {
		cout << "unknown packet: " << (int)packet_buf[1] << endl;
		break;
	}
	}
}

void NetCore::errorDisplay(const char* msg)
{
	WCHAR* lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"Error! - " << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
}

//sendPacket

void NetCore::SendLobbyLoginPacket(const std::string& id, const std::string& pw) {
	memcpy(m_client.id_str, id.c_str(), sizeof(char) * MAX_ID_LEN);

	//todo: 나중에 login으로 바꿔야함
	cl_packet_login p;
	p.size = sizeof(p);
	p.type = CL_LOGIN;
	memcpy(p.id, id.c_str(), sizeof(char) * MAX_ID_LEN);
	memcpy(p.pw, pw.c_str(), sizeof(char) * MAX_ID_LEN);
	SendPacket(&p, SV_LOBBY);

	//cl_packet_dummy_login p;
	//p.size = sizeof(p);
	//p.type = CL_DUMMY_LOGIN;
	//SendPacket(&p, SV_LOBBY);
}

void NetCore::SendLobbySignUpPacket(const std::string& id, const std::string& pw) {
	//DB 업뎃 이후 
}

void NetCore::SendFindRoomPacket() {
	cl_packet_find_room p;
	p.size = sizeof(p);
	p.type = CL_FIND_ROOM;
	SendPacket(&p, SV_LOBBY);
}

void NetCore::SendMatchCancelPacket() {
	cl_packet_cancel_automatch p;
	p.size = sizeof(p);
	p.type = CL_CANCEL_AUTOMATCH;
	SendPacket(&p, SV_LOBBY);
}

void NetCore::SendRequestUserInfoPacket() {
	cl_packet_request_userinfo p;
	p.size = sizeof(p);
	p.type = CL_REQUEST_USER_INFO;
	SendPacket(&p, SV_LOBBY);
}

void NetCore::SendRequestPushAnimPacket(int anim_type) {
	cb_packet_push_anim p;
	p.size = sizeof(p);
	p.type = CB_PUSH_ANIM;
	p.id = m_client.battle_id;
	p.anim_type = anim_type;

	SendPacket(&p, SV_BATTLE);
}

void NetCore::SendRequestPopAnimPacket(int anim_type) {
	cb_packet_push_anim p;
	p.size = sizeof(p);
	p.type = CB_POP_ANIM;
	p.id = m_client.battle_id;
	p.anim_type = anim_type;

	SendPacket(&p, SV_BATTLE);
}

bool NetCore::SendBattleLoginPacket() {
	cb_packet_login p;
	p.size = sizeof(p);
	p.type = CB_LOGIN;
	memcpy(&p.name, m_client.id_str, sizeof(char) * MAX_ID_LEN);
	p.mmr = m_client.mmr;
	if (!SendPacket(&p, SV_BATTLE)) return false;
	return true;
}

void NetCore::SendBattleRoomJoinPacket() {
	//m_battle_clients[m_client.battle_id]->m_room_num = m_room_no;
	cb_packet_join p;
	p.size = sizeof(p);
	p.type = CB_JOIN;
	p.room_no = m_battle_clients[m_client.battle_id]->m_room_num;
	p.is_roomMnr = m_battle_clients[m_client.battle_id]->m_host;
	SendPacket(&p, SV_BATTLE);
}

void NetCore::SendBattleRoomLeavePacket() {
	m_battle_clients[m_client.battle_id]->Initialize();

	cb_packet_room_leave p;
	p.size = sizeof(p);
	p.type = CB_ROOM_LEAVE;
	SendPacket(&p, SV_BATTLE);
}

void NetCore::SendReadyPacket() {
	cb_packet_ready p;
	p.size = sizeof(p);
	p.type = CB_READY;
	SendPacket(&p, SV_BATTLE);
}

void NetCore::SendPositionPacket(XMFLOAT3 pos, int anim_type) {
	cb_packet_position p;
	p.size = sizeof(p);
	p.type = CB_POSITION_VECTOR;
	p.id = m_client.battle_id;
	p.pos.x = pos.x;
	p.pos.y = pos.y;
	p.pos.z = pos.z;
	p.anim_type = anim_type;
	SendPacket(&p, SV_BATTLE);
}

void NetCore::SendGetCoinPacket(int coin_id) {
	cb_packet_get_coin p;
	p.size = sizeof(p);
	p.type = CB_GET_COIN;
	p.id = m_client.battle_id;
	p.coin_id = coin_id;

	SendPacket(&p, SV_BATTLE);
}

void NetCore::SendLookVectorPacket(XMFLOAT3& look) {
	cb_packet_look_vector p;
	p.size = sizeof(p);
	p.type = CB_LOOK_VECTOR;
	p.id = m_client.battle_id;
	p.look.x = look.x;
	p.look.y = look.y;
	p.look.z = look.z;
	SendPacket(&p, SV_BATTLE);
}

void NetCore::SendBulletPacket(XMFLOAT3& dir, short bullet_type) {
	cb_packet_request_bullet p;
	p.size = sizeof(p);
	p.type = CB_REQUEST_BULLET;
	p.id = m_client.battle_id;
	p.cam_look.x = dir.x;
	p.cam_look.y = dir.y;
	p.cam_look.z = dir.z;
	p.bullet_type = bullet_type;
	SendPacket(&p, SV_BATTLE);
}

void NetCore::SendUpdateUserInfo(int mmr) {
	//DB 업뎃 이후 
}