#pragma once
#include "Scene.h"
#include "Timer.h"
#include "Player.h"

class CGameFramework : public TemplateSingleton<CGameFramework>
{
private:
	HINSTANCE						m_hInstance;
	HWND							m_hWnd;

	int								m_nWndClientWidth;
	int								m_nWndClientHeight;

	IDXGIFactory4					*m_pdxgiFactory;				// DXGI ���丮 �������̽��� ���� �������̴�.
	IDXGISwapChain3					*m_pdxgiSwapChain;				// ���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���� �ʿ��ϴ�.
	ID3D12Device					*m_pd3dDevice;					// Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���� �ʿ��ϴ�.

	bool							m_bMsaa4xEnable = false;
	UINT							m_nMsaa4xQualityLevels = 0;		// MSAA ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ�.
	
	static const UINT				m_nSwapChainBuffers = 2;		// ���� ü���� �ĸ� ������ �����̴�.
	UINT							m_nSwapChainBufferIndex;		// ���� ���� ü���� �ĸ� ���� �ε����̴�.
	
	ID3D12Resource					*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap			*m_pd3dRtvDescriptorHeap = NULL;
	UINT							m_nRtvDescriptorIncrementSize;	// ���� Ÿ�� ����, ������ �� �������̽� ������, ���� Ÿ�� ������ ������ ũ���̴�.
	
	ID3D12Resource					*m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap			*m_pd3dDsvDescriptorHeap = NULL;
	UINT							m_nDsvDescriptorIncrementSize;	// ����-���ٽ� ����, ������ �� �������̽� ������, ����-���ٽ� ������ ������ ũ���̴�.

	ID3D12CommandQueue				*m_pd3dCommandQueue;
	ID3D12CommandAllocator			*m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList		*m_pd3dCommandList;				// ��� ť, ��� �Ҵ���, ��� ����Ʈ �������̽� �������̴�.

	ID3D12Fence						*m_pd3dFence;
	UINT64							m_nFenceValues[m_nSwapChainBuffers];
	HANDLE							m_hFenceEvent;					// �潺 �������̽� ������, �潺�� ��, �̺�Ʈ �ڵ��̴�.

#if defined(_DEBUG)
	ID3D12Debug* m_pd3dDebugController;
#endif

	GameTimer						* m_GameTimer;

	CScene							* m_pScene = NULL;
	CPlayer							* m_pPlayer = NULL;
	CCamera							* m_pCamera = NULL;

	POINT							m_ptOldCursorPos;				// ���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ

	_TCHAR							m_pszFrameRate[50];				// ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ�

public:
	void Initialize();
	void ShutDown();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);	// �����ӿ�ũ�� �ʱ�ȭ�ϴ� �Լ��̴�(�� �����찡 �����Ǹ� ȣ��ȴ�).
	void OnDestroy();
	
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();					// ���� ü��, ����̽�, ������ ��, ��� ť/�Ҵ���/����Ʈ�� �����ϴ� �Լ��̴�.     
	void CreateRenderTargetViews();
	void CreateDepthStencilView();						// ���� Ÿ�� ��� ����-���ٽ� �並 �����ϴ� �Լ��̴�. 

	void OnResizeBackBuffers();

	void BuildObjects();
	void ReleaseObjects();								// �������� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�.

	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�.
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void WaitForGpuComplete();							// CPU�� GPU�� ����ȭ�ϴ� �Լ��̴�.
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); // �������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
};

