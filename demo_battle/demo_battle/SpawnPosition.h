#pragma once 
#include "Global.h"

class SpawnPosition {
public:
	enum ePositionType{SPAWN_1P, SPAWN_2P, SPAWN_COUNT};

public:
	SpawnPosition() = default;
	void SetSpawnPosition(ePositionType ty, const XMFLOAT4& val) {
		m_spawn_pos[ty] = val;
	}
	XMFLOAT4 GetSpawnPosition(ePositionType ty) {
		return m_spawn_pos[ty];
	}

public:
	std::array<XMFLOAT4, SPAWN_COUNT> m_spawn_pos;
};