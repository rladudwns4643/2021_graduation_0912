#include "Object.h"
#include "Shader.h"

CGameObject::CGameObject(int nMeshes)
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh * [m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;
	}
}
CGameObject::~CGameObject()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}

	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}
void CGameObject::SetMesh(int nIndex, CMesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}
void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}
XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

// 회전축
// 게임 객체의 로컬 z-축 벡터를 반환
XMFLOAT3 CGameObject::GetLook()
{
	XMFLOAT3 xmf3Look{ m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33 };
	return(Vector3::Normalize(xmf3Look));
}
// 게임 객체의 로컬 y-축 벡터를 반환
XMFLOAT3 CGameObject::GetUp()
{
	XMFLOAT3 xmf3Up{ m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23 };
	return(Vector3::Normalize(xmf3Up));
}
// 게임 객체의 로컬 x-축 벡터를 반환
XMFLOAT3 CGameObject::GetRight()
{
	XMFLOAT3 xmf3Right{ m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13 };
	return(Vector3::Normalize(xmf3Right));
}

// 게임 객체를 로컬 x-축 방향으로 이동
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
// 게임 객체를 로컬 y-축 방향으로 이동
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
// 게임 객체를 로컬 z-축 방향으로 이동
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

// 게임 객체를 주어진 각도로 회전
void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	// 객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}
void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::ReleaseUploadBuffers()
{
	// 정점 버퍼를 위한 업로드 버퍼를 소멸
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}
}

// 애니메이션
void CGameObject::Animate(float fTimeElapsed)
{
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	// 객체의 정보를 셰이더 변수(상수 버퍼)로 복사
	UpdateShaderVariables(pd3dCommandList);

	// 게임 객체에 셰이더 객체가 연결되어 있으면 셰이더 상태 객체를 설정
	if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);

	// 게임 객체에 메쉬가 연결되어 있으면 메쉬를 렌더링
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}
void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances)
{
	OnPrepareRender();

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
		}
	}
}
void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView)
{
	OnPrepareRender();

	// 게임 객체에 메쉬가 연결되어 있으면 메쉬를 렌더링
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances, d3dInstancingBufferView);
		}
	}
}

CRotatingObject::CRotatingObject(int nMeshes) : CGameObject(nMeshes)
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}
CRotatingObject::~CRotatingObject()
{
}

// 회전 객체 애니메이션
void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}


CMapCubeObject::CMapCubeObject()
{
	CubeObjectTexture m_eTexture = ctWHITE;
	bool m_bBreakable = false;
}
CMapCubeObject::~CMapCubeObject()
{
}

void CMapCubeObject::Animate(float fTimeElapsed)
{
}


// 높이 맵
CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dGraphicsRootSignature, int nWidthBlock, int nLengthBlock) : CGameObject(0)
{
	m_nWidthBlock = nWidthBlock;
	m_nLengthBlock = nLengthBlock;

	m_pHeightMap = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, 0, 0, nWidthBlock, nLengthBlock);
	SetMesh(0, m_pHeightMap);
	
	// 지형을 렌더링하기 위한 셰이더를 생성
	CTerrainShader* pShader = new CTerrainShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader);
}
CHeightMapTerrain::~CHeightMapTerrain(void)
{
}