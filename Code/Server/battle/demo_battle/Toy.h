#pragma once
#include "Player.h"
#include "extern.h"
#include "Global.h"

class Toy : public Player {
public:
	Toy();
	Toy(const Player& rhs) : Player(rhs) { Initialize(); }
	~Toy() override;

	void Initialize();
	void Reset() override;
	bool Update(float elapsedTime) override;

public:
	void SetHP(const unsigned short& hp);
	unsigned short GetHP() const;

	void SetMaxHP(const unsigned short& max_hp);
	unsigned short GetMaxHP() const;

public:
	void SetIsShoot(bool shoot);
	int GetAnimType() override;
	void SetAnimType(int anim);

private:
	unsigned short m_HP;
	unsigned short m_MaxHP;
	bool m_isShoot{ false };
	bool m_animJump{ false };
	float m_fAttackTime{};
	int m_anim;
};