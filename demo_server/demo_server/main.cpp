#include "pch.h"
#include "Global.h"
#include "LobbyServer.h"
#include "User.h"

LobbyServer* g_lobbyServer;

std::atomic_int new_user_id{ 0 };	//0Àº ºñ¿öµÒ

int main() {
	g_lobbyServer = new LobbyServer(LOBBY_ID);

	g_lobbyServer->ClinetAccept(new_user_id + 1);

	std::thread worker_thread(&LobbyServer::DoWorker, g_lobbyServer);

	worker_thread.join();

	delete g_lobbyServer;
}