#pragma once
#include "pch.h"

#define LOG_ON

constexpr int EVENT_KEY = 10000;
constexpr int MATCH_MAKING_SERVER_KEY = 0;
constexpr int UDP_SOCKET_KEY = 1;

constexpr int MAX_PLAYER = 4;
constexpr int MAX_ROOM = 50;
constexpr int MAX_BUFFER = 256;
constexpr int MAX_ID_STR = 10;
constexpr int MAX_CLIENTS = 5100;

namespace SHARED_RESOURCE {
	HANDLE iocp;
	std::array<Room, MAX_ROOM> rooms;
	std::array<CLIENT*, MAX_CLIENT> clients;

	int spawnX;
	int spawnY;
}

namespace ATOMIC {
	std::mutex Room_lock;
	std::mutex Clients_lock;
}

enum EVENT_TYPE {
	EV_TCP_RECV, EV_SEND,
	EV_UPDATE, EV_TICK, EV_FLUSH_MSG,
	EV_PENALTY_CHECK, EV_PENALTY_BEGIN, EV_PENALTY_END,
	EV_MOVE_ENABLE, EV_MAKE_MOVE_DISABLE,
	EV_UPDATE_DB,
	EV_RESET_ROOM,
	EV_FULL_WORLD_UPDATE_END,
	EV_THUNDERBOLT_SET_NOT_USE,
	EV_TYPE_COUNT
};

struct EX_OVER {
	WSAOVERLAPPED		over;
	WSABUF				wsabuf[1];
	char				net_buf[MAX_BUFFER];
	EVENT_TYPE			ev_type;
};

struct CLIENT {
	EX_OVER			m_recv_over;
	TCPSocketPtr	m_s;
	SOCKADDR		m_addr;
	unsigned int	user_id;
	unsigned int	room_id;

	bool			isConnected = false;

	unsigned char packet_buf[MAX_BUFFER];
	int prev_packet_data;
	int curr_packet_size;

	char id_str[10];
	int mmr;
	int totalGameCnt;
	int winningGameCnt;
};

struct EVENT {
	int id;
	int target; //
	std::chrono::high_resolution_clock::time_point start_time;
	//EVENT_TYPE et;
	constexpr bool operator<(const EVENT& rhs) const { return start_time > rhs.start_time; }
};

struct message {
	int id;
	char type;
	PTC_VECTOR vec;
};