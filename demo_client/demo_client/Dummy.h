#pragma once
#include "pch.h"
#include "../../demo_server/demo_server/protocol.h"

#define MAX_BUF	128
#define MAX_PACKET_SIZE	128
#define MAX_DUMMY	500

enum SERVER_TYPE { ST_LOBBY, ST_BATTLE, ST_COUNT};

struct EXOVER {
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char io_buf[MAX_BUF];
	SERVER_TYPE st_type;
};

struct ConnectSocket {
	SOCKET socket;
	EXOVER recv_over;
	unsigned char packet_buf[MAX_PACKET_SIZE];
	int prev_packet_data;
	int curr_packet_size;
};

struct CLIENT {
	int id;
	short room_num;
	char is_host = 0;

	bool connect;
	bool loginok;

	ConnectSocket connectSocket[ST_COUNT];

	std::string name;
	int mmr = -1;
};

class Dummy {
public:
	Dummy();
	~Dummy();

	void DoWorker();
	void ProcessPacket(int id, unsigned char packet[]);

	void ConnectLobbyServer();
	void ConnectBattleServer();

	void DisconnectClient(int id);

	void SendLoginPacket(int id);
	void SendRequestUserInfo(int id);
	void SendUpdateUserInfo(int id, int mmr, int winrate);
	void SendAutoMatchPacket(int id);

	void SendPacket(int id, void* packet, SERVER_TYPE st);

	void error_display(const char* msg, int err_no);

private:
	HANDLE iocp;

	std::array<CLIENT, MAX_DUMMY> dummy;
	std::atomic_int dummy_count = 0;
	std::atomic_int num_connections;
};