#include "pch.h"
#include "Bullet.h"

Bullet::Bullet() {
	Initialize();
}

Bullet::~Bullet() {
	//delete m_boundaries;
	//m_boundaries = nullptr;
}

void Bullet::Initialize() {
	m_forceAmountXZ = 3.f;
	m_mass = 1.f;
	m_fricCoef = 1.f;
	m_maxVel = 3.f;
	m_isApplyGravity = false;
	m_xmf4x4World = SMathHelper::Identity4x4();
	SetVelocity(XMFLOAT3{ 0.f, 0.f, 0.f });
	SetAcceleration(XMFLOAT3{ 0.f,0.f, 0.f });

	m_isShoot = false;
	m_isCollider = false;
	m_damage = BULLET_DAMAGE;

	XMFLOAT3 size{};
	int numBB{ 1 };
	//m_boundaries->SetBB(numBB);
	//m_boundaries->SetBBPos(GetPosition(), numBB - 1);
	//m_boundaries->SetBBSize(size, numBB - 1);

	//m_boundaries->SetObjType(OBJECT_TYPE_BULLET);
}

void Bullet::Shoot(const XMFLOAT3& pos, const XMFLOAT3& look, XMFLOAT3& dir, float eTime = 10.f) {
	//player 총 부분에서 총알이 출발해야 함
	XMFLOAT3 shoot_pos{ pos };
	float scala_distance{ 30.f }; // player pivot에서 look 방향으로 떨어진 만큼
	shoot_pos.x += look.x * scala_distance;
	shoot_pos.z += look.z * scala_distance;
	shoot_pos.y += 120.f; // player pivot에서 얼마나 위에서 쏘는가

	m_xmf4x4World = SMathHelper::Identity4x4();
	SetPosition(shoot_pos); //실제 위치 변경
	SetLook(dir.x, dir.y, dir.z); //look 방향으로 쏘면 안될까
	SetShoot(true);
	SetCollider(true);

	XMFLOAT3 force = SMathHelper::Normalize(dir);

	XMStoreFloat3(&force, XMLoadFloat3(&force) * m_forceAmountXZ);
	AddForce(force, eTime, true);
}

unsigned short Bullet::GetDamage() const {
	return m_damage;
}

void Bullet::SetDamage(const unsigned short& damage) {
	m_damage = damage;
}

void Bullet::SetShoot(bool isShoot) {
	m_isShoot = isShoot;
}

bool Bullet::isShoot() {
	return m_isShoot;
}

void Bullet::SetCollider(bool isCollider) {
	m_isCollider = isCollider;
}

bool Bullet::isCollider() {
	return m_isCollider;
}

bool Bullet::isBulletActive() {
	XMFLOAT3 bullet_pos = GetPosition();

	if (bullet_pos.y <= 0) {
#ifdef LOG_ON
		std::cout << "bullet.y <= 0\n";
#endif
		return false;
	}
	else if (bullet_pos.y <= 25) {	//HEIGHT
#ifdef LOG_ON
		std::cout << "bullet.y <= 25\n";
#endif
		return false;
	}
	else if (bullet_pos.x <= -50 || bullet_pos.x >= 50) { // width
#ifdef LOG_ON
		std::cout << "bullet.x <= 0 or >= 80\n";
#endif
		return false;
	}
	else if (bullet_pos.z <= -75 || bullet_pos.z >= 75) {
#ifdef LOG_ON
		std::cout << "bullet.z <= -75 or >= 75\n";
#endif
		return false;
	}
	return true;
}