#include "pch.h"
#include "Character.h"
#include "Boundary.h"
#include "extern.h"
#include "SpawnPosition.h"

//class SpawnPosition;

Character::Character(const int& spawn_pos_idx) {
	Initialize();
}

Character::Character(Object& cur, const Boundary* new_bb) {
	Initialize();

	m_xmf4x4World = SMathHelper::Identity4x4();

	m_xmfAcc = cur.GetAcceleration();
	m_xmfVel = cur.GetVelocity();

	int numofBB = new_bb->GetNumOfBB();
}

Character::~Character() {
}

void Character::Initialize() {
	m_xmf4x4World = SMathHelper::Identity4x4();
	m_xmfAcc = {};
	m_xmfVel = {};

	m_forceAmountXZ = 10.f;
	m_forceAmountY = 5.f;
	m_fricCoef = 0.03f;
	m_maxVel = 0.15f;		//이동속도
	m_mass = 50.f;
}
