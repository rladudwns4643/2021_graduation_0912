#pragma once
#include "pch.h"
#include "Global.h"
#include "User.h"

extern std::atomic<int> new_playerID;

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
	bool isActive = false;

	char io_buf[MAX_PACKET_SIZE];
	int m_prev_packet_data;
	int m_curr_packet_size;

	User* user_info = NULL;
};

class LobbyServer {
public:
	LobbyServer(short lobby_id);
	~LobbyServer();

	void ClinetAccept(int id);
	void DoWorker();
	void ProcessPacket(int id, void* buf);

public:
	void SendLoginOKPacket(int id);
	void SendLoginFailPacket(int id);
	void SendSignUpOKPacket(int id, int mmr);
	void SendSignUpFailPacket(int id);
	void SendMatchPacket(int id, short roomNo, char is_host);
	void SendUserInfoPacket(int id);
	void SendCancelAutoMatchSuccess(int id);
	void SendAutoMatchPacket(int id, int mmr);
	void SendMatchStartPacket(int id);



	void SendPacket(int id, void* buf);

	void error_display(const char* msg, int err_no);
	void recv_packet_construct(const int& user_id, DWORD& io_byte);

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
	std::array<CLIENT*, MAX_PLAYER + 1> userList;
};