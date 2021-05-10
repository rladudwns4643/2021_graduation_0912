#pragma once
#include "Singleton.h"
#include "SceneManager.h"
#include "netCore.h"
#include <functional>

#define EVENT_LOBBY_CONNECT_OK						0xA0
#define EVENT_LOBBY_CONNECT_FAIL					0xA1
#define EVENT_LOBBY_LOGIN_REQUEST					0xA2
#define EVENT_LOBBY_LOGIN_OK						0xA3
#define EVENT_LOBBY_LOGIN_FAIL						0xA4
#define EVENT_LOBBY_SIGNUP_OK						0xA5
#define EVENT_LOBBY_SIGNUP_FAIL						0xA6
#define EVENT_LOBBY_UPDATE_CLIENT_USERINFO			0xA7
#define EVENT_LOBBY_MATCH_START						0xA8
#define EVENT_LOBBY_MATCH_CANCEL					0xA9

#define EVENT_BATTLE_ACCEPT_OK						0xB0
#define EVENT_BATTLE_ROOM_JOIN_OK					0xB1
#define EVENT_BATTLE_ROOM_JOIN_FAIL					0xB2

#define EVENT_ROOM_READY							0xC0
#define EVENT_ROOM_START_AVAILABLE					0xC1
#define EVENT_ROOM_START							0xC2

#define EVENT_GAME_CALLBACK_MOVE					0xD0
#define EVENT_GAME_MAKE_MOVE						0xD1
#define EVENT_GAME_CALLBACK_MOUSE					0xD2
#define EVENT_GAME_MAKE_MOUSE						0xD3
#define EVENT_GAME_SHOOT_BULLET						0xD4
#define EVENT_GAME_REMOVE_BULLET					0xD5
#define EVENT_GAME_HIT								0xD6
#define EVENT_GAME_DIE								0xD7
#define EVENT_GAME_ANIM								0xD8


#define EVENT_GAME_TIMER							0xE0
#define EVENT_GAME_GAMEOVER							0xE1

#define EVENT_GAME_UPDATE_SERVER_USERINFO			0xF0 //GAME OVER

class Service : public TemplateSingleton<Service> {
private:
	bool active = true;
	bool lobby_active = false;
	bool battle_active = false;

private:
	XMFLOAT3 arg_look = { 0.f, 1.f, 0.f };
	XMFLOAT3 arg_look_callback = { 0.f, 1.f, 0.f };

public:
	void Clear();
	void ActiveService();

	void Notify(int sEvent, int argsCount = 0, ...);
	void setConnectLobby(bool connect) { lobby_active = connect; }
	bool isConnectLobby() const { return lobby_active; }
	bool isConnectBattle() const { return battle_active; }

public:
	int GetMyBattleID() const;
	BattleClient* GetBattleClient(int id) const;
	int GetBattleClientsCount() const;
	int GetWinner() const;
	int GetMapInfo() const;


private:
	std::mutex m_mutex_event;
};