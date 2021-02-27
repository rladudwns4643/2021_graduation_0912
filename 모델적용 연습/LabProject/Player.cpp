#include "pch.h"
#include "Player.h"
#include "Shader.h"

CPlayer::CPlayer() : CGameObject()
{
	m_pCamera = NULL;
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fSpeed = 0;
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}
CPlayer::~CPlayer()
{
	ReleaseShaderVariables();
	if (m_pCamera) delete m_pCamera;
}

void CPlayer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);

	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void CPlayer::ReleaseShaderVariables()
{
	CGameObject::ReleaseShaderVariables();

	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}
void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		// 'W'�� ������ ���� z-�� �������� �̵�(����)�Ѵ�. 'S'�� ������ �ݴ� �������� �̵�
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);

		// 'D'�� ������ ���� x-�� �������� �̵��Ѵ�. 'A'�� ������ �ݴ� �������� �̵�
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);

		// 'Page Up'�� ������ ���� y-�� �������� �̵��Ѵ�. 'Page Down'�� ������ �ݴ� �������� �̵�
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		// �÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵�
		Move(xmf3Shift, bUpdateVelocity);
	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	// bUpdateVelocity�� ���̸� �÷��̾ �̵����� �ʰ� �ӵ� ���͸� ����
	if (bUpdateVelocity)
	{
		// �÷��̾��� �ӵ� ���͸� xmf3Shift ���͸�ŭ ����
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		// �÷��̾ ���� ��ġ ���Ϳ��� xmf3Shift ���͸�ŭ �̵�
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);

		// �÷��̾��� ��ġ�� ����Ǿ����Ƿ� ī�޶��� ��ġ�� xmf3Shift ���͸�ŭ �̵�
		if (m_pCamera) m_pCamera->Move(xmf3Shift);
	}
}

// �÷��̾ ���� x-��, y-��, z-���� �߽����� ȸ��
void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCameraMode = m_pCamera->GetMode();
	if ((nCameraMode == FIRST_PERSON_CAMERA) || (nCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		// ī�޶� x, y, z ��ŭ ȸ��(�÷��̾ ȸ���ϸ� ī�޶� ȸ��
		m_pCamera->Rotate(x, y, z);

		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCameraMode == SPACESHIP_CAMERA)
	{
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	
	// z�� �������� ���͵��� ������ ����
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::Update(float fTimeElapsed)
{
	// �÷��̾��� �ӵ� ���͸� �߷� ���Ϳ� ����
	// �߷� ���Ϳ� fTimeElapsed�� ���ϴ� ���� �߷��� �ð��� ����ϵ��� �����Ѵٴ� �ǹ�
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));

	//�÷��̾��� �ӵ� ������ XZ-������ ũ�⸦ ����
	// �̰��� XZ-����� �ִ� �ӷº��� ũ�� �ӵ� ������ x�� z-���⼺���� ����
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	// �÷��̾��� �ӵ� ������ y-������ ũ�⸦ ����
	// �̰��� y-�� ������ �ִ� �ӷº��� ũ�� �ӵ� ������ y-���� ������ ����
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	// �÷��̾ �ӵ� ���� ��ŭ ������ �̵�(ī�޶� �̵�) - �̲������� ���°� �Ϸ��� ���� �ּ� Ǯ��(������ ���)
	// XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	Move(m_xmf3Velocity, false);

	// �÷��̾� ��ġ�� ��ȿ�� ��ġ�� �ƴҶ�(�浹 �˻���� ����) ��ȿ�� ��ġ�� �ٽ� ����
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);

	DWORD nCameraMode = m_pCamera->GetMode();
	// �÷��̾��� ��ġ�� ����Ǿ����Ƿ� 3��Ī ī�޶� ������
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position, fTimeElapsed);

	// ī�޶��� ��ġ�� ����� �� �߰��� ������ �۾��� ����
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);

	// ī�޶� 3��Ī ī�޶��̸� ī�޶� ����� �÷��̾� ��ġ�� �ٶ󺸵��� ��
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);

	// ī�޶��� ī�޶� ��ȯ ����� �ٽ� ����
	m_pCamera->RegenerateViewMatrix();

	// �÷��̾��� �ӵ� ���Ͱ� ������ ������ ������ �Ǿ�� �Ѵٸ� ���� ���͸� ����
	// �ӵ� ������ �ݴ� ���� ���͸� ���ϰ� ���� ���ͷ� ����
	// ���� ����� �ð��� ����ϵ��� �Ͽ� �������� ����
	// ���� ���Ϳ� �������� ���Ͽ� ���� ���͸� ����
	// �ӵ� ���Ϳ� ���� ���͸� ���Ͽ� �ӵ� ���͸� ����
	// �������� �ӷº��� ũ�� �ӷ��� 0�� ��
	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

// ī�޶� ����
CCamera* CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	// ���ο� ī�޶��� ��忡 ���� ī�޶� ���� ����
	CCamera* pNewCamera = NULL;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break;
	}

	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		// ���� ī�޶� ����ϴ� �÷��̾� ��ü�� ����
		pNewCamera->SetPlayer(this);
	}
	if (m_pCamera) delete m_pCamera;

	return(pNewCamera);
}
void CPlayer::OnPrepareRender()
{
	m_xmf4x4World._11 = m_xmf3Right.x;
	m_xmf4x4World._12 = m_xmf3Right.y;
	m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x;
	m_xmf4x4World._22 = m_xmf3Up.y;
	m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x;
	m_xmf4x4World._32 = m_xmf3Look.y;
	m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x;
	m_xmf4x4World._42 = m_xmf3Position.y;
	m_xmf4x4World._43 = m_xmf3Position.z;
}
void CPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	// ī�޶� ��尡 3��Ī�̸� �÷��̾� ��ü�� ������
	if (nCameraMode == THIRD_PERSON_CAMERA)
		CGameObject::Render(pd3dCommandList, pCamera);
}

