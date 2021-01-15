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

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);	// ��� ���۸� ����
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);								//��� ������ ������ ����
	virtual void ReleaseShaderVariables();

	void ReleaseUploadBuffers();

	virtual void SetMesh(int nIndex, CMesh* pMesh);
	virtual void SetShader(CShader* pShader);

	// ���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	// ���� ��ü�� ��ġ�� ����
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	// ���� ��ü�� ���� x-��, y-��, z-�� �������� �̵�
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	// ���� ��ü�� ȸ��
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	// ���� ��ü �ؽ���
	void SetTexture(CubeObjectTexture eTexture) { m_eTexture = eTexture; };
	CubeObjectTexture GetTexture() { return m_eTexture; }

	virtual void Animate(float fTimeElapsed);

	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView);
};

// ȸ���ϴ� ������Ʈ
class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes = 1);
	virtual ~CRotatingObject();

private:
	XMFLOAT3 m_xmf3RotationAxis;		// ȸ�� ��
	float m_fRotationSpeed;				// ȸ�� �ӵ�

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

// ���� �� ����
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature, int nWidthBlock, int nLengthBlock);
	virtual ~CHeightMapTerrain();

private:
	CHeightMapGridMesh* m_pHeightMap = NULL;	// ������ ���� ��

	// ���� ���� ���ο� ���� ��ϰ���
	int m_nWidthBlock;
	int m_nLengthBlock;

public:
	float GetHeight(float x, float z) {
		return(m_pHeightMap->GetHeight(x , z));
	}

	// ������ ���� ���͸� ����ϴ� �Լ�(���� ��ǥ��) - ���� ���� ���� ���͸� ���
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMap->GetHeightMapNormal((int)x, (int)z));
	}

	// ������ ũ��(����/����)�� ��ȯ - ���� ���� ũ�⿡ �������� ���� ��
	float GetWidth() { return(m_nWidthBlock * STD_CUBE_SIZE); }
	float GetLength() { return(m_nLengthBlock * STD_CUBE_SIZE); }
};