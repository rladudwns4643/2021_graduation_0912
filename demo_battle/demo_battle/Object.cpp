#include "pch.h"
#include "Object.h"

Object::Object() {
	m_uniqueID = -1;
	m_xmf4x4World = MathHelper::Identity4x4();
	m_xmfVel = { 0.f, 0.f, 0.f };
	m_xmfAcc = { 0.f, 0.f, 0.f };
	m_xmf3PrePosition = { 0.f, 0.f, 0.f };
	m_forceAmountXZ = 0.f;
	m_forceAmountY = 0.0f;
	m_mass = 0.f;
	m_fricCoef = 0.f;
	m_maxVel = 0.f;
	m_boundaries = new Boundary;
}

Object::Object(const Object& other) noexcept {
	m_uniqueID = other.m_uniqueID;
	m_xmf4x4World = other.m_xmf4x4World;
	m_xmf3PrePosition = other.m_xmf3PrePosition;
	m_xmfVel = other.m_xmfVel;
	m_xmfAcc = other.m_xmfAcc;
	m_forceAmountXZ = other.m_forceAmountXZ;
	m_forceAmountY = other.m_forceAmountY;
	m_mass = other.m_mass;
	m_fricCoef = other.m_fricCoef;
	m_maxVel = other.m_maxVel;

	m_boundaries = new Boundary(*other.m_boundaries);
}

Object& Object::operator=(const Object& other) {
	if (this != &other) {
		delete m_boundaries;

		m_xmf4x4World = other.m_xmf4x4World;
		m_xmfVel = other.m_xmfVel;
		m_xmfAcc = other.m_xmfAcc;
		m_forceAmountXZ = other.m_forceAmountXZ;
		m_forceAmountY = other.m_forceAmountY;
		m_mass = other.m_mass;
		m_fricCoef = other.m_fricCoef;
		m_maxVel = other.m_maxVel;

		m_boundaries = other.m_boundaries;
	}
	return *this;
}

bool Object::Update(float elapsedTime, bool is_player) {
	m_xmf3PrePosition = GetPosition();

	float force{ GRAVITY * m_mass };		//¼öÁ÷Ç×·Â
	float friction = force * m_fricCoef;	//¸¶Âû

	//direction
	XMFLOAT3 normalrized = MathHelper::Normalize(m_xmfVel);

	//friction force
	float frictionX = -normalrized.x * friction;
	float frictionZ = -normalrized.z * friction;

	//friction acc
	XMFLOAT3 fricAcc;
	if (m_isApplyGravity) {
		fricAcc = {
			frictionX / m_mass,
			-GRAVITY / (float)pow(elapsedTime, 2),
			frictionZ / m_mass
		};
	}
	else {
		fricAcc = {
			frictionX / m_mass,
			0,
			frictionZ / m_mass
		};
	}

	//update velocity by fricion force
	XMFLOAT3 newVel{
		m_xmfVel.x + fricAcc.x * elapsedTime,
		m_xmfVel.y - GRAVITY * elapsedTime,
		m_xmfVel.z + fricAcc.z * elapsedTime
	};

	if (newVel.x * m_xmfVel.x <= FLT_EPSILON) m_xmfVel.x = 0.f;
	else m_xmfVel.x = newVel.x;
	if (newVel.z * m_xmfVel.z <= FLT_EPSILON) m_xmfVel.z = 0.f;
	else m_xmfVel.z = newVel.z;
	m_xmfVel.y = newVel.y;

	if (is_player) {
		float velSize = sqrtf(pow(m_xmfVel.x, 2) + pow(m_xmfVel.z, 2));
		if (velSize > m_maxVel) {
			m_xmfVel.x = m_xmfVel.x / velSize * m_maxVel;
			m_xmfVel.z = m_xmfVel.z / velSize * m_maxVel;
		}
	}

	m_xmf4x4World._41 += m_xmfVel.x * elapsedTime;
	m_xmf4x4World._42 += m_xmfVel.y * elapsedTime;
	m_xmf4x4World._43 += m_xmfVel.z * elapsedTime;


	if (m_xmf4x4World._42 < FLT_EPSILON) {
		m_xmf4x4World._42 = 0.f;
		m_xmfVel.y = 0.f;
	}

	XMFLOAT3 newPos = MathHelper::Subtract(GetPosition(), m_xmf3PrePosition);
	XMStoreFloat3(&newPos, XMVector3Length(XMLoadFloat3(&newPos)));
	if (MathHelper::IsZero(newPos.x)) {
		return false;
	}
	else {
		//À§Ä¡°¡ ¹Ù²î¸é boundariesµµ ¹Ù²ñ
		//m_boundaries->BoundaryMove(change);
		return true;
	}
}

bool Object::BulletUpdate(float elapsedTime) {
	m_xmf3PrePosition = GetPosition();

	float velSize = sqrtf(pow(m_xmfVel.x, 2) + pow(m_xmfVel.y, 2) + pow(m_xmfVel.z, 2));
	if (velSize > m_maxVel) {
		m_xmfVel.x = m_xmfVel.x / velSize * m_maxVel;
		m_xmfVel.y = m_xmfVel.y / velSize * m_maxVel;
		m_xmfVel.z = m_xmfVel.z / velSize * m_maxVel;
	}
	m_xmf4x4World._41 += m_xmfVel.x * elapsedTime;
	m_xmf4x4World._42 += m_xmfVel.y * elapsedTime;
	m_xmf4x4World._43 += m_xmfVel.z * elapsedTime;
	return true;
}

