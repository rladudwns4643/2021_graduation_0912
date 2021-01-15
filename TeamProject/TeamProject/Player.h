#pragma once
#define DIR_FORWARD 0x01
#define DIR_BACKWARD 0x02
#define DIR_LEFT 0x04
#define DIR_RIGHT 0x08
#define DIR_UP 0x10
#define DIR_DOWN 0x20

#include "Object.h"
#include "Camera.h"

class CPlayer : public CGameObject
{
protected:
	// 플레이어의 위치 벡터, x-축(Right), y-축(Up), z-축(Look) 벡터
	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Up;
	XMFLOAT3 m_xmf3Look;

	// 플레이어가 로컬 x-축(Right), y-축(Up), z-축(Look)으로 얼마만큼 회전했는가를 나타냄
	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	// 플레이어의 이동 속도를 나타내는 벡터
	XMFLOAT3 m_xmf3Velocity;
	// 플레이어의 이동속도를 나타내는 변수
	float m_fSpeed;

	// 플레이어에 작용하는 중력을 나타내는 벡터
	XMFLOAT3 m_xmf3Gravity;

	// xz-평면에서 (한 프레임 동안) 플레이어의 이동 속력의 최대값
	float m_fMaxVelocityXZ;
	// y-축 방향으로 (한 프레임 동안) 플레이어의 이동 속력의 최대값
	float m_fMaxVelocityY;

	// 플레이어에 작용하는 마찰력
	float m_fFriction;

	// 플레이어의 위치가 바뀔 때마다 호출되는 OnPlayerUpdateCallback() 함수에서 사용하는 데이터
	LPVOID m_pPlayerUpdatedContext;
	// 카메라의 위치가 바뀔 때마다 호출되는 OnCameraUpdateCallback() 함수에서 사용하는 데이터
	LPVOID m_pCameraUpdatedContext;

	// 플레이어에 현재 설정된 카메라
	CCamera* m_pCamera = NULL;

public:
	CPlayer(int nMeshes = 1);
	virtual ~CPlayer();

	XMFLOAT3& GetPosition() { return(m_xmf3Position); }
	XMFLOAT3& GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }

	void SetPosition(XMFLOAT3& xmf3Position) {
		Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false);
	}

	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }
	float GetSpeed() { return m_fSpeed; }
	float GetYaw() { return(m_fYaw); }
	float GetPitch() { return(m_fPitch); }
	float GetRoll() { return(m_fRoll); }

	CCamera* GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	// 플레이어를 이동하는 함수
	void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);

	// 플레이어를 회전하는 함수
	void Rotate(float x, float y, float z);

	// 플레이어의 위치와 회전 정보를 경과 시간에 따라 갱신하는 함수
	void Update(float fTimeElapsed);

	// 플레이어의 위치가 바뀔 때마다 호출되는 함수와 그 함수에서 사용하는 정보를 설정하는 함수
	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	// 카메라의 위치가 바뀔 때마다 호출되는 함수와 그 함수에서 사용하는 정보를 설정하는 함수
	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	// 쉐이더 변수 설정
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	// 카메라를 변경하기 위하여 호출하는 함수
	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }

	// 플레이어의 위치와 회전축으로부터 월드 변환 행렬을 생성하는 함수
	virtual void OnPrepareRender();

	// 플레이어의 카메라가 3인칭 카메라일 때 플레이어(메쉬)를 렌더링한다.
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};

class CTestPlayer : public CPlayer
{
public:
	CTestPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, int nMeshes = 1);
	virtual ~CTestPlayer();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPrepareRender();

	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);
};