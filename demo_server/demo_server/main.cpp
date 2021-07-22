#include "pch.h"
#include "Global.h"
#include "LobbyServer.h"
#include "User.h"
#include "DataBase.h"

LobbyServer* g_lobbyServer;
DataBase* g_database;
std::atomic_int new_user_id{ 0 };

int main() {

	wcout.imbue(std::locale("korean"));

	g_lobbyServer = new LobbyServer(LOBBY_ID);
	g_database = DataBase::GetInstance();
	g_lobbyServer->BattleServerAccept();

	std::thread worker_thread(&LobbyServer::DoWorker, g_lobbyServer);

	while (true) {
		g_lobbyServer->ClientAccept(new_user_id + 1);
		++new_user_id;
	}

	worker_thread.join();

	delete g_lobbyServer;
	DataBase::DeleteInstance();
}