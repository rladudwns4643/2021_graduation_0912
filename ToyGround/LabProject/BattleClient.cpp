#include "pch.h"
#include "BattleClient.h"

BattleClient::BattleClient(int battle_ID) {
	m_battleID = battle_ID;
	m_player_num = -1;
	Initialize();
}

BattleClient::~BattleClient() {}

void BattleClient::Initialize() {
	m_player_num = -1;
	m_room_num = -1;
	m_spawn_position = XMFLOAT4(-1.f, -1.f, -1.f, -1.f);
	m_host = false;
	m_ready = false;
	mmr = -1;
}