#pragma once
#include "pch.h"
#include "Global.h"
#include "User.h"

extern std::atomic<int> new_playerID;

enum E_STATUS { ST_FREE, ST_ALLOC, ST_ACTIVE };
enum E_OP { OP_RECV, OP_SEND, OP_ACCEPT };

struct EXOVER {
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	char io_buf[MAX_BUF_SIZE];
	E_OP op;
};

struct CLIENT {
	EXOVER m_recv_over;
	SOCKET m_s;
	E_STATUS m_status;

	char io_buf[MAX_PACKET_SIZE];
	User* user_info = NULL;
};

class LobbyServer {
public:
	LobbyServer(short lobby_id);
	~LobbyServer();

	void ClinetAccept(int id);
	void Worker();
	void ProcessPacket(int id, void* buf);

public:
	void SendLoginOKPacket(int id);
	void SendLoginFailPacket(int id);
	void SendSignUpOKPacket(int id, int mmr);
	void SendSignUpFailPacket(int id);
	void SendMatchPacket(int id, short roomNo, char is_host);
	void SendUserInfoPacket(int id, int mmr, int winCnt);
	void SendCancelAutoMatchSuccess(int id);

	void SendPacket(int id, void* buf);

	void error_display(const char* msg, int err_no);

	std::uniform_int_distribution<int> uid{ 0, 5000 };
	std::default_random_engine dre;

private:
	HANDLE iocp;
	WSADATA WSAData;
	SOCKET listenSocket;
	SOCKADDR_IN serverAddr;
	DWORD flags;
	int addrlen = sizeof(SOCKADDR_IN);

	SOCKADDR_IN clientAddr;
	SOCKET clientSocket;

	short lobbyID;
	std::array<CLIENT*, MAX_PLAYER + 1> player;
};