#pragma once
#include "Scene.h"
#include "Camera.h"
#include "Timer.h"
#include "Player.h"

class CGameFramework
{
private:
	HINSTANCE m_hInstance;				// Instance Handle
	HWND m_hWnd;						// 윈도우 핸들

	int m_nWndClientWidth;				// 클라이언트 가로 크기
	int m_nWndClientHeight;				// 클라이언트 세로 크기

	IDXGIFactory4* m_pdxgiFactory;		// DXGI 팩토리 인터페이스에 대한 포인터이다.
	IDXGISwapChain3* m_pdxgiSwapChain;	// 스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다.
	ID3D12Device* m_pd3dDevice;			// Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.
	// Device는 그래픽카드를 추상화한 객체이다.(하드웨어 장치를 추상화함)

	bool m_bMsaa4xEnable = false;		// MSAA = Multi-Sampling AA
	UINT m_nMsaa4xQualityLevels = 0;	// MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.

	static const UINT m_nSwapChainBuffers = 2;	// 스왑체인의 후면 버퍼의 개수이다
	UINT m_nSwapChainBufferIndex;				// 현재 스왑체인의 버퍼 인덱스이다.

	// 리소스를 다루는 버퍼, 뷰, 서술자 힙
	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];	// 랜더 타겟 버퍼(후면 버퍼로 사용할 것임)
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;						// 랜더 타겟 서술자 힙 인터페이스 포인터
	UINT m_nRtvDescriptorIncrementSize;									// 렌더 타겟 서술자 원소의 크기

	ID3D12Resource* m_pd3dDepthStencilBuffer;							// 깊이-스텐실 버퍼
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;						// 깊이-스텐실 서술자 힙 인터페이스 포인터
	UINT m_nDsvDescriptorIncrementSize;									// 깊이-스텐실 서술자 원소의 크기

	// 명령어를 다루는 명령 큐, 할당자, 리스트
	ID3D12CommandQueue* m_pd3dCommandQueue;								// 명령 큐
	ID3D12CommandAllocator* m_pd3dCommandAllocator;						// 명령 할당자(메모리 저장)
	ID3D12GraphicsCommandList* m_pd3dCommandList;						// 명령 리스트 인터페이스 포인터


	ID3D12PipelineState* m_pd3dPipelineState;							// 그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터

	ID3D12Fence* m_pd3dFence;											// 펜스 인터페이스 포인터
	UINT64 m_nFenceValues[m_nSwapChainBuffers];							// 각 후면 버퍼의 펜스의 값
	HANDLE m_hFenceEvent;												// 이벤트 핸들

private:// 타이머 관련
	CGameTimer m_GameTimer;		// 게임 프레임워크에서 사용할 타이머
	_TCHAR m_pszFrameRate[50];	// 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열

private:// 씬 관련
	CScene* m_pScene;

public:// 카메라 관련
	CCamera* m_pCamera = NULL;

public:// Player 관련
	CPlayer* m_pPlayer = NULL;
	// 마우스 버튼을 클릭할 때의 마우스 커서의 위치
	POINT m_ptOldCursorPos;

public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);					// 프레임워크를 초기화하는 함수(주 윈도우가 생성되면 호출)
	void OnDestroy();													// 프레임워크 소멸

	void CreateSwapChain();												// 스왑체인 생성
	void CreateRtvAndDsvDescriptorHeaps();								// 서술자 힙 생성
	void CreateDirect3DDevice();										// 디바이스 생성
	void CreateCommandQueueAndList();									// 명령큐/할당자/리스트 생성

	void CreateRenderTargetViews();										// 랜더 타겟 뷰 생성
	void CreateDepthStencilView();										// 깊이-스텐실 뷰 생성
	// 뷰 = 서술자 같은 뜻 - 어떤 식으로 리소스를 다룰지 결정하는 것

	void BuildObjects();												// 렌더링할 게임 객체 생성
	void ReleaseObjects();												// 게임객체 소멸

	// 스왑체인
	void ChangeSwapChainState();										// 스왑체인 상태 변경

	// 프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)
	void ProcessInput();												// 사용자 입력
	void AnimateObjects();												// 애니메이션
	void FrameAdvance();												// 렌더링

	void WaitForGpuComplete();											// CPU와 GPU를 동기화하는 함수
	void MoveToNextFrame();												// 현재 버퍼의 펜스값 갱신

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);				// 마우스 입력 처리
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);				// 키보드 입력 처리
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);	// 윈도우 메시지 처리
};