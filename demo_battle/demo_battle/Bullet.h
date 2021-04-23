#pragma once
#include "Object.h"

#define BULLET_DAMAGE 20

class Bullet : public Object {
public:
	Bullet();
	~Bullet();

public:
	void Initialize();
	void Shoot(const XMFLOAT3& pos, const XMFLOAT3& look, XMFLOAT3& dir, double eTime);

public:
	unsigned short GetDamage() const;
	void SetDamage(const unsigned short& damage);

	void SetShoot(bool isShoot);
	bool isShoot();
	void SetCollider(bool isCollider);
	bool isCollider();

	//in map
	bool isBulletActive();

private:
	unsigned short		m_damage;
	bool				m_isShoot;
	bool				m_isCollider;
};