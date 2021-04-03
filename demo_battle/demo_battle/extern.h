#pragma once
#include "pch.h"
#include "Global.h"

namespace SHARED_RESOURCE {
	extern HANDLE g_iocp;
	//std::array<Room, MAX_ROOM> g_rooms;
	extern std::array<CLIENT*, MAX_CLIENT> g_clients;
	extern std::priority_queue<EVENT> g_timer_queue;		//우선순위 queue, 내림차순 기준
}

namespace ATOMIC {
	extern std::mutex g_timer_lock;
	extern std::mutex g_room_lock;
	extern std::mutex g_clients_lock;
	extern std::mutex g_msg_lock;
}
