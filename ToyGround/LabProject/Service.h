#pragma once
#include "Singleton.h"
#include "SceneManager.h"
#include "netCore.h"
#include <functional>

#define EVENT_LOBBY_CONNECT_OK						0xA00
#define EVENT_LOBBY_CONNECT_FAIL					0xA01
#define EVENT_LOBBY_LOGIN_REQUEST					0xA02
#define EVENT_LOBBY_LOGIN_OK							0xA03
#define EVENT_LOBBY_LOGIN_FAIL						0xA04
#define EVENT_LOBBY_REQUEST_SIGNUP					0xA05
#define EVENT_LOBBY_SIGNUP_OK						0xA06
#define EVENT_LOBBY_SIGNUP_FAIL						0xA07
#define EVENT_LOBBY_REQUEST_USER_INFO				0xA08
#define EVENT_LOBBY_UPDATE_CLIENT_USERINFO			0xA09
#define EVENT_LOBBY_REQUEST_FIND_ROOM				0xA10
#define EVENT_LOBBY_CANCLE_FIND_ROOM				0xA11

#define EVENT_BATTLE_LOGIN_OK						0xB00
#define EVENT_BATTLE_LOGIN_FAIL						0xB01
#define EVENT_ROOM_FIND_ROOM						0xB02
#define EVENT_ROOM_JOIN_OK							0xB04
#define EVENT_ROOM_JOIN_FAIL						0xB05
#define EVENT_ROOM_ENTER							0xB06
#define EVENT_ROOM_LEAVE							0xB07
#define EVENT_ROOM_READY							0xB08
#define EVENT_ROOM_START_AVAILABLE					0xB09
#define EVENT_ROOM_START							0xB10
#define EVENT_GAME_ROUND_START						0xB11

#define EVENT_GAME_START							0xCA0
#define EVENT_GAME_CALLBACK_MOVE					0xCA1
#define EVENT_GAME_MAKE_MOVE						0xCA2
#define EVENT_GAME_CALLBACK_MOUSE					0xCA3
#define EVENT_GAME_MAKE_MOUSE						0xCA4
#define EVENT_GAME_REQUEST_BULLET					0xCA5
#define EVENT_GAME_CALLBACK_BULLET					0xCA6
#define EVENT_GAME_REMOVE_BULLET					0xCA7
#define	EVENT_GAME_ADD_COIN							0xCA8
#define EVENT_GAME_GET_COIN							0xCA9
#define EVENT_GAME_UPDATE_COIN						0xCB0
#define EVENT_GAME_WIN_SATISFACTION					0xCB1
#define EVENT_GAME_CALLBACK_HIT						0xCB2
#define EVENT_GAME_CALLBACK_DIE						0xCB3
#define EVENT_GAME_REQUEST_DIE						0xCB4
#define EVENT_GAME_CALLBACK_PUSH_ANIM				0xCB5
#define EVENT_GAME_CALLBACK_POP_ANIM				0xCB6
#define EVENT_GAME_REQUEST_PUSH_ANIM				0xCB7
#define EVENT_GAME_REQUEST_POP_ANIM					0xCB8
#define EVENT_GAME_RESPAWN							0xCB9

#define EVENT_GAME_TIMER							0xD0
#define EVENT_GAME_GAMEOVER							0xD1

#define EVENT_GAME_UPDATE_SERVER_USERINFO			0xE0 //after GAME OVER

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

	void AddEvent(int sEvent, int argsCount = 0, ...);
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