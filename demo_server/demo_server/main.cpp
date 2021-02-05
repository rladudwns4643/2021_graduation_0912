#include "pch.h"
#include "Global.h"
#include "LobbyServer.h"
#include "User.h"

LobbyServer* g_lobbyServer;

std::atomic_int new_user_id{ 0 };	//0Àº ºñ¿öµÒ

int main() {
	g_lobbyServer = new LobbyServer(LOBBY_ID);
	
	std::thread worker_thread(&LobbyServer::DoWorker, g_lobbyServer);

	while (true) {
		g_lobbyServer->ClinetAccept(new_user_id + 1);
		++new_user_id;
	}

	worker_thread.join();

	delete g_lobbyServer;
}