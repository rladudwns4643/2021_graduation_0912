#include "pch.h"
#include "Boundary.h"

Boundary::Boundary() noexcept {
	m_xmf4x4_BB_world = nullptr;
	m_xmf3_BB_size = nullptr;
	m_xmf3_BB_rot = nullptr;
	m_obj_type = NONETYPE;
	m_numOfBB = 0;
}

Boundary::Boundary(int numBB) noexcept {
	m_obj_type = NONETYPE;

	m_numOfBB = numBB;

	m_xmf4x4_BB_world = new XMFLOAT4X4[m_numOfBB];
	for (int i = 0; i < m_numOfBB; ++i) {
		m_xmf4x4_BB_world[i] = MathHelper::Identity4x4();
	}
	m_xmf3_BB_size = new XMFLOAT3[m_numOfBB];
	m_xmf3_BB_rot = new XMFLOAT3[m_numOfBB];
}

Boundary::Boundary(const Boundary& other) noexcept {
	m_obj_type = other.m_obj_type;
	m_numOfBB = other.m_numOfBB;
	m_xmf4x4_BB_world = new XMFLOAT4X4[m_numOfBB];
	m_xmf3_BB_size = new XMFLOAT3[m_numOfBB];
	m_xmf3_BB_rot = new XMFLOAT3[m_numOfBB];

	memcpy(m_xmf4x4_BB_world, other.m_xmf4x4_BB_world, sizeof(XMFLOAT4X4) * m_numOfBB);
	memcpy(m_xmf4x4_BB_world, other.m_xmf4x4_BB_world, sizeof(XMFLOAT4X4) * m_numOfBB);
	memcpy(m_xmf4x4_BB_world, other.m_xmf4x4_BB_world, sizeof(XMFLOAT4X4) * m_numOfBB);
}

Boundary& Boundary::operator=(const Boundary& other) {
	if (this != &other) {
		delete[] m_xmf4x4_BB_world;
		delete[] m_xmf3_BB_size;
		delete[] m_xmf3_BB_rot;

		m_numOfBB = other.m_numOfBB;
		m_xmf4x4_BB_world = new XMFLOAT4X4[m_numOfBB];
		m_xmf3_BB_size = new XMFLOAT3[m_numOfBB];
		m_xmf3_BB_rot = new XMFLOAT3[m_numOfBB];

		memcpy(m_xmf4x4_BB_world, other.m_xmf4x4_BB_world, sizeof(XMFLOAT4X4) * m_numOfBB);
		memcpy(m_xmf3_BB_size, other.m_xmf3_BB_size, sizeof(XMFLOAT3) * m_numOfBB);
		memcpy(m_xmf3_BB_rot, other.m_xmf3_BB_rot, sizeof(XMFLOAT3) * m_numOfBB);
	}
	return *this;
}

Boundary::~Boundary() {
	delete[] m_xmf4x4_BB_world;
	delete[] m_xmf3_BB_size;
	delete[] m_xmf3_BB_rot;
}

void Boundary::SetBB(int numBB) {
	if (m_xmf4x4_BB_world != nullptr) delete[] m_xmf4x4_BB_world;
	if (m_xmf3_BB_size != nullptr) delete[] m_xmf3_BB_size;
	if (m_xmf3_BB_rot != nullptr) delete[] m_xmf3_BB_rot;

	m_numOfBB = numBB;
	m_xmf4x4_BB_world = new XMFLOAT4X4[numBB];
	for (int i = 0; i < numBB; ++i){
		m_xmf4x4_BB_world[i] = MathHelper::Identity4x4();
	}
	m_xmf3_BB_size = new XMFLOAT3[numBB];
	m_xmf3_BB_rot = new XMFLOAT3[numBB];
}

int Boundary::GetObjType() const {
	return m_obj_type;
}

void Boundary::SetObjType(int obj_type) {
	m_obj_type = obj_type;
}

int Boundary::GetNumOfBB() const {
	return m_numOfBB;
}

void Boundary::SetNumOfBB(int num_bb) {
	m_numOfBB = num_bb;
}

XMFLOAT3 Boundary::GetBBRot(int idx) const {
	return m_xmf3_BB_rot[idx];
}

void Boundary::SetBBRot(float x, float y, float z, int idx) {
	if (x >= 360.f) x -= 360;
	if (y >= 360.f) y -= 360;
	if (z >= 360.f) z -= 360;
	m_xmf3_BB_rot[idx].x = x;
	m_xmf3_BB_rot[idx].y = y;
	m_xmf3_BB_rot[idx].z = z;
}

