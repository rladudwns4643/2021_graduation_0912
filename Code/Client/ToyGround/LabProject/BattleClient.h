#pragma once
#include "pch.h"

class BattleClient {
	friend class NetCore;
	friend class Service;
	friend class GameroomScene;

	int m_battleID;

	char name[MAX_ID_LEN];
	int mmr{ -1 };

public:
	BattleClient(int battle_id);
	virtual ~BattleClient();

	void Initialize();

private:
	int m_player_num; //id
	XMFLOAT4 m_spawn_position;

	int m_room_num;
	bool m_host;
	bool m_ready;
};