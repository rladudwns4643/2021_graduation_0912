#pragma once
#include "Timer.h"
#include "Camera.h"
#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene() {};

	// 씬에서 마우스와 키보드 메시지를 처리
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();

	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }	// 높이맵 반환

	// 그래픽 루트 시그너쳐를 생성
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

protected:
	// 배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현
	//CObjectsShader* m_pShaders = NULL;
	CInstancingShader* m_pShaders = NULL;
	int m_nShaders = 0;

	// 높이맵 관련
	CHeightMapTerrain* m_pTerrain = NULL;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;	// 루트 시그너쳐를 나타내는 인터페이스 포인터
};