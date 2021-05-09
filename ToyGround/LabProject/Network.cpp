#include "pch.h"
#include "Network.h"

Network::Network() {
	m_client.connect = false;
	m_client.loginok = false;
	m_client.battle_connect = false;
	m_client.id = -1;
	m_client.battle_id = -1;

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);
}

Network::~Network() { }

void Network::DoWorker() {
	while (true) {
		DWORD io_size;
		unsigned long long key;
		EXOVER* over;
		BOOL retval = GetQueuedCompletionStatus(
			iocp,
			&io_size,
			(PULONG_PTR)&key,
			reinterpret_cast<LPWSAOVERLAPPED*>(&over),
			INFINITE);

		//if (m_client.id != key) {
		//	cout << "not my input\n";
		//}
		int& id{ m_client.id };
		if (retval == false) {
			int err_no = WSAGetLastError();
			if (err_no == 64) DisconnectClient(id);
			else error_display("GQCS: ", WSAGetLastError());
		}
		if (io_size == 0) {
			DisconnectClient(id);
			continue;
		}
		SERVER_TYPE sv = over->sv_type;
		if (over->ev_type == EV_RECV) {
			unsigned char* buf = over->io_buf;
			unsigned int psize = m_client.connectSocket[sv].curr_packet_size;
			unsigned int pr_size = m_client.connectSocket[sv].prev_packet_data;

			while (io_size > 0) {
				if (psize == 0) {
					psize = buf[0];
				}
				if (io_size + pr_size >= psize) { //패킷 완성 가능
					unsigned char p[MAX_PACKET_SIZE];
					memcpy(p, m_client.connectSocket[sv].packet_buf, pr_size);
					memcpy(p + pr_size, buf, psize - pr_size);
					ProcessPacket(m_client.id, p);
					io_size -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0;
					pr_size = 0;
				}
				else {
					memcpy(m_client.connectSocket[sv].packet_buf + pr_size, buf, io_size);
					pr_size += io_size;
					io_size = 0;
				}
			}
			m_client.connectSocket[sv].curr_packet_size = psize;
			m_client.connectSocket[sv].prev_packet_data = pr_size;

			DWORD recv_flag = 0;
			int ret_recv = WSARecv(m_client.connectSocket[sv].socket, &m_client.connectSocket[sv].recv_over.wsabuf, 1, NULL, &recv_flag, &m_client.connectSocket[sv].recv_over.over, NULL);
			if (ret_recv == SOCKET_ERROR) {
				int err_no = WSAGetLastError();
				if (err_no != WSA_IO_PENDING) error_display("RECV ERROR", err_no);
			}
		}
		else if (over->ev_type == EV_SEND) {
			if (io_size != over->wsabuf.len) {
				closesocket(m_client.connectSocket[ST_LOBBY].socket);
				closesocket(m_client.connectSocket[ST_BATTLE].socket);
				m_client.connect = false;
				m_client.loginok = false;
			}
			delete over;
		}
		else {
			cout << "GQCS error!\n" << (int)sv;
			while (true);
		}
	}
}

