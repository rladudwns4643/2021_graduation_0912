#pragma once
#include "pch.h"

constexpr int MAX_PLAYER = 4;
constexpr int MAX_ROOM = 50;
constexpr int MAX_BUFFER = 256;
constexpr int MAX_ID_STR = 10;
constexpr int MAX_CLIENTS = 5100;

struct EX_OVER {
	WSAOVERLAPPED		over;
	WSABUF				wsabuf[1];
	char				net_buf[MAX_BUFFER];
	bool				is_recv;
};

struct CLIENT {
	SOCKET			m_s;
	EX_OVER			m_recv_over;
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