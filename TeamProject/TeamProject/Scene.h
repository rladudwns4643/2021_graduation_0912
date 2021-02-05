#pragma once
#include "Timer.h"
#include "Camera.h"
#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene() {};

	// ������ ���콺�� Ű���� �޽����� ó��
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();

	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }	// ���̸� ��ȯ

	// �׷��� ��Ʈ �ñ׳��ĸ� ����
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

protected:
	// ��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ��
	//CObjectsShader* m_pShaders = NULL;
	CInstancingShader* m_pShaders = NULL;
	int m_nShaders = 0;

	// ���̸� ����
	CHeightMapTerrain* m_pTerrain = NULL;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;	// ��Ʈ �ñ׳��ĸ� ��Ÿ���� �������̽� ������
};