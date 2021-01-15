#pragma once
#include "Scene.h"
#include "Camera.h"
#include "Timer.h"
#include "Player.h"

class CGameFramework
{
private:
	HINSTANCE m_hInstance;				// Instance Handle
	HWND m_hWnd;						// ������ �ڵ�

	int m_nWndClientWidth;				// Ŭ���̾�Ʈ ���� ũ��
	int m_nWndClientHeight;				// Ŭ���̾�Ʈ ���� ũ��

	IDXGIFactory4* m_pdxgiFactory;		// DXGI ���丮 �������̽��� ���� �������̴�.
	IDXGISwapChain3* m_pdxgiSwapChain;	// ���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D12Device* m_pd3dDevice;			// Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	// Device�� �׷���ī�带 �߻�ȭ�� ��ü�̴�.(�ϵ���� ��ġ�� �߻�ȭ��)

	bool m_bMsaa4xEnable = false;		// MSAA = Multi-Sampling AA
	UINT m_nMsaa4xQualityLevels = 0;	// MSAA ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ�.

	static const UINT m_nSwapChainBuffers = 2;	// ����ü���� �ĸ� ������ �����̴�
	UINT m_nSwapChainBufferIndex;				// ���� ����ü���� ���� �ε����̴�.

	// ���ҽ��� �ٷ�� ����, ��, ������ ��
	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];	// ���� Ÿ�� ����(�ĸ� ���۷� ����� ����)
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;						// ���� Ÿ�� ������ �� �������̽� ������
	UINT m_nRtvDescriptorIncrementSize;									// ���� Ÿ�� ������ ������ ũ��

	ID3D12Resource* m_pd3dDepthStencilBuffer;							// ����-���ٽ� ����
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;						// ����-���ٽ� ������ �� �������̽� ������
	UINT m_nDsvDescriptorIncrementSize;									// ����-���ٽ� ������ ������ ũ��

	// ��ɾ �ٷ�� ��� ť, �Ҵ���, ����Ʈ
	ID3D12CommandQueue* m_pd3dCommandQueue;								// ��� ť
	ID3D12CommandAllocator* m_pd3dCommandAllocator;						// ��� �Ҵ���(�޸� ����)
	ID3D12GraphicsCommandList* m_pd3dCommandList;						// ��� ����Ʈ �������̽� ������


	ID3D12PipelineState* m_pd3dPipelineState;							// �׷��Ƚ� ���������� ���� ��ü�� ���� �������̽� ������

	ID3D12Fence* m_pd3dFence;											// �潺 �������̽� ������
	UINT64 m_nFenceValues[m_nSwapChainBuffers];							// �� �ĸ� ������ �潺�� ��
	HANDLE m_hFenceEvent;												// �̺�Ʈ �ڵ�

private:// Ÿ�̸� ����
	CGameTimer m_GameTimer;		// ���� �����ӿ�ũ���� ����� Ÿ�̸�
	_TCHAR m_pszFrameRate[50];	// ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ�

private:// �� ����
	CScene* m_pScene;

public:// ī�޶� ����
	CCamera* m_pCamera = NULL;

public:// Player ����
	CPlayer* m_pPlayer = NULL;
	// ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ
	POINT m_ptOldCursorPos;

public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);					// �����ӿ�ũ�� �ʱ�ȭ�ϴ� �Լ�(�� �����찡 �����Ǹ� ȣ��)
	void OnDestroy();													// �����ӿ�ũ �Ҹ�

	void CreateSwapChain();												// ����ü�� ����
	void CreateRtvAndDsvDescriptorHeaps();								// ������ �� ����
	void CreateDirect3DDevice();										// ����̽� ����
	void CreateCommandQueueAndList();									// ���ť/�Ҵ���/����Ʈ ����

	void CreateRenderTargetViews();										// ���� Ÿ�� �� ����
	void CreateDepthStencilView();										// ����-���ٽ� �� ����
	// �� = ������ ���� �� - � ������ ���ҽ��� �ٷ��� �����ϴ� ��

	void BuildObjects();												// �������� ���� ��ü ����
	void ReleaseObjects();												// ���Ӱ�ü �Ҹ�

	// ����ü��
	void ChangeSwapChainState();										// ����ü�� ���� ����

	// �����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)
	void ProcessInput();												// ����� �Է�
	void AnimateObjects();												// �ִϸ��̼�
	void FrameAdvance();												// ������

	void WaitForGpuComplete();											// CPU�� GPU�� ����ȭ�ϴ� �Լ�
	void MoveToNextFrame();												// ���� ������ �潺�� ����

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);				// ���콺 �Է� ó��
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);				// Ű���� �Է� ó��
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);	// ������ �޽��� ó��
};