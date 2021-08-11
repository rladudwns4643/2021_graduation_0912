#include "pch.h"
#include "LobbyServer.h"
#include "TimerThread.h"
#include "User.h"
#include "DataBase.h"

std::atomic_int new_user_id{ 0 };

namespace SR {
	std::map<ID, MatchInfo> g_match_list;
	SOCKET g_battleSocket;
}

namespace ATOMIC {
	std::mutex g_match_list_lock;
}

int main() {

	wcout.imbue(std::locale("korean"));
	DataBase* g_database = DataBase::GetInstance();

	LobbyServer* g_lobbyServer = new LobbyServer();
	g_lobbyServer->BattleServerAccept();
	
	std::thread worker_thread(&LobbyServer::DoWorker, g_lobbyServer);

	TimerThread* timer_thread = new TimerThread();
	timer_thread->InitThread();

	
	while (true) {
		g_lobbyServer->ClientAccept(new_user_id + 1);
		++new_user_id;
	}
	
	worker_thread.join();
	
	delete g_lobbyServer;
	DataBase::DeleteInstance();
}