#pragma once 
#include "Global.h"

class SpawnPosition {
public:
	enum ePositionType{SPAWN_1P, SPAWN_2P, SPAWN_COUNT};

public:
	SpawnPosition() = default;

public:
	std::array<XMFLOAT4, SPAWN_COUNT> m_spawn_pos;
};