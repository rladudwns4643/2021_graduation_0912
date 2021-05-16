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

void Service::AddEvent(int sEvent, int argsCount, ...) {
	switch (sEvent) {
	//lobby
	case EVENT_LOBBY_CONNECT_OK: {
		lobby_active = true;
		break;
	}
	case EVENT_LOBBY_CONNECT_FAIL: {
		lobby_active = false;
		NetCore::GetApp()->DisconnectServer(eSERVER::SV_LOBBY);
		break;
	}
	case EVENT_LOBBY_LOGIN_REQUEST: {
		std::string id{};
		std::string pw{};
		NetCore::GetApp()->SendLobbyLoginPacket(id, pw);
		break;
	}
	case EVENT_LOBBY_LOGIN_OK: { //LC_LOGIN_OK
		SceneManager::GetApp()->SendEventArgs(SceneType::eLobby, EVENT_LOBBY_LOGIN_OK);
		break;
	}
	case EVENT_LOBBY_LOGIN_FAIL: { //LC_LOGIN_FAIL
		SceneManager::GetApp()->SendEventArgs(SceneType::eLobby, EVENT_LOBBY_LOGIN_FAIL);
		break;
	}
	case EVENT_LOBBY_SIGNUP_OK: { //LC_SIGNUP_OK
		SceneManager::GetApp()->SendEventArgs(SceneType::eLobby, EVENT_LOBBY_SIGNUP_OK);
		break;
	}
	case EVENT_LOBBY_SIGNUP_FAIL: { //LC_SIGNUP_FAIL
		SceneManager::GetApp()->SendEventArgs(SceneType::eLobby, EVENT_LOBBY_SIGNUP_FAIL);
		break;
	}
	case EVENT_LOBBY_REQUEST_USER_INFO: {
		NetCore::GetApp()->SendRequestUserInfoPacket();
		break;
	}
	case EVENT_LOBBY_UPDATE_CLIENT_USERINFO: { //LC_USERINFO
		int mmr;
		va_list ap;
		va_start(ap, argsCount);
		mmr = va_arg(ap, int);
		va_end(ap);

		SceneManager::GetApp()->SendEventArgs(SceneType::eLobby, EVENT_LOBBY_UPDATE_CLIENT_USERINFO, 1, mmr);
		break;
	}
	case EVENT_LOBBY_REQUEST_FIND_ROOM: {
		NetCore::GetApp()->ConnectServer(SV_BATTLE);
		NetCore::GetApp()->SendBattleLoginPacket();
		battle_active = true;
		break;
	}
	case EVENT_LOBBY_CANCLE_FIND_ROOM: {
		break;
	}
	//battle ( = lobby Scene에서 안바뀜 일단)
	case EVENT_BATTLE_LOGIN_OK: { //BC_BATTLE_LOGIN_OK
		SceneManager::GetApp()->SendEventArgs(SceneType::eLobby, EVENT_BATTLE_LOGIN_OK);
		break;
	}
	case EVENT_BATTLE_LOGIN_FAIL: {
		break;
	}
	case EVENT_ROOM_FIND_ROOM: {
		//방찾으면 바로 join 보내기
		NetCore::GetApp()->SendBattleRoomJoinPacket();
		//SceneManager::GetApp()->SendEventArgs(SceneType::eLobby, EVENT_ROOM_FIND_ROOM);
		break;
	}
	case EVENT_ROOM_JOIN_OK: {
		SceneManager::GetApp()->SendEventArgs(SceneType::eLobby, EVENT_ROOM_JOIN_OK);
		break;
	}
	case EVENT_ROOM_JOIN_FAIL: { //BC_JOIN_FAIL, 미구현
		break;
	}
	case EVENT_ROOM_ENTER: {
		int arg_id{};
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		va_end(ap);
		SceneManager::GetApp()->SendEventArgs(SceneType::eLobby, EVENT_ROOM_ENTER, argsCount, arg_id);
		break;
	}
	case EVENT_ROOM_LEAVE: {
		int leaver{};
		va_list ap;
		va_start(ap, argsCount);
		leaver = va_arg(ap, int);
		va_end(ap);
		SceneManager::GetApp()->SendEventArgs(SceneType::eLobby, EVENT_ROOM_LEAVE, 1, leaver);
	}
	case EVENT_ROOM_READY: { //BC_READY
		int id;
		bool ready;
		va_list ap;
		va_start(ap, argsCount);
		id = va_arg(ap, int);
		ready = va_arg(ap, bool);
		va_end(ap);
		SceneManager::GetApp()->SendEventArgs(SceneType::eLobby, EVENT_ROOM_READY, 2, id, ready);
		break;
	}
	case EVENT_ROOM_START_AVAILABLE: { //BC_GAME_START_AVAILABLE
		bool available;
		va_list ap;
		va_start(ap, argsCount);
		available = va_arg(ap, bool);
		va_end(ap);
		SceneManager::GetApp()->SendEventArgs(SceneType::eLobby, EVENT_ROOM_START_AVAILABLE, 1, available);
		break;
	}
	//////////////////ingame
	case EVENT_ROOM_START: {
		SceneManager::GetApp()->SendEventArgs(SceneType::eGamePlay, EVENT_ROOM_START);
		break;
	}
	case EVENT_GAME_START: {
		std::lock_guard<std::mutex> lock(m_mutex_event);
		for (auto& bc : NetCore::GetApp()->m_battle_clients) {
			SceneManager::GetApp()->SendEventArgs(SceneType::eGamePlay, EVENT_GAME_START, 2, bc.second->m_battleID, bc.second->m_spawn_position);
		}
		break;
	}
	case EVENT_GAME_CALLBACK_MOVE: {
		int arg_id;
		XMFLOAT3 arg_pos;

		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		arg_pos = va_arg(ap, XMFLOAT3);
		va_end(ap);

		SceneManager::GetApp()->SendEventArgs(SceneType::eGamePlay, EVENT_GAME_CALLBACK_MOVE, argsCount, arg_id, arg_pos);
		break;
	}
	case EVENT_GAME_MAKE_MOVE: {
		XMFLOAT3 arg_pos;

		va_list ap;
		va_start(ap, argsCount);
		arg_pos = va_arg(ap, XMFLOAT3);
		va_end(ap);

		NetCore::GetApp()->SendPositionPacket(arg_pos);
		break;
	}
	case EVENT_GAME_CALLBACK_MOUSE: {
		int arg_id;
		XMFLOAT3 arg_look;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		arg_look = va_arg(ap, XMFLOAT3);
		va_end(ap);

		SceneManager::GetApp()->SendEventArgs(SceneType::eGamePlay, EVENT_GAME_CALLBACK_MOUSE, argsCount, arg_id, arg_look);
		break;
	}
	case EVENT_GAME_MAKE_MOUSE: {
		XMFLOAT3 arg_look;
		va_list ap;
		va_start(ap, argsCount);
		arg_look = va_arg(ap, XMFLOAT3);
		va_end(ap);

		NetCore::GetApp()->SendLookVectorPacket(arg_look);
		break;
	}
	case EVENT_GAME_MAKE_ANIM: {
		char arg_anim_type;
		va_list ap;
		va_start(ap, argsCount);
		arg_anim_type = va_arg(ap, char);
		va_end(ap);
		cout << "MAKE_ANIM" << endl;
		NetCore::GetApp()->SendAnimPacket(arg_anim_type);
		break;
	}
	case EVENT_GAME_CALLBACK_ANIM: {
		int arg_id;
		int arg_anim_type;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		arg_anim_type = va_arg(ap, int);
		va_end(ap);

		cout << "CALLBACK_ANIM" << endl;
		SceneManager::GetApp()->SendEventArgs(SceneType::eGamePlay, EVENT_GAME_CALLBACK_ANIM, argsCount, arg_id, arg_anim_type);
		break;
	}
	case EVENT_GAME_SHOOT_BULLET: {
		int arg_bullet_id;
		XMFLOAT3 arg_start_pos;
		va_list ap;
		va_start(ap, argsCount);
		arg_bullet_id = va_arg(ap, int);
		arg_start_pos = va_arg(ap, XMFLOAT3);
		va_end(ap);

		SceneManager::GetApp()->SendEventArgs(SceneType::eGamePlay, EVENT_GAME_SHOOT_BULLET, argsCount, arg_bullet_id, arg_start_pos);
		break;
	}
	case EVENT_GAME_REMOVE_BULLET: {
		int arg_bullet_id;
		va_list ap;
		va_start(ap, argsCount);
		arg_bullet_id = va_arg(ap, int);
		va_end(ap);
		SceneManager::GetApp()->SendEventArgs(SceneType::eGamePlay, EVENT_GAME_REMOVE_BULLET, argsCount, arg_bullet_id);
		break;
	}
	case EVENT_GAME_HIT: {
		int arg_id;
		float arg_hp;

		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		arg_hp = va_arg(ap, float);
		va_end(ap);

		SceneManager::GetApp()->SendEventArgs(SceneType::eGamePlay, EVENT_GAME_HIT, argsCount, arg_id, arg_hp);
		break;
	}
	case EVENT_GAME_DIE: {
		int arg_id;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		va_end(ap);

		SceneManager::GetApp()->SendEventArgs(SceneType::eGamePlay, EVENT_GAME_DIE, argsCount, arg_id);
		break;
	}
	case EVENT_GAME_TIMER: { //BC_LEFT_TIME
		int t;
		va_list ap;
		va_start(ap, argsCount);
		t = va_arg(ap, int);
		va_end(ap);

		SceneManager::GetApp()->SendEventArgs(SceneType::eGamePlay, EVENT_GAME_TIMER, argsCount, t);
		break;
	}
	case EVENT_GAME_ADD_COIN: {
		XMFLOAT3 arg_pos;
		va_list ap;
		va_start(ap, argsCount);
		arg_pos = va_arg(ap, XMFLOAT3);
		va_end(ap);

		SceneManager::GetApp()->SendEventArgs(SceneType::eGamePlay, EVENT_GAME_ADD_COIN, argsCount, arg_pos);
		break;
	}
	case EVENT_GAME_GAMEOVER: { //BC_GAME_OVER
		SceneManager::GetApp()->SendEventArgs(SceneType::eGamePlay, EVENT_GAME_GAMEOVER);
		break;
	}
	case EVENT_GAME_UPDATE_SERVER_USERINFO: { //BC_UPDATE_USER_INFO
		break;
	}
	//-------------------------------------------------
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
