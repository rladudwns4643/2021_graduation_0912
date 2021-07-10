#include "pch.h"
#include "Toy.h"

Toy::Toy() {
	Initialize();
}

Toy::~Toy() {
	if (m_cur != nullptr) {
		delete m_cur;
	}
	m_cur = nullptr;
}

void Toy::Initialize() {
	m_HP = 100;
	m_MaxHP = 100;
	m_isMoveable = true;
	m_isDead = false;
	m_cur = nullptr;
	if (m_isReady) m_isReady = false;
}

void Toy::Reset() {

	m_HP = 100;
	m_MaxHP = 100;
	m_isMoveable = true;
	m_isDead = false;
	m_cur = nullptr;
	if (m_isReady) m_isReady = false;
}

bool Toy::Update(float elapsedTime) {
	if (m_cur == nullptr) return false;

	//if (m_animJump) {
	//	m_fTimeElapsedJump += elapsedTime;
	//	if (m_fTimeElapsedJump >= ANIMTIME_JUMP) {
	//		m_animJump = false;
	//		m_fTimeElapsedJump = 0.f;
	//	}
	//}
	//
	//if (m_isShoot) {
	//	m_fAttackTime += elapsedTime;
	//	if (m_fAttackTime >= ATTACK_DELAY) {
	//		m_isShoot = false;
	//		m_fAttackTime;
	//	}
	//}
	//
	//XMFLOAT3 force{};
	//
	//XMFLOAT3 look = m_cur->GetLook();
	//XMFLOAT3 up = m_cur->GetUp();
	//XMFLOAT3 right = m_cur->GetRight();
	//
	//if (m_keyW) { force = SMathHelper::Add(force, look, 1.f); }
	//if (m_keyS) { force = SMathHelper::Add(force, look, -1.f); }
	//if (m_keyA) { force = SMathHelper::Add(force, right, -1.f); }
	//if (m_keyD) { force = SMathHelper::Add(force, right, 1.f); }
	//if (m_keyJump) {
	//	force = SMathHelper::Add(force, up, 1.f);
	//	m_animJump = true;
	//	m_keyJump = false; //anim 끝나고 false 처리?
	//}
	//
	//SMathHelper::Normalize(force);
	//if (!SMathHelper::IsZero(force)) {
	//	float curForceAmountXZ{ m_cur->GetForceAmountXZ() };
	//	float curForceAmountY{ m_cur->GetForceAmountY() };
	//	force.x *= curForceAmountXZ;
	//	force.y *= curForceAmountY;
	//	force.z *= curForceAmountXZ;
	//	
	//	m_cur->AddForce(force, elapsedTime, false);
	//}

	return m_cur->Update(elapsedTime, true);
}

void Toy::SetHP(const unsigned short& hp) {
	m_HP = hp;
}

unsigned short Toy::GetHP() const {
	return m_HP;
}

void Toy::SetMaxHP(const unsigned short& max_hp) {
	m_MaxHP = max_hp;
}

unsigned short Toy::GetMaxHP() const {
	return m_MaxHP;
}

void Toy::SetIsShoot(bool shoot) {
	m_isShoot = shoot;
}

int Toy::GetAnimType() {
	return m_anim;
}

void Toy::SetAnimType(int anim)  {
	m_anim = anim;
}
