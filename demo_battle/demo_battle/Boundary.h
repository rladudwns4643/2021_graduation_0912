#pragma once
#include "Global.h"
#include "SMathHelper.h"

constexpr int NONETYPE = -99;

class Boundary {
public:
	Boundary() noexcept;
	Boundary(int numBB) noexcept;
	Boundary(const Boundary& other) noexcept;
	Boundary& operator=(const Boundary& other);
	~Boundary();

public:
	void SetBB(int numBB); //member º¯¼ö set

	int GetObjType() const;
	void SetObjType(int obj_type);

	int GetNumOfBB() const;
	void SetNumOfBB(int num_bb);

	XMFLOAT3 GetBBRot(int idx) const;
	void SetBBRot(float x, float y, float z, int idx);
	void SetBBRot(XMFLOAT3 xmfRot, int idx);

	XMFLOAT3 GetBBSize(int idx) const;
	void SetBBSize(float x, float y, float z, int idx);
	void SetBBSize(XMFLOAT3 xmfBBSize, int idx);

	XMFLOAT3 GetBBPos(int idx) const;
	void SetBBPos(float x, float y, float z, int idx);
	void SetBBPos(XMFLOAT3 xmfPos, int idx);

	XMFLOAT3 GetBBLook(int idx) const;
	void SetBBLook(float x, float y, float z, int idx);
	void SetBBLook(XMFLOAT3 xmfLook, int idx);

	XMFLOAT3 GetBBUp(int idx) const;
	void SetBBUp(float x, float y, float z, int idx);
	void SetBBUp(XMFLOAT3 xmfUp, int idx);

	XMFLOAT3 GetBBRight(int idx) const;
	void SetBBRight(float x, float y, float z, int idx);
	void SetBBRight(XMFLOAT3 xmfRight, int idx);

	XMFLOAT4X4 GetWorldMatrix(int idx) const;
	void SetWorldMatrix(const XMFLOAT4X4& mat, int idx);

	void BoundaryMove(float x, float y, float z);
	void BoundaryMove(XMFLOAT3 distance);

private:
	int m_obj_type;
	int m_numOfBB;
	XMFLOAT4X4* m_xmf4x4_BB_world;
	XMFLOAT3* m_xmf3_BB_size;
	XMFLOAT3* m_xmf3_BB_rot;
};