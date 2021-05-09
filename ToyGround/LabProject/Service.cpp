#include "pch.h"
#include "Service.h"

void Service::Clear() {
	active = false;
}

void Service::ActiveService() {
	NetCore::GetApp()->CreateSocket(eSERVER::SV_LOBBY);
	NetCore::GetApp()->CreateSocket(eSERVER::SV_BATTLE);
			 
	NetCore::GetApp()->ConnectServer(eSERVER::SV_LOBBY);

	while (active) {
		if (lobby_active) {
			NetCore::GetApp()->RecvLobbyServer();
		}
		if (battle_active) {
			NetCore::GetApp()->RecvBattleServer();
		}
	}
}

void Service::Notify(int sEvent, int argsCount, ...) {
	switch (sEvent) {
	case EVENT_LOBBY_LOGIN_OK: {
		break;
	}
	case EVENT_LOBBY_LOGIN_FAIL: {
		break;
	}
	case EVENT_LOBBY_SIGNUP_OK: {
		break;
	}
	case EVENT_LOBBY_SIGNUP_FAIL: {
		break;
	}
	case EVENT_LOBBY_UPDATE_CLIENT_USERINFO: {
		break;
	}
	case EVENT_LOBBY_MATCH_OK: {
		break;
	}
	case EVENT_LOBBY_MATCH_CANCEL: {
		break;
	}
	case EVENT_BATTLE_ACCEPT_OK: {
		break;
	}
	case EVENT_BATTLE_ROOM_JOIN_OK: {
		break;
	}
	case EVENT_BATTLE_ROOM_JOIN_FAIL: {
		break;
	}
	case EVENT_ROOM_READY: {
		break;
	}
	case EVENT_ROOM_START_AVAILABLE: {
		break;
	}
	case EVENT_ROOM_START: {
		break;
	}
	case EVENT_GAME_TIMER: {
		break;
	}
	case EVENT_GAME_GAMEOVER: {
		break;
	}
	case EVENT_LOBBY_UPDATE_SERVER_USERINFO: {
		break;
	}
	}
}

int Service::GetMyBattleID() const {
	return NetCore::GetApp()->m_client.battle_id;
}

BattleClient* Service::GetBattleClient(int id) const {
	if (NetCore::GetApp()->m_battle_clients.count(id)) {
		return NetCore::GetApp()->m_battle_clients[id].get();
	}
	return nullptr;
}

int Service::GetBattleClientsCount() const {
	return NetCore::GetApp()->m_battle_clients.size();
}

int Service::GetWinner() const {
	return NetCore::GetApp()->m_winner;
}

int Service::GetMapInfo() const {
	return NetCore::GetApp()->m_map_info;
}
