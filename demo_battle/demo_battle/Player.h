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

	bool GetEmpty();

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

	void SetPosition(XMFLOAT3 pos);
	XMFLOAT3 GetPosition();

	void SetLook(XMFLOAT3 look);
	XMFLOAT3 GetLook();

public:
	virtual int GetAnimType() = 0;
	int GetPrevAnimType();
	void SetPrevAnimType(int animType);

protected:
	int m_id;		//=g_clients key val
	Object* m_cur;
	bool m_isEmpty;
	bool m_isReady;
	bool m_isMoveable;
	bool m_isDead;
	size_t m_coin_cnt;

	char m_idStr[MAX_ID_LEN];
	int m_mmr;
	char m_prevAnimType;

	float m_fTimeElapsedJump{};

	XMFLOAT3 m_pos;
	XMFLOAT3 m_look;
};