void Boundary::SetBBRot(XMFLOAT3 xmfRot, int idx) {
	SetBBRot(xmfRot.x, xmfRot.y, xmfRot.z, idx);
}

XMFLOAT3 Boundary::GetBBSize(int idx) const {
	return m_xmf3_BB_size[idx];
}

void Boundary::SetBBSize(float x, float y, float z, int idx) {
	m_xmf3_BB_size[idx].x = x;
	m_xmf3_BB_size[idx].y = y;
	m_xmf3_BB_size[idx].z = z;
}

void Boundary::SetBBSize(XMFLOAT3 xmfBBSize, int idx) {
	SetBBSize(xmfBBSize.x, xmfBBSize.y, xmfBBSize.z, idx);
}

XMFLOAT3 Boundary::GetBBPos(int idx) const {
	return XMFLOAT3(m_xmf4x4_BB_world[idx]._41, m_xmf4x4_BB_world[idx]._42, m_xmf4x4_BB_world[idx]._43);
}

void Boundary::SetBBPos(float x, float y, float z, int idx) {
	m_xmf4x4_BB_world[idx]._41 = x;
	m_xmf4x4_BB_world[idx]._42 = y;
	m_xmf4x4_BB_world[idx]._43 = z;
}

void Boundary::SetBBPos(XMFLOAT3 xmfPos, int idx) {
	SetBBPos(xmfPos.x, xmfPos.y, xmfPos.z, idx);
}

XMFLOAT3 Boundary::GetBBLook(int idx) const {
	return MathHelper::Normalize(XMFLOAT3(m_xmf4x4_BB_world[idx]._31, m_xmf4x4_BB_world[idx]._32, m_xmf4x4_BB_world[idx]._33));
}

void Boundary::SetBBLook(float x, float y, float z, int idx) {
	m_xmf4x4_BB_world[idx]._31 = x;
	m_xmf4x4_BB_world[idx]._32 = y;
	m_xmf4x4_BB_world[idx]._33 = z;
}

void Boundary::SetBBLook(XMFLOAT3 xmfLook, int idx) {
	SetBBLook(xmfLook.x, xmfLook.y, xmfLook.z, idx);
}

XMFLOAT3 Boundary::GetBBUp(int idx) const {
	return MathHelper::Normalize(XMFLOAT3(m_xmf4x4_BB_world[idx]._21, m_xmf4x4_BB_world[idx]._22, m_xmf4x4_BB_world[idx]._23));
}

void Boundary::SetBBUp(float x, float y, float z, int idx) {
	m_xmf4x4_BB_world[idx]._21 = x;
	m_xmf4x4_BB_world[idx]._22 = y;
	m_xmf4x4_BB_world[idx]._23 = z;
}

void Boundary::SetBBUp(XMFLOAT3 xmfUp, int idx) {
	SetBBUp(xmfUp.x, xmfUp.y, xmfUp.z, idx);
}

XMFLOAT3 Boundary::GetBBRight(int idx) const {
	return MathHelper::Normalize(XMFLOAT3(m_xmf4x4_BB_world[idx]._11, m_xmf4x4_BB_world[idx]._12, m_xmf4x4_BB_world[idx]._13));
}

void Boundary::SetBBRight(float x, float y, float z, int idx) {
	m_xmf4x4_BB_world[idx]._11 = x;
	m_xmf4x4_BB_world[idx]._12 = y;
	m_xmf4x4_BB_world[idx]._13 = z;
}

void Boundary::SetBBRight(XMFLOAT3 xmfRight, int idx) {
	SetBBRight(xmfRight.x, xmfRight.y, xmfRight.z, idx);
}

XMFLOAT4X4 Boundary::GetWorldMatrix(int idx) const {
	return m_xmf4x4_BB_world[idx];
}

void Boundary::SetWorldMatrix(const XMFLOAT4X4& mat, int idx) {
	m_xmf4x4_BB_world[idx] = mat;
}

void Boundary::BoundaryMove(float x, float y, float z){
	XMFLOAT3 distance{ x,y,z };
	BoundaryMove(distance);
}

void Boundary::BoundaryMove(XMFLOAT3 distance){
	for (int i = 0; i < m_numOfBB; ++i) {
		XMFLOAT3 bbPos = GetBBPos(i);
		bbPos = MathHelper::Add(bbPos, distance);
		SetBBPos(bbPos, i);
	}
}
