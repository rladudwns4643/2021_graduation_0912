#pragma once
#include "Global.h"
#include "MathHelper.h"
#include "Boundary.h"
#include "pch.h"

class Boundary;
class Object {
public:
	Object();
	virtual ~Object() {};

	explicit Object(const Object& other) noexcept;
	Object& operator=(const Object& other);

public:
	virtual bool Update(float elapsedTime, bool is_player);
	bool BulletUpdate(float elapsedTime);
	void AddForce(XMFLOAT3 force, float elapsedTime, bool isBullet);

public:
	XMFLOAT4X4 GetMatrix() const;

	XMFLOAT3 GetPosition() const;
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmfPosition);

	XMFLOAT3 GetPrePosition()const;
	void SetPrePosition(XMFLOAT3 xmfPosition);


	XMFLOAT3 GetLook() const;
	void SetLook(const float& x, const float& y, const float& z);
	XMFLOAT3 GetPreLook() const;
	void SetPreLook(XMFLOAT3 look);
	XMFLOAT3 GetUp() const;
	void SetUp(const float& x, const float& y, const float& z);
	XMFLOAT3 GetRight() const;
	void SetRight(const float& x, const float& y, const float& z);

	XMFLOAT3 GetRotation() const;
	XMFLOAT4 GetRotationQuaternion() const;

	XMFLOAT3 GetVelocity() const;
	void SetVelocity(XMFLOAT3 xmfVel);
	void AddVelocity(XMFLOAT3 xmfVel);
	void SubVelocity(XMFLOAT3 xmfVel);
	void InitYVelocity();	//y vel값을 

	XMFLOAT3 GetAccerleration() const;
	void SetAcceleration(XMFLOAT3 xmfAcc);

	float GetFricCoef() const;
	void SetFricCoef(float friction);

	float GetMass()const;
	void SetMass(float mass);

	float GetMaxVel()const;
	void SetMaxVel(float velocity);

	float GetForceAmountXZ()const;
	float GetForceAmountY() const;

	// x, y, z 축 방향 이동
	void MoveObject(XMFLOAT3 distance);
	void MoveObject(float x, float y, float z);
	void MoveStrafe(float distance);
	void MoveUp(float distance);
	void MoveForward(float distance);

	// 회전
	void Rotate(float pitch, float yaw, float roll);
	void SetMatrixByLook(float x, float y, float z);

	virtual Object* clone() const { return new Object(*this); }

	void SetuniqueID(int uniq) { m_uniqueID = uniq; }
	int GetUniqueID() const { return m_uniqueID; }

	bool IsStopped();
protected:
	int m_uniqueID;

	XMFLOAT4X4 m_xmf4x4World{};
	XMFLOAT3 m_PreLook{ 0.f, 0.f, 1.f };
	XMFLOAT3 m_xmfVel{};
	XMFLOAT3 m_xmfAcc{};

	XMFLOAT3 m_xmf3PrePosition{};

	float m_forceAmountXZ{};
	float m_forceAmountY{};
	float m_mass{};
	float m_fricCoef{};
	float m_maxVel{};

	bool m_isApplyGravity = true;

public:
	Boundary* m_boundaries;
};