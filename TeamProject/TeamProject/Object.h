#pragma once
#include "Mesh.h"
#include "Camera.h"

enum CubeObjectTexture {
	ctWHITE,
	ctRED,
	ctBLUE,
	ctGREEN,
	ctYELLOW,
	ctGROUND
};

class CShader;

class CGameObject
{
private:
	int m_nReferences = 0;

	CubeObjectTexture m_eTexture;

public:
	XMFLOAT4X4 m_xmf4x4World;

protected:
	CMesh** m_ppMeshes = NULL;
	int m_nMeshes = 0;
	CShader* m_pShader = NULL;

public:
	CGameObject(int nMeshes=1);
	virtual ~CGameObject();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);	// 상수 버퍼를 생성
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);								//상수 버퍼의 내용을 갱신
	virtual void ReleaseShaderVariables();

	void ReleaseUploadBuffers();

	virtual void SetMesh(int nIndex, CMesh* pMesh);
	virtual void SetShader(CShader* pShader);

	// 게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	// 게임 객체의 위치를 설정
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	// 게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	// 게임 객체를 회전
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	// 게임 객체 텍스쳐
	void SetTexture(CubeObjectTexture eTexture) { m_eTexture = eTexture; };
	CubeObjectTexture GetTexture() { return m_eTexture; }

	virtual void Animate(float fTimeElapsed);

	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView);
};

// 회전하는 오브젝트
class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes = 1);
	virtual ~CRotatingObject();

private:
	XMFLOAT3 m_xmf3RotationAxis;		// 회전 축
	float m_fRotationSpeed;				// 회전 속도

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {
		m_xmf3RotationAxis = xmf3RotationAxis;
	}

	virtual void Animate(float fTimeElapsed);
};

class CMapCubeObject : public CGameObject
{
public:
	CMapCubeObject();
	~CMapCubeObject();

private:
	bool m_bBreakable;

public:
	void SetBreakable(bool bBreakable) { m_bBreakable = bBreakable; }
	bool GetBreakable() { return m_bBreakable; }

	virtual void Animate(float fTimeElapsed);
};

// 높이 맵 지형
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature, int nWidthBlock, int nLengthBlock);
	virtual ~CHeightMapTerrain();

private:
	CHeightMapGridMesh* m_pHeightMap = NULL;	// 지형의 높이 맵

	// 높이 맵의 가로와 세로 블록개수
	int m_nWidthBlock;
	int m_nLengthBlock;

public:
	float GetHeight(float x, float z) {
		return(m_pHeightMap->GetHeight(x , z));
	}

	// 지형의 법선 벡터를 계산하는 함수(월드 좌표계) - 높이 맵의 법선 벡터를 사용
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMap->GetHeightMapNormal((int)x, (int)z));
	}

	// 지형의 크기(가로/세로)를 반환 - 높이 맵의 크기에 스케일을 곱한 값
	float GetWidth() { return(m_nWidthBlock * STD_CUBE_SIZE); }
	float GetLength() { return(m_nLengthBlock * STD_CUBE_SIZE); }
};