#pragma once
#include "pch.h"
#include "Global.h"
#include "User.h"
#include "../../protocol.h"

extern std::atomic<int> new_player_id;

struct EXOVER {
	WSAOVERLAPPED over;
	WSABUF wsabuf[1];
	char io_buf[MAX_BUF_SIZE];
	bool is_recv;
};

struct CLIENT {
	SOCKET m_s;
	EXOVER m_recv_over;
	int id;
	bool is_active = false;

	char io_buf[MAX_PACKET_SIZE];
	int m_prev_packet_data;
	int m_curr_packet_size;

	User* user = NULL;
};

class Lobby {
public:
	Lobby(short lobby_id);
	~Lobby();

	void ClientAccept(int id);
	void BattleServerAccept();
	void do_worker();
	void ProcessPacket(int id, void* buf);

#pragma region Sender
	void SendLoginOkPacket(int to_id);
	void SendLoginFailPacket(int to_id);
	void SendUserInfoPacket(int to_id);
	void SendFindRoomPacket(int id, short room_no); //match making Àü ÀÓ½Ã
	void SendRequestFindRoomPacket(int mmr);
	void SendCancelFindRoomPacket(int to_id);

	void SendPacket(int to_id, void* buf);
#pragma endregion
	void error_display(const char* msg, int err_no);

	std::uniform_int_distribution<int> u_id{ 0, 5000 };
	std::default_random_engine dre;

private:
	HANDLE iocp;
	WSADATA WSAData;
	SOCKET socket_listen;
	SOCKADDR_IN addr_listen;
	DWORD flags;
	int addrlen = sizeof(SOCKADDR_IN);

	SOCKADDR_IN addr_client;
	SOCKET socket_client;

	SOCKADDR_IN addr_battle_server;
	SOCKET socket_battle_server;

	short lobby_id;
	std::array<CLIENT*, MAX_PLAYER + 1> user_list;
};