void Network::ProcessPacket(int id, unsigned char packet[]) {
	switch (packet[1]) {
	case LC_LOGIN_OK: {
		lc_packet_login_ok* p = reinterpret_cast<lc_packet_login_ok*>(packet);
		m_client.id = p->id;
<<<<<<< HEAD
		cout << "!!!" << m_client.id << endl;
=======
>>>>>>> parent of 10430d4... Revert "Connect Client - Sever"
		m_client.loginok = true;
		SendRequestUserInfo(m_client.id);
		break;
	}
	case LC_LOGIN_FAIL: cout << "login_fail\n"; break;
	case LC_USERINFO: {
		lc_packet_userinfo* p = reinterpret_cast<lc_packet_userinfo*>(packet);
		m_client.mmr = p->mmr;
		m_client.name = p->id_str;

		cout << m_client.mmr << " " << m_client.name << endl;
		//원래는 버튼으로 오토매칭
		SendAutoMatchPacket(m_client.id);
		break;
	}
	case BC_AUTO_ACCEPT_OK: {
		bc_packet_accept_ok* p = reinterpret_cast<bc_packet_accept_ok*>(packet);
		m_client.battle_id = p->id;
		m_client.battle_connect = true;
		m_client.room_num = 1;

		SendJoinPacket(m_client.id, m_client.room_num);
		break;
	}
	case LC_MATCHSTART: {
		//scene change
#ifdef LOG_ON
		cout << "MATCHSTART" << endl;
#endif
		lc_packet_startMatch* p = reinterpret_cast<lc_packet_startMatch*>(packet);
		m_client.room_num = p->roomNum;
		m_client.is_host = p->is_host;
		ConnectBattleServer(m_client.id);
		break;
	}
	case BC_PLAYER_ROT: break;
	case BC_PLAYER_POS: {
		bc_packet_player_pos* p = reinterpret_cast<bc_packet_player_pos*>(packet);
		pos.x = p->pos.x;
		pos.y = p->pos.y;
		pos.z = p->pos.z;
		break;
	}
	case BC_JOIN_OK: {
		bc_packet_join_ok* p = reinterpret_cast<bc_packet_join_ok*>(packet);
		SendReadyPacket(m_client.id);
		break;
	}
	case BC_JOIN_FAIL: cout << "join fail\n"; break;
	case BC_AUTO_ACCEPT_FAIL: cout << "auto accept fail\n"; break;
	case BC_ROOM_ENTERED: cout << "room entered\n"; break;
	case BC_NEW_ROOM_HOST: cout << "new room host\n"; break;
	case BC_LEFT_TIME: cout << "left time\n"; break;
	case BC_READY: {
		SendGameStartPacket(m_client.id);
		break;
	}
	case BC_GAME_START: cout << "start\n"; break;
	case BC_GAME_START_AVAILABLE: {
		//SendGameStartPacket(id);
		break;
	}
	case BC_UPDATED_USER_INFO: {
		bc_packet_updated_user_info* p = reinterpret_cast<bc_packet_updated_user_info*>(packet);
		SendUpdateUserInfo(m_client.id, p->mmr);
	}
	default: {
		cout << "unknown packet" << (int)packet[1] << endl;
		while (1);
	}
	}
}

void Network::ConnectLobbyServer() {

	m_client.connectSocket[ST_LOBBY].socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	sockaddr_in ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(sockaddr_in));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(LOBBY_SERVER_PORT);
	ServerAddr.sin_addr.S_un.S_addr = inet_addr(LOBBY_SERVER_IP_PUBLIC);

	int ret_con = WSAConnect(
		m_client.connectSocket[ST_LOBBY].socket,
		(sockaddr*)&ServerAddr,
		sizeof(ServerAddr),
		NULL, NULL, NULL, NULL);
	if (ret_con != 0) {
		cout << "WSAConnect: ";
		cout << WSAGetLastError() << endl;
		//error_display("WSAConnect: ", WSAGetLastError());
	}

	m_client.connectSocket[ST_LOBBY].curr_packet_size = 0;
	m_client.connectSocket[ST_LOBBY].prev_packet_data = 0;
	ZeroMemory(&m_client.connectSocket[ST_LOBBY].recv_over, sizeof(m_client.connectSocket[ST_LOBBY].recv_over));
	m_client.connectSocket[ST_LOBBY].recv_over.sv_type = ST_LOBBY;
	m_client.connectSocket[ST_LOBBY].recv_over.ev_type = EV_RECV;

	m_client.connectSocket[ST_LOBBY].recv_over.wsabuf.buf = reinterpret_cast<char*>(m_client.connectSocket[ST_LOBBY].recv_over.io_buf);
	m_client.connectSocket[ST_LOBBY].recv_over.wsabuf.len = sizeof(m_client.connectSocket[ST_LOBBY].recv_over.io_buf);

	DWORD rcv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_client.connectSocket[ST_LOBBY].socket), iocp, 0, 0);
	int ret_rcv = WSARecv(
		m_client.connectSocket[ST_LOBBY].socket,
		&m_client.connectSocket[ST_LOBBY].recv_over.wsabuf,
		1, NULL, &rcv_flag,
		&m_client.connectSocket[ST_LOBBY].recv_over.over,
		NULL);
	if (SOCKET_ERROR == ret_rcv) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) error_display("RECV ERROR", err_no);
	}
	m_client.connect = true;
	SendLoginPacket(m_client.id);
}

