#pragma once
#include "Timer.h"
#include "Camera.h"
#include "Shader.h"
#include "Player.h"

struct LIGHT
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;
	XMFLOAT3 m_xmf3Position;
	float m_fFalloff;
	XMFLOAT3 m_xmf3Direction;
	float m_fTheta; //cos(m_fTheta)
	XMFLOAT3 m_xmf3Attenuation;
	float m_fPhi; //cos(m_fPhi)
	bool m_bEnable;
	int m_nType;
	float m_fRange;
	float padding;
};
struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	XMFLOAT4 m_xmf4GlobalAmbient;
};
struct MATERIALS
{
	CMATERIAL m_pReflections[MAX_MATERIALS];
};

class CScene
{
public:
	CScene();
	~CScene() {};

	// ���� ��� ����� ������ ����
	void BuildLightsAndMaterials();

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	// ���� ��� ����� ������ ���� ���ҽ��� �����ϰ� ����
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	// �׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

	// ������ ���콺�� Ű���� �޽����� ó���Ѵ�.
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();


public:
	CPlayer* m_pPlayer = NULL;

protected:
	// ��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�.
	CObjectsShader* m_pObjectShaders = NULL;
	int m_nObjectShaders = 0;

	CBoundingBoxShader* m_pBoundingBoxShaders = NULL;
	int m_nBoundingBoxShaders = 0;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	// ���� ����
	LIGHTS* m_pLights = NULL;

	// ������ ��Ÿ���� ���ҽ��� ���ҽ��� ���� ������
	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

	// ���� ��ü�鿡 ����Ǵ� ����
	MATERIALS* m_pMaterials = NULL;

	// ������ ��Ÿ���� ���ҽ��� ���ҽ��� ���� �������̴�.
	ID3D12Resource* m_pd3dcbMaterials = NULL;
	CMATERIAL* m_pcbMappedMaterials = NULL;
};