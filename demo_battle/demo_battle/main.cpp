#include "pch.h"
#include "Room.h"
#include "Global.h"

namespace SHARED_RESOURCE {
	HANDLE iocp;
	std::array<Room, MAX_ROOM> rooms;
	std::array<CLIENT, MAX_CLIENT> clients;

	int spawnX;
	int spawnY;
}

namespace ATOMIC {
	std::mutex Room_lock;
	std::mutex Clients_lock;
}