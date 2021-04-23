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
	m_keyW = false;
	m_keyA = false;
	m_keyS = false;
	m_keyD = false;
	m_keyJump = false;
	m_animJump = false;

	m_HP = 100;
	m_MaxHP = 100;
	m_isMoveable = true;
	m_isDead = false;
	m_cur = nullptr;
	if (m_isReady) m_isReady = false;
}

bool Toy::Update(float elapsedTime) {
	if (m_cur == nullptr) return false;

	if (m_animJump) {
		m_fTimeElapsedJump += elapsedTime;
		if (m_fTimeElapsedJump >= ANIMTIME_JUMP) {
			m_animJump = false;
			m_fTimeElapsedJump = 0.f;
		}
	}

	XMFLOAT3 force;
	int vertical{ 0 }; //ws
	int horizontal{ 0 }; //ad

	XMFLOAT3 look = m_cur->GetLook();
	XMFLOAT3 up = m_cur->GetUp();

	if (m_keyW) { vertical++; }
	if (m_keyS) { vertical--; }
	if (m_keyA) { horizontal++; }
	if (m_keyD) { horizontal--; }
	if (m_keyJump) {
		force = MathHelper::Add(force, up, 1.f);
		m_animJump = true;
		m_keyJump = false; //anim 끝나고 false 처리?
	}
	if (vertical != 0 || horizontal != 0) {
		force = MathHelper::Add(force, look, 1.f);
	}

	MathHelper::Normalize(force);
	if (!MathHelper::IsZero(force)) {
		float curForceAmountXZ{ m_cur->GetForceAmountXZ() };
		float curForceAmountY{ m_cur->GetForceAmountY() };
		force.x *= curForceAmountXZ;
		force.y *= curForceAmountY;
		force.z *= curForceAmountXZ;
		
		m_cur->AddForce(force, elapsedTime, false);
	}

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

int Toy::GetAnimType() {
	if(m_animJump){ 
		return ANIM_JUMP;
	}
	if (!m_keyW
		&& !m_keyA
		&& !m_keyS
		&& !m_keyD
		&& !m_keyJump) {
		return ANIM_IDLE;
	}
	else {
		return ANIM_MOVE;
	}
}