void Network::ConnectBattleServer(int id) {
	if (m_client.id != id) cout << "connect battle error\n";

	m_client.connectSocket[ST_BATTLE].socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(BATTLE_SERVER_PORT);
	addr.sin_addr.S_un.S_addr = inet_addr(BATTLE_SERVER_IP_PUBLIC);

	int result = WSAConnect(m_client.connectSocket[ST_BATTLE].socket, (sockaddr*)&addr, sizeof(addr), NULL, NULL, NULL, NULL);
	if (result != 0) error_display("WSA TCP connect: ", WSAGetLastError());

	m_client.connectSocket[ST_BATTLE].curr_packet_size = 0;
	m_client.connectSocket[ST_BATTLE].prev_packet_data = 0;
	ZeroMemory(&m_client.connectSocket[ST_BATTLE].recv_over, sizeof(m_client.connectSocket[ST_BATTLE].recv_over));
	m_client.connectSocket[ST_BATTLE].recv_over.ev_type = EV_RECV;
	m_client.connectSocket[ST_BATTLE].recv_over.sv_type = ST_BATTLE;
	m_client.connectSocket[ST_BATTLE].recv_over.wsabuf.buf = reinterpret_cast<CHAR*>(m_client.connectSocket[ST_BATTLE].recv_over.io_buf);
	m_client.connectSocket[ST_BATTLE].recv_over.wsabuf.len = sizeof(m_client.connectSocket[ST_BATTLE].recv_over.io_buf);

	DWORD recv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_client.connectSocket[ST_BATTLE].socket), iocp, id, 0);
	int ret = WSARecv(m_client.connectSocket[ST_BATTLE].socket, &m_client.connectSocket[ST_BATTLE].recv_over.wsabuf, 1, NULL, &recv_flag, &m_client.connectSocket[ST_BATTLE].recv_over.over, NULL);
	if (ret == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) { error_display("RECV ERROR: ", err_no); }
	}

	SendBattleLoginPacket(m_client.id);
}

void Network::DisconnectClient(int id)
{
	closesocket(m_client.connectSocket[ST_LOBBY].socket);
	closesocket(m_client.connectSocket[ST_LOBBY].socket);
	m_client.connect = false;
	std::cout << "Client[" << m_client.id << "] Disconnect" << endl;
}

void Network::SendLoginPacket(int id)
{
	cl_packet_dummy_login p;
	p.size = sizeof(p);
	p.type = CL_DUMMY_LOGIN;
	std::string id_str{ "Dummy" };
	id_str += std::to_string(id);
	memcpy(p.id, id_str.c_str(), sizeof(char) * MAX_ID_LEN);
	SendPacket(id, &p, ST_LOBBY);
}

void Network::SendBattleLoginPacket(int id) {
	cout << "Send BattleLogin\n";
	cb_packet_login p;
	p.size = sizeof(p);
	p.type = CB_LOGIN;
	string id_str{ "D_" };
	id_str += std::to_string(id);
	strcpy(p.name, id_str.c_str());
	p.name[strlen(id_str.c_str())] = '\0';
	p.mmr = m_client.mmr;
	SendPacket(id, &p, ST_BATTLE);
}

void Network::SendRequestUserInfo(int id) {
	cout << "Send RequestUser\n";
	cl_packet_request_userinfo p;
	p.size = sizeof(p);
	p.type = CL_REQUEST_USER_INFO;
	SendPacket(id, &p, ST_LOBBY);
}

void Network::SendUpdateUserInfo(int id, int mmr) {
	cout << "Send Update\n";
	cl_packet_update_user_info p;
	p.size = sizeof(p);
	p.type = CL_UPDATE_USER_INFO;
	p.mmr = mmr;
	SendPacket(id, &p, ST_LOBBY);
}

