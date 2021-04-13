#include "pch.h"
#include "Global.h"
#include "battleServer.h"

#define LOG_ON

namespace SR {
	HANDLE g_iocp;
	//std::array<Room, MAX_ROOM> g_rooms;
	std::array<CLIENT*, MAX_CLIENT> g_clients;
	std::priority_queue<EVENT> g_timer_queue;		//우선순위 queue, 내림차순 기준
}

namespace ATOMIC {
	std::mutex g_timer_lock;
	std::mutex g_room_lock;
	std::mutex g_clients_lock;
	std::mutex g_msg_lock;
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

	std::wcout.imbue(std::locale("korean"));

	CreatePlayerSlot(MAX_CLIENT);
	BattleServer* battleserver = BattleServer::GetInstance();

	battleserver->ConncetLobbyServer();
	battleserver->Run();
}

