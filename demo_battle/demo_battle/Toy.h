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
	int GetAnimType() override;

private:
	unsigned short m_HP;
	unsigned short m_MaxHP;
};