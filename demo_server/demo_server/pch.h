#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <random>
#include <array>
#include <map>

using namespace std;
#include <WS2tcpip.h>
//#include <MSWSock.h>
#pragma comment(lib, "Ws2_32.lib")
//#pragma comment(lib, "mswsock.lib")

#define MMRDEFAULT 1000

constexpr int LOBBY_ID = 0;
constexpr int MAX_BUF_SIZE = 1024;
constexpr int MAX_PLAYER = 5000;
constexpr int MAX_PACKET_SIZE = 255;

constexpr int mmr_hard_gap = 100;

#include "User.h"

typedef int ID;
struct MatchInfo {
	int mmr;
	chrono::steady_clock::time_point wait_time;
	constexpr bool operator<(const MatchInfo& rhs) const { return wait_time > rhs.wait_time; }
	MatchInfo(int _mmr, chrono::high_resolution_clock::time_point _wait_time) : mmr(_mmr), wait_time(_wait_time) {}
};

extern std::atomic<int> new_user_id;

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