void Object::AddForce(XMFLOAT3 force, float elapsedTime, bool isBullet) {
	XMFLOAT3 acc{ 0.f, 0.f ,0.f };
	XMStoreFloat3(&acc, XMLoadFloat3(&force) / m_mass);

	m_xmfVel.x += acc.x * elapsedTime;
	m_xmfVel.y += acc.y * elapsedTime;
	if (isBullet) m_xmfVel.y += acc.y * elapsedTime;
	else m_xmfVel.y += acc.y * 16.f;
}

XMFLOAT4X4 Object::GetMatrix() const {
	return m_xmf4x4World;
}

XMFLOAT3 Object::GetPosition() const {
	return XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
}

void Object::SetPosition(float x, float y, float z) {
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void Object::SetPosition(XMFLOAT3 xmfPosition) {
	SetPosition(xmfPosition.x, xmfPosition.y, xmfPosition.z);
}

XMFLOAT3 Object::GetPrePosition() const {
	return m_xmf3PrePosition;
}

void Object::SetPrePosition(XMFLOAT3 xmfPosition) {
	m_xmf3PrePosition = xmfPosition;
}

XMFLOAT3 Object::GetLook() const {
	return MathHelper::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23));
}

void Object::SetLook(const float& x, const float& y, const float& z) {
	m_xmf4x4World._31 = x;
	m_xmf4x4World._32 = y;
	m_xmf4x4World._33 = z;
}

XMFLOAT3 Object::GetPreLook() const {
	return m_PreLook;
}

void Object::SetPreLook(XMFLOAT3 look) {
	m_PreLook = look;
}

XMFLOAT3 Object::GetUp() const {
	return MathHelper::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23));
}

void Object::SetUp(const float& x, const float& y, const float& z) {
	m_xmf4x4World._21 = x;
	m_xmf4x4World._22 = y;
	m_xmf4x4World._23 = z;
}

XMFLOAT3 Object::GetRight() const {
	return MathHelper::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13));
}

void Object::SetRight(const float& x, const float& y, const float& z) {
	m_xmf4x4World._11 = x;
	m_xmf4x4World._12 = y;
	m_xmf4x4World._13 = z;
}

/// <summary>
/// unused
/// </summary>
/// <returns></returns>
XMFLOAT3 Object::GetRotation() const {
	return XMFLOAT3();
}

XMFLOAT4 Object::GetRotationQuaternion() const {
	return MathHelper::QuatFromTwoVectors(m_PreLook, GetLook());
}

XMFLOAT3 Object::GetVelocity() const {
	return m_xmfVel;
}

void Object::SetVelocity(XMFLOAT3 xmfVel) {
	m_xmfVel = xmfVel;
}

void Object::AddVelocity(XMFLOAT3 xmfVel) {
	m_xmfVel = MathHelper::Add(m_xmfVel, xmfVel);
}

void Object::SubVelocity(XMFLOAT3 xmfVel) {
	m_xmfVel = MathHelper::Subtract(m_xmfVel, xmfVel);
}

void Object::InitYVelocity() {
	m_xmfVel.y = -GRAVITY;
}

XMFLOAT3 Object::GetAccerleration() const {
	return m_xmfAcc;
}

void Object::SetAcceleration(XMFLOAT3 xmfAcc) {
	m_xmfAcc = xmfAcc;
}

float Object::GetFricCoef() const {
	return m_fricCoef;
}

void Object::SetFricCoef(float friction) {
	m_fricCoef = friction;
}

float Object::GetMass() const {
	return m_mass;
}

void Object::SetMass(float mass) {
	m_mass = mass;
}

float Object::GetMaxVel() const {
	return m_maxVel;
}

void Object::SetMaxVel(float velocity) {
	m_maxVel = velocity;
}

float Object::GetForceAmountXZ() const {
	return m_forceAmountXZ;
}

float Object::GetForceAmountY() const {
	return m_forceAmountY;
}

void Object::MoveObject(XMFLOAT3 distance) {
	SetPosition(MathHelper::Add(GetPosition(), distance));
}

void Object::MoveObject(float x, float y, float z) {
	XMFLOAT3 distance{ x,y,z };
	MoveObject(distance);
}

void Object::MoveStrafe(float distance) {
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();

	xmf3Position = MathHelper::Add(xmf3Position, xmf3Right, distance);
	SetPosition(xmf3Position);
}

void Object::MoveUp(float distance) {
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();

	xmf3Position = MathHelper::Add(xmf3Position, xmf3Up, distance);
	SetPosition(xmf3Position);
}

void Object::MoveForward(float distance) {
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = MathHelper::Add(xmf3Position, xmf3Look, distance);
	SetPosition(xmf3Position);
}

void Object::Rotate(float pitch, float yaw, float roll) {
	XMFLOAT4 quat;
	XMStoreFloat4(&quat, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll)));
	XMFLOAT4X4 rot;

	MathHelper::QuatToMatrix(&quat, &rot);
	XMStoreFloat4x4(&m_xmf4x4World, XMLoadFloat4x4(&rot) * XMLoadFloat4x4(&m_xmf4x4World));
}

void Object::SetMatrixByLook(float x, float y, float z) {
	XMFLOAT3 look{ x,y,z };
	XMFLOAT3 up = GetUp();
	XMFLOAT3 right = MathHelper::CrossProduct(up, look);

	m_xmf4x4World._11 = right.x;
	m_xmf4x4World._12 = right.y;
	m_xmf4x4World._13 = right.z;

	m_xmf4x4World._31 = right.x;
	m_xmf4x4World._32 = right.y;
	m_xmf4x4World._33 = right.z;
}

bool Object::IsStopped() {
	XMFLOAT3 cur = GetPosition();
	if (abs(m_xmf3PrePosition.x - cur.x) < 0.1f
		&& abs(m_xmf3PrePosition.z - cur.z) < 0.1f) {
		return true;
	}
	return false;
}