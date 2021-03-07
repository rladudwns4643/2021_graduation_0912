#pragma once
#define DIR_FORWARD 0x01
#define DIR_BACKWARD 0x02
#define DIR_LEFT 0x04
#define DIR_RIGHT 0x08
#define DIR_UP 0x10
#define DIR_DOWN 0x20

#include "GameObject.h"
#include "Camera.h"

class CPlayer : public CGameObject
{
protected:
	// �÷��̾��� ��ġ ����, x-��(Right), y-��(Up), z-��(Look) ����
	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	// �÷��̾ ���� x-��(Right), y-��(Up), z-��(Look)���� �󸶸�ŭ ȸ���ߴ°��� ��Ÿ��
	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	XMFLOAT3					m_xmf3Velocity;		// �÷��̾��� �̵� �ӵ��� ��Ÿ���� ����
	XMFLOAT3     				m_xmf3Gravity;		// �÷��̾ �ۿ��ϴ� �߷��� ��Ÿ���� ����
	float           			m_fMaxVelocityXZ;	// xz-��鿡�� (�� ������ ����) �÷��̾��� �̵� �ӷ��� �ִ밪
	float           			m_fMaxVelocityY;	// y-�� �������� (�� ������ ����) �÷��̾��� �̵� �ӷ��� �ִ밪
	float           			m_fFriction;		// �÷��̾ �ۿ��ϴ� ������

	// �÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnPlayerUpdateCallback() �Լ����� ����ϴ� ������
	LPVOID						m_pPlayerUpdatedContext;
	// ī�޶��� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnCameraUpdateCallback() �Լ����� ����ϴ� ������
	LPVOID						m_pCameraUpdatedContext;

	// �÷��̾ ���� ������ ī�޶�
	CCamera*					m_pCamera = NULL;

public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3& GetPosition() { return(m_xmf3Position); }
	XMFLOAT3& GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPosition(const XMFLOAT3& xmf3Position) { Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false); }

	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }

	CCamera* GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	// �÷��̾ �̵��ϴ� �Լ�
	void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);

	// �÷��̾ ȸ���ϴ� �Լ�
	void Rotate(float x, float y, float z);

	// �÷��̾��� ��ġ�� ȸ�� ������ ��� �ð��� ���� �����ϴ� �Լ�
	void Update(float fTimeElapsed);

	// �÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ�
	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	// ī�޶��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ�
	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	// ���̴� ���� ����
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	// ī�޶� �����ϱ� ���Ͽ� ȣ���ϴ� �Լ�
	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }

	// �÷��̾��� ��ġ�� ȸ�������κ��� ���� ��ȯ ����� �����ϴ� �Լ�
	virtual void OnPrepareRender();

	// �÷��̾��� ī�޶� 3��Ī ī�޶��� �� �÷��̾�(�޽�)�� �������Ѵ�.
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};

class CTestPlayer : public CPlayer
{
public:
	CTestPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CTestPlayer();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPrepareRender();

	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);
};