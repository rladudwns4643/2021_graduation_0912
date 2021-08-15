#include "pch.h"
#include "Global.h"
#include "battleServer.h"
#include "Room.h"
#include "Map.h"
#include "SpawnPosition.h"
#include "Boundary.h"

#define LOG_ON

namespace SR {
	HANDLE g_iocp;
	std::unordered_map<int, Room*> g_rooms;
	std::array<int, MAX_ROOM> g_room_no;
	std::array<CLIENT*, MAX_CLIENT> g_clients;
	std::priority_queue<EVENT> g_timer_queue;		//우선순위 queue, 내림차순 기준

	Map g_map;
	std::map<int, Boundary*> g_boundaries;
	SpawnPosition g_spawn_pos;
}

namespace ATOMIC {
	std::mutex g_timer_lock;
	std::mutex g_msg_lock;
	std::mutex g_room_no_lock;
	std::mutex g_room_lock;
	std::mutex g_clients_lock;

	std::atomic_int g_RoomNum;

	std::mutex g_dbInfo_lock;
}


void CreatePlayerSlot(int maxslot) {
#ifdef LOG_ON
	cout << "CreatePlayerSlot start" << endl;
#endif
	for (int user_id = 0; user_id < maxslot; ++user_id) {
		CLIENT* player = new CLIENT;
		player->m_recv_over.wsabuf[0].len = MAX_BUFFER;
		player->m_recv_over.wsabuf[0].buf = player->m_recv_over.net_buf;
		player->user_id = user_id;
		player->room_id = -1; //미 설정
		ZeroMemory(player->packet_buf, MAX_BUFFER);
		ZeroMemory(player->id_str, sizeof(char) * MAX_ID_STR);
		SR::g_clients[user_id] = player;
	}
#ifdef LOG_ON
	cout << "CreatePlayerSlot compleat" << endl;
#endif	
}

int main() {
	cout << "BATTLE" << endl;
	std::wcout.imbue(std::locale("korean"));
	srand(time(NULL));

	CreatePlayerSlot(MAX_CLIENT);

	Boundary* t_b = new Boundary;
	
	SR::g_boundaries[OBJECT_TYPE_TOY] = t_b;
	SR::g_map.LoadMapInfo("FirstMap");
	BattleServer* battleserver = BattleServer::GetInstance();
	
	battleserver->ConncetLobbyServer();
	battleserver->Run();

}

