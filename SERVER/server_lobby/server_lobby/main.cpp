#include "pch.h"
#include "Global.h"
#include "Lobby.h"
#include "User.h"

Lobby* g_lobbyServer;

std::atomic_int new_user_id{ 0 };

int main() {

	wcout.imbue(std::locale("korean"));

	g_lobbyServer = new Lobby(LOBBY_ID);
	g_lobbyServer->BattleServerAccept();

	std::thread worker_thread(&Lobby::do_worker, g_lobbyServer);

	while (true) {
		g_lobbyServer->ClientAccept(new_user_id + 1);
		++new_user_id;
	}

	worker_thread.join();

	delete g_lobbyServer;
}