void Network::SendAutoMatchPacket(int id) {
	cout << "Send AutoMath\n";
	cl_packet_automatch p;
	p.size = sizeof(p);
	p.type = CL_AUTOMATCH;

	SendPacket(id, &p, ST_LOBBY);
}

void Network::SendGameStartPacket(int id) {
	cout << "Send GameStart\n";
	cb_packet_start p;
	p.size = sizeof(p);
	p.type = CB_START;
	SendPacket(id, &p, ST_BATTLE);
}

void Network::SendReadyPacket(int id) {
	cout << "Send Ready\n";
	cb_packet_ready p;
	p.size = sizeof(p);
	p.type = CB_READY;
	SendPacket(id, &p, ST_BATTLE);

	//gameScene change
}

void Network::SendJoinPacket(int id, int room_no) {
	cout << "Send Join\n";
	cb_packet_join p;
	p.size = sizeof(p);
	p.type = CB_JOIN;
	p.room_no = room_no;
	p.is_roomMnr = m_client.is_host;
	SendPacket(id, &p, ST_BATTLE);
}

void Network::SendKeyDownW(int id) {
	cb_packet_move_key_status p;
	p.size = sizeof(p);
	p.type = CB_KEY_W_DOWN;

	SendPacket(id, &p, ST_BATTLE);
}

void Network::SendKeyDownA(int id) {
	cb_packet_move_key_status p;
	p.size = sizeof(p);
	p.type = CB_KEY_A_DOWN;

	SendPacket(id, &p, ST_BATTLE);
}

void Network::SendKeyDownS(int id) {
	cb_packet_move_key_status p;
	p.size = sizeof(p);
	p.type = CB_KEY_S_DOWN;

	SendPacket(id, &p, ST_BATTLE);
}

void Network::SendKeyDownD(int id) {
	cb_packet_move_key_status p;
	p.size = sizeof(p);
	p.type = CB_KEY_D_DOWN;

	SendPacket(id, &p, ST_BATTLE);
}

void Network::SendKeyUpW(int id)
{
	cb_packet_move_key_status p;
	p.size = sizeof(p);
	p.type = CB_KEY_W_UP;

	SendPacket(id, &p, ST_BATTLE);
}

void Network::SendKeyUpA(int id)
{
	cb_packet_move_key_status p;
	p.size = sizeof(p);
	p.type = CB_KEY_A_UP;

	SendPacket(id, &p, ST_BATTLE);
}

void Network::SendKeyUpS(int id)
{
	cb_packet_move_key_status p;
	p.size = sizeof(p);
	p.type = CB_KEY_S_UP;

	SendPacket(id, &p, ST_BATTLE);
}

void Network::SendKeyUpD(int id)
{
	cb_packet_move_key_status p;
	p.size = sizeof(p);
	p.type = CB_KEY_D_UP;

	SendPacket(id, &p, ST_BATTLE);
}


void Network::SendPacket(int id, void* packet, SERVER_TYPE st)
{
	int psize = reinterpret_cast<unsigned char*>(packet)[0];
	int ptype = reinterpret_cast<unsigned char*>(packet)[1];
	EXOVER* over = new EXOVER;
	over->ev_type = EV_SEND;
	memcpy(over->io_buf, packet, psize);
	ZeroMemory(&over->over, sizeof(over->over));
	over->wsabuf.buf = reinterpret_cast<char*>(over->io_buf);
	over->wsabuf.len = psize;

	int ret_snd = WSASend(m_client.connectSocket[st].socket, &over->wsabuf, 1, NULL, 0, &over->over, NULL);
	if (ret_snd != 0) {
		int error_no = WSAGetLastError();
		if (error_no != WSA_IO_PENDING) error_display("Error in SendPacket: ", error_no);
	}
}

XMFLOAT3 Network::GetPos() const
{
	return pos;
}

void Network::error_display(const char* msg, int err_no)
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
