#include "pch.h"
//#include "Room.h"
#include "Global.h"
#include "battleServer.h"

#define LOG_ON

int main() {

	std::wcout.imbue(std::locale("korean"));

	CreatePlayerSlot(MAX_CLIENTS);
	BattleServer* battleserver = BattleServer::GetInstance();

	battleserver->AcceptLobbyServer();
	//battleserver->Run();
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
		player->room_id = -1; //¹Ì ¼³Á¤
		ZeroMemory(player->packet_buf, MAX_BUFFER);
		ZeroMemory(player->id_str, sizeof(char) * MAX_ID_STR);
		SHARED_RESOURCE::g_clients[user_id] = player;
	}
#ifdef LOG_ON
	cout << "CreatePlayerSlot compleat" << endl;
#endif	
}