#pragma once
#include "pch.h"
#include "Global.h"
#include "Map.h"
#include "SpawnPosition.h"

class Room;
//SHARED RESOURCE
namespace SR {
	extern HANDLE g_iocp;
	extern std::unordered_map<int, Room*> g_rooms;
	extern std::array<int, MAX_ROOM> g_room_no;
	extern std::array<CLIENT*, MAX_CLIENT> g_clients;
	extern std::priority_queue<EVENT> g_timer_queue;		//우선순위 queue, 내림차순 기준

	extern Map g_map;
	extern std::map<int, Boundary*> g_boundaries;
	extern SpawnPosition g_spawn_pos;
}

namespace ATOMIC {
	extern std::mutex g_timer_lock;
	extern std::mutex g_msg_lock;
	extern std::mutex g_room_no_lock;
	extern std::mutex g_room_lock;
	extern std::mutex g_clients_lock;

	extern std::atomic_int g_RoomNum;

	extern std::mutex g_dbInfo_lock;
}