//---------------------------------------------------------------------------------------------

CTestPlayer::CTestPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CPlayer()
{
	// �÷��̾� �޽��� �����Ѵ�.
	CMesh* pTestPlayerMesh = new CCubeMesh(pd3dDevice, pd3dCommandList, PLAYER_SIZE_X, PLAYER_SIZE_Y, PLAYER_SIZE_Z);;
	SetMesh(pTestPlayerMesh);

	// �÷��̾��� ī�޶� �����̽�-�� ī�޶�� ����(����)
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	// �÷��̾��� ��ġ�� ����
	XMFLOAT3 fPlayerPos(0.0f, 0.0f, 0.0f);
	SetPosition(fPlayerPos);
	// �÷��̾� �ӵ��� ����
	SetSpeed(200.f);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//// �÷��̾��� ��ġ�� ����� �� ������ ������ ���� �÷��̾��� ��ġ�� ������ �� �ֵ��� ����
	//SetPlayerUpdatedContext(pTerrain);
	//// ī�޶��� ��ġ�� ����� �� ������ ������ ���� ī�޶��� ��ġ�� ������ �� �ֵ��� ����
	//SetCameraUpdatedContext(pTerrain);

	// �÷��̾� �޽��� �������� �� ����� ���̴��� ����
	CPlayerShader* pShader = new CPlayerShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetShader(pShader);
}
CTestPlayer::~CTestPlayer()
{
}

void CTestPlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();

	// ���� �׸��� ���� x-������ 90�� ȸ��
//	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f);
//	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}
// ī�޶� ������ �� ȣ��Ǵ� �Լ� - nNewCameraMode�� ���� ������ ī�޶� ���
CCamera* CTestPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	XMFLOAT3 SetPGrav;
	switch (nNewCameraMode)
	{
	// �÷��̾��� Ư���� 1��Ī ī�޶� ��忡 �°� �����Ѵ�. �߷��� �������� �ʴ´�.
	case FIRST_PERSON_CAMERA:
		SetFriction(200.0f);
		SetPGrav = XMFLOAT3(0.0f, 0.0f, 0.0f);
		SetGravity(SetPGrav);
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	// �÷��̾��� Ư���� �����̽�-�� ī�޶� ��忡 �°� �����Ѵ�. �߷��� �������� �ʴ´�.
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetPGrav = XMFLOAT3(0.0f, 0.0f, 0.0f);
		SetGravity(SetPGrav);
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	// �÷��̾��� Ư���� 3��Ī ī�޶� ��忡 �°� �����Ѵ�. ���� ȿ���� ī�޶� �������� �����Ѵ�.
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetPGrav = XMFLOAT3(0.0f, 0.0f, 0.0f);
		SetGravity(SetPGrav);
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		// 3��Ī ī�޶��� ���� ȿ���� ����
		m_pCamera->SetTimeLag(0.10f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, STD_CUBE_SIZE * 1.5f, -STD_CUBE_SIZE * 4.5f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		break;
	default:
		break;
	}
	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
	// �÷��̾ �ð��� ����� ���� ����(��ġ�� ������ ����: �ӵ�, ������, �߷� ���� ó��)�Ѵ�.
	Update(fTimeElapsed);

	return(m_pCamera);
}

// �÷��̾��� ��ġ�� �ٲ𶧸��� ȣ��
void CTestPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{
	//XMFLOAT3 xmf3PlayerPosition = GetPosition();
	//CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pPlayerUpdatedContext;
	//
	//// �������� �÷��̾��� ���� ��ġ (x, z)�� ���� ����(y)�� ���Ѵ�. 
	//// �׸��� �÷��̾� �޽��� ���̰� 12�̰� �÷��̾��� �߽��� ������ü�� ����̹Ƿ� y ���� �޽��� ������ ������ ���ϸ� �÷��̾��� ��ġ�� �ȴ�.
	//float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z) + STD_PLAYER_CUBE_SIZE + STD_CUBE_SIZE;
	//
	//// �÷��̾ �׻� ���� ���� �ֵ��� ����
	//if (xmf3PlayerPosition.y < fHeight)
	//{
	//	XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
	//	xmf3PlayerVelocity.y = 0.0f;
	//	SetVelocity(xmf3PlayerVelocity);
	//	xmf3PlayerPosition.y = fHeight;
	//	SetPosition(xmf3PlayerPosition);
	//}
}
void CTestPlayer::OnCameraUpdateCallback(float fTimeElapsed)
{
	//XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	//
	//// ī�޶� ���� ������ ���� �ʵ��� ����
	//CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pCameraUpdatedContext;
	//float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z) + 5.0f;
	//if (xmf3CameraPosition.y <= fHeight)
	//{
	//	xmf3CameraPosition.y = fHeight;
	//	m_pCamera->SetPosition(xmf3CameraPosition);
	//	if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
	//	{
	//		// 3��Ī ī�޶��� ��� ī�޶� ��ġ(y-��ǥ)�� ����Ǿ����Ƿ� ī�޶� �÷��̾ �ٶ󺸵��� �Ѵ�.
	//		CThirdPersonCamera* p3rdPersonCamera = (CThirdPersonCamera*)m_pCamera;
	//		p3rdPersonCamera->SetLookAt(GetPosition());
	//	}
	//}
}