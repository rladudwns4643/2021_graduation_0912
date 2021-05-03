#pragma once
#include "pch.h"
#include "Character.h"

class Object;

class Player {
public:
	Player();
	Player(const Player& rhs);
	Player& operator=(const Player& rhs)noexcept;
	virtual ~Player();

public:
	void Initialize();
	virtual void Reset();

	virtual bool Update(float elapsedTime) = 0;

public:
	void SetCurObject(const int& spawn_pos_idx);
	Object* GetCurObject();

	void Enter();
	void Leave();

	bool GetSlotStatus();

	void SetID(const int& id);
	int GetID();

	void SetReady(const bool& ready);
	bool GetReady();

	void SetID_STR(char* name);
	char* GetID_STR();

	void SetMMR(int mmr);
	int GetMMR();

	void SetIsMoveable(const bool& moveable);
	bool IsMoveable();

	void SetDead(const bool& dead);
	bool IsDead();

	void SetCoin(size_t coin);
	size_t GetCoin();

public:
	virtual int GetAnimType() = 0;
	int GetPrevAnimType();
	void SetPrevAnimType(int animType);

public:
	void SetKeyW(bool input);
	void SetKeyS(bool input);
	void SetKeyA(bool input);
	void SetKeyD(bool input);

	void SetKeyJump(bool input);

	bool GetKeyW();
	bool GetKeyA();
	bool GetKeyS();
	bool GetKeyD();
	bool GetKeyJump();

protected:
	int m_id;		//=g_clients key val
	Object* m_cur;
	bool m_isEmpty;
	bool m_isReady;
	bool m_isMoveable;
	bool m_isDead;
	size_t m_coin_cnt;

	char m_idStr[MAX_STR_LEN];
	int m_mmr;

	bool m_keyW{ false };
	bool m_keyA{ false };
	bool m_keyS{ false };
	bool m_keyD{ false };
	bool m_keyJump{ false };
	bool m_animJump{ false };

	char m_prevAnimType;

	float m_fTimeElapsedJump{};
};
