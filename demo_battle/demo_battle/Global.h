#pragma once
#include "SMathHelper.h"

constexpr int NO_MSG = -1;

constexpr int EVENT_KEY = 10000;
constexpr int LOBBY_SERVER_KEY = 0;

constexpr int MAX_PLAYER = 2;
constexpr int MAX_BULLET = 30;
constexpr int MAX_ROOM = 50;
constexpr int MAX_BUFFER = 256;
constexpr int MAX_ID_STR = 10;
constexpr int MAX_CLIENT = 5100;

constexpr size_t MAX_LEFT_TIME = 60 + 1; // 60초만(중간발표용)
constexpr size_t COUNTDOWN_TIME = 5;
constexpr size_t WIN_COIN_CNT = 10;

constexpr int WINNER_PLUS_MMR = 50;
constexpr int LOOSER_MIN_MMR = -40;

constexpr float GRAVITY = 0.980f;

//animtime
constexpr float ANIMTIME_JUMP = 500.f; //임시

constexpr float ATTACK_DELAY = 500.f;

enum EVENT_TYPE {
	EV_RECV, EV_SEND,
	EV_UPDATE, EV_TICK, EV_FLUSH_MSG,
	EV_MOVE_ENABLE, EV_MAKE_MOVE_DISABLE, EV_ADD_COIN,
	EV_UPDATE_DB,
	EV_RESET_ROOM,
	EV_WORLD_UPDATE,
	EV_TYPE_COUNT
};

struct SEND_INFO {
	int to;
	char buff[MAX_BUFFER];
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
	int target; // room_no
	std::chrono::high_resolution_clock::time_point event_time;
	EVENT_TYPE et;
	constexpr bool operator<(const EVENT& rhs) const { return event_time > rhs.event_time; } //event event_time 가 오래된 것 먼저 g_timer_queue 정렬된다(less<>)
};

struct message {
	int id;
	char type;
	XMFLOAT3 vec;
	int anim_type;
};
