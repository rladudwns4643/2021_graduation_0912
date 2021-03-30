#pragma once
#include "Global.h"

constexpr int NONETIYPE = -99;

//box, sphere

class Boundary {
public:
	Boundary() noexcept;
	Boundary(int unmBB, int numBS) noexcept;
	Boundary(const Boundary& other) noexcept;
	Boundary& operator=(const Boundary& ohter);
	~Boundary();

public:
	void SetBB(int numBB); //member 변수 set
	void SetBS(int numBS); 

	int GetObjType() const;
	void SetObjType(int obj_type);

	int GetNumOfBB() const;
	void SetNumOfBB(int num_bb);

	int GetNumObBS() const;
	void SetNumOfBS(int num_bs);

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

	XMFLOAT3 GetBSPos(int idx) const;
	void SetBSPos(float x, float y, float z, int idx);
	void SetBSPos(XMFLOAT3 xmfPos, int idx);

	float GetBSRad(int idx) const;
	void SetBSRad(float rad, int idx);

private:
	int m_obj_type;
	//SetBB
	int m_numOfBB;
	XMFLOAT4X4* m_xmf4x4_BB_world;
	XMFLOAT3* m_xmf3_BB_size;
	XMFLOAT3* m_xmf_BB_rot;
	//~SetBB

	//SetBS
	int m_numOfBS;
	XMFLOAT3* m_xmf3_BS;
	float* m_BS_rad; //반지름
	//~SetBS
};