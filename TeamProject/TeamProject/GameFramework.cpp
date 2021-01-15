#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;				// 팩토리 인터페이스 포인터
	m_pdxgiSwapChain = NULL;			// 스왑 체인 인터페이스 포인터
	m_pd3dDevice = NULL;				// 디바이스 인터페이스 포인터

	m_pd3dCommandAllocator = NULL;		// 명령 할당자
	m_pd3dCommandQueue = NULL;			// 명령 큐
	m_pd3dPipelineState = NULL;			// 파이프라인 상태 객체에 대한 인터페이스 포인터
	m_pd3dCommandList = NULL;			// 명령 리스트 인터페이스 포인터

	// 랜더 타겟 버퍼 초기화
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppd3dRenderTargetBuffers[i] = NULL;
	m_pd3dRtvDescriptorHeap = NULL;		// 렌더 타겟 서술자 힙 인터페이스 포인터
	m_nRtvDescriptorIncrementSize = 0;	// 렌더 타겟 서술자 원소 크기 초기화

	m_pd3dDepthStencilBuffer = NULL;	// 깊이-스텐실 버퍼
	m_pd3dDsvDescriptorHeap = NULL;		// 깊이-스텐실 서술자 힙 인터페이스 포인터
	m_nDsvDescriptorIncrementSize = 0;	// 깊이-스텐실 서술자 원소 크기 초기화

	m_nSwapChainBufferIndex = 0;		// 현재 스왑체인의 버퍼 인덱스

	m_hFenceEvent = NULL;				// 이벤트 핸들
	m_pd3dFence = NULL;					// 펜스 인터페이스 포인터
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;	// 펜스 값 초기화

	m_pScene = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;		// 클라이언트 가로 크기
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;	// 클라이언트 세로 크기

	// 타이머 관련
	_tcscpy_s(m_pszFrameRate, _T("TeamProject("));	// 윈도우 캡션에 출력할 문자열
}
CGameFramework::~CGameFramework()
{
}

// 응용 프로그램이 실행되어 주 윈도우가 생성되면 호출된다.
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	// Direct3D 디바이스, 명령 큐와 명령 리스트, 스왑 체인 등을 생성
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilView();

	// 렌더링할 게임 객체를 생성한다.
	BuildObjects();

	return(true);
}
// Com 객체들은 Release를 통해서 없앤다.
void CGameFramework::OnDestroy()
{
	// GPU가 모든 명령 리스트를 실행할 때 까지 기다림
	WaitForGpuComplete();

	//게임 객체(게임 월드 객체)를 소멸
	ReleaseObjects();

	// 핸들을 닫음
	::CloseHandle(m_hFenceEvent);

	// 렌더 타겟 버퍼 소멸
	for (int i = 0; i < m_nSwapChainBuffers; i++) 
		if (m_ppd3dRenderTargetBuffers[i])
			m_ppd3dRenderTargetBuffers[i]->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();

	// 깊이-스텐실 버퍼 소멸
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();

	// 명령 할당자, 큐, 리스트 소멸
	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	// 펜스 소멸
	if (m_pd3dFence) m_pd3dFence->Release();

	// 스왑체인, 디바이스, 팩토리 소멸
	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();

#if defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

// 스왑체인 생성
void CGameFramework::CreateSwapChain()
{
	// 클라이언트 크기 받아오기
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	// 스왑체인 설정 - 처음에는 창모드로 설정
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));			// 초기화
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;					// 후면버퍼 개수
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;					// 화면 가로 크기
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;				// 화면 세로 크기
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// 후면버퍼 픽셀 형식 - DXGI_FORMAT_R8G8B8A8_UNORM 가 Default
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;				// 갱신 주기, 프레임의 분모
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;				// 갱신 주기, 프레임의 분자
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// 버퍼를 어떤 방식으로 쓸 것인지 - 렌더 대상으로 사용
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;			// 스와핑을 처리하는 방법 - 버퍼내용을 폐기(FLIP은 전체화면에서만 가능)
	dxgiSwapChainDesc.OutputWindow = m_hWnd;								// 출력할 장소
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;			// 샘플 갯수, 샘플링을 몇 번 할 것인지
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0; // 퀄리티 수준, 0을 주면 안티앨리어싱 사용X
	dxgiSwapChainDesc.Windowed = TRUE;										// 창모드 사용여부
	// 풀스크린 모드에서 바탕화면의 해상도를 스왑체인(후면버퍼)의 크기에 맞게 변경한다.
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;		// 스왑 체인의 동작에 대한 선택사항을 지정(0이면 풀스크린 불가능) - 응용프로그램이 디스플레이(바탕화면) 모드를 변경 가능
	// Flags는 UINT 형식이기 때문에 | 를 사용하여 여러개의 옵션을 동시에 적용 가능하다.

	// 스왑 체인 생성
	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);
	// 제대로 생성 됐는지 확인
	if (!m_pdxgiSwapChain)
	{
		MessageBox(NULL, L"Swap Chain Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}
	//“Alt+Enter”키의 동작을 비활성화 - DXGI가 기본으로 제공하는 전환이 일어나기 때문에 내 의도대로 전환 할 수 없다.
	// 후면버퍼의 크기를 정해놨기 때문에 크기가 바뀌는 것을 원하지 않음
	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	
	// 스왑체인의 현재 후면버퍼 인덱스를 저장한다.
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

// 스왑체인 상태 변경(풀스크린 모드 - 창모드)
void CGameFramework::ChangeSwapChainState()
{
	// CPU와GPU 동기화
	WaitForGpuComplete();

	// 풀스크린 모드인지 윈도우 모드인지 판단한 후에 반대 모드로 바꿔줌
	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	// 바탕화면 해상도로 내 화면을 바꿈
	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);	// Target은 바탕화면을 의미 - 바탕화면의 해상도를 내 후면버퍼의 크기로 바꾼다.

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dRenderTargetBuffers[i])
			m_ppd3dRenderTargetBuffers[i]->Release();

	// 기존의 스왑체인의 후면 버퍼 형식을 유지하기 위해 호출해야 함
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight,
		dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

// 디바이스 생성
void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;	// SUCCEEDED or FAILED

	UINT nDXGIFactoryFlags = 0;
#if defined(_DEBUG)
	ID3D12Debug* pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	// DXGI(DirectX Graphics Infrastructure)란 하드웨어적으로 이루어지는 기능들을 사용하기 위해 씀
	// DirectX 그래픽을 위한 기본적이고 공통적인 프레임워크를 제공
	// 응용프로그램이 하드웨어에 접근할 수 있도록 돕는 역할
	// 뒤에 붙는 숫자들이 의미 하는 값은 버전 ex. IDXGIFactory4 = 1.4 버전
	// Device 구조는 Device PPT - 27p 확인

	// 어댑터(Adapter), 출력장치(Output), 디바이스(Device)를 포함하고 있는 컴객체인 Factory 생성
	// 어댑터(Adapter) - 비디오카드, 출력장치(Output) - 모니터, 디바이스(Device) - 그래픽 카드
	// Factory를 통해서 각 장치들이 몇 개 연결되어 있는지 등의 정보를 얻을 수 있다.
	// __uuidof = 내 장치의 GUID를 리턴해준다.
	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)&m_pdxgiFactory);

	// Device 생성 부분 - m_pd3dDevice 에 그래픽 카드를 연결한다.
	// 어댑터를 이용하여 가장 좋은 성능의 디바이스를 연결한다.
	// 모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성(D3버전과 보통 일치시킨다.)
	IDXGIAdapter1* pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice))) break;
	}
	// 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성
	if (!m_pd3dDevice)
	{
		hResult = m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void**)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}
	// 디바이스가 생성이 안되면 에러 출력
	if (!m_pd3dDevice)
	{
		MessageBox(NULL, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}

	// MSAA(Multi-Sampling AA) 다중 샘플링 설정
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; // Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	// 디바이스가 지원하는 다중 샘플의 품질 수준을 확인
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	// 다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	if (pd3dAdapter) pd3dAdapter->Release();

	// 펜스를 생성(기본 펜스)
	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
	// 펜스와 동기화를 위한 이벤트 객체를 생성한다(이벤트 객체의 초기값을 FALSE이다).
	// 이벤트가 실행되면(Signal) 이벤트의 값을 자동적으로 FALSE가 되도록 생성한다.
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

// 명령 큐 / 명령 할당자 / 명령 리스트 생성(모두 렌더링이 목적이다)
void CGameFramework::CreateCommandQueueAndList()
{
	// 직접(Direct) 명령 큐를 생성
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;			// 기본 명령 큐
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;			// GPU가 직접 실행할 수 있는 명령 버퍼(List)
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);

	// 직접(Direct) 명령 할당자를 생성
	// Command List에 추가된 명령들을 메모리에 저장하는 역할
	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);

	// 직접(Direct) 명령 리스트를 생성한다. (Device PPT - 77p)
	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);

	// 명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만듦
	// 닫힌 상태에서만 명령 큐에 넣을 수 있다.(Excute)
	hResult = m_pd3dCommandList->Close();
}

// 렌더 타겟, 깊이-스텐실 서술자 힙 생성
// 서술자 힙 - 리소스를 서술하는 서술자들을 저장하는 연속적인 메모리 영역(배열)
void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	// 렌더 타겟 서술자 힙 테이블 구조 설정
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;					// 서술자의 개수(버퍼의 개수)
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;				// 렌더타겟 뷰
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;				// Default 값
	d3dDescriptorHeapDesc.NodeMask = 0;											// 단인 GPU:0
	// 렌더 타겟 서술자 힙 생성
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);
	// 렌더 타겟 서술자 힙의 원소의 크기를 저장 - GetDescriptorHandleIncrementSize(크기를 가져오는 함수)
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// 깊이-스텐실 서술자 힙 테이블 구조 설정
	d3dDescriptorHeapDesc.NumDescriptors = 1;									// 서술자의 개수 - 1
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;				// 깊이-스텐실 뷰
	// 깊이-스텐실 서술자 힙 생성
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
	// 깊이-스텐실 서술자 힙의 원소의 크기를 저장
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

// 스왑체인의 각 버퍼에 대한 렌더 타겟 뷰를 생성 후 힙에 저장
// 서술자 힙에 저장하는 방법(중요)
void CGameFramework::CreateRenderTargetViews()
{
	// 렌더타겟 서술자 힙의 시작 지점을 핸들에 저장
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppd3dRenderTargetBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

// 깊이: 투영변환 후 정규화된 [0,1]인 깊이 값(float값)
// 스텐실: 픽셀을 마스킹 하는 역할(양초)
// 스텐실 값을 0으로 초기화 한 뒤 의미 있는 부분에만 값을 넣어 놓으면 검사할때 0부분은 뛰어 넘어서(그리지 않아서) 속도면에서 유리하다.
// 즉, 필요한 부분만 스텐실 버퍼에 채워 놓으면 된다.
// 깊이 버퍼: 깊이의 정보를 담고 있다.(초기화시 1로 초기화)
// 프레임(타겟) 버퍼: 색의 정보를 담고 있다.(초기화시 원하는 색으로 초기화)
// 스텐실 버퍼: 특정 영역을 렌더링 되는 것을 막기 위한 용도로 사용된다.
// 텍스쳐, 버퍼 = 리소스 -> 뷰에 저장해야함 뷰 -> 서술자 힙에 저장

// 깊이-스텐실 뷰 생성
void CGameFramework::CreateDepthStencilView()
{
	// 리소스를 어떻게 다룰지 설정
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 규모 - 2D 텍스쳐를 사용(버퍼와 2D 텍스쳐중 선택)
	d3dResourceDesc.Alignment = 0;									// 정렬 기준 - 0(64KB) - Default값
	d3dResourceDesc.Width = m_nWndClientWidth;						// 배열 가로 크기
	d3dResourceDesc.Height = m_nWndClientHeight;					// 배열 세로 크기
	d3dResourceDesc.DepthOrArraySize = 1;							// 깊이 또는 배열의 크기(3차원을 나타냄)
	d3dResourceDesc.MipLevels = 0;									// 맵맵레벨, 0(자동계산)
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			// 리소스 형식
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;			// 다차원 리소스를 1차원 리소스로 매핑하기 위한 방법
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;// 리소스의 용도 - 깊이-스텐실 버퍼로 사용

	// 리소스를 저장하는 역할인 힙 설정
	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;						// 힙의 유형(CPU의 접근 관련 설정) - GPU는 읽고쓰기 가능, CPU는 접근 불가
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// 힙에 대한 CPU 페이지 속성
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;		// 힙에 대한 메모리 풀
	d3dHeapProperties.CreationNodeMask = 1;									// 단일 GPU 어댑터
	d3dHeapProperties.VisibleNodeMask = 1;									// 다중 어댑터에서 리소스를 볼 수 있는 노드의 집합을 나타내는 비트

	// 리소스를 최적의 방법으로 초기화할 값 설정
	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;						// 깊이 값(초기화 기준 값 1.0f)(0.0f~1.0f)
	d3dClearValue.DepthStencil.Stencil = 0;							// 스텐실 값(0~255)

	// 깊이-스텐실 버퍼를 생성
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);

	// 깊이-스텐실 서술자 힙의 시작 지점을 핸들에 저장(Device PPT - 90p)
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// 깊이-스텐실 버퍼 뷰를 생성 후 깊이-스텐실 힙에 저장
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
}

// 객체 생성
void CGameFramework::BuildObjects()
{
	// 명령어 리스트를 비우고 open 상태로 만듦(하나의 명령어 리스트를 계속 재사용하기 위함)
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	// 씬 객체를 생성하고 씬에 포함될 게임 객체들을 생성
	m_pScene = new CScene();
	if (m_pScene) m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	// 플레이어 생성
	m_pPlayer = new CTestPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->GetTerrain(), 1);
	m_pCamera = m_pPlayer->GetCamera();

	// 씬 객체를 생성하기 위하여 필요한 그래픽 명령 리스트들을 명령 큐에 추가
	m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	// 명령어 큐에 명령어 리스트를 넣어줌
	// 큐에 넣어주자마자 명령어들이 바로 실행되지 않음(중요!!)
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	// 그래픽 명령 리스트들이 모두 실행될 때까지 기다린다.
	WaitForGpuComplete();

	// 그래픽 리소스들을 생성하는 과정에 생성된 업로드 버퍼들을 소멸
	if (m_pScene) m_pScene->ReleaseUploadBuffers();

	// 타이머 리셋
	m_GameTimer.Reset();
}
// 객체 소멸
void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
}

// 마우스 입력 처리
void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		// 마우스 캡쳐를 하고 현재 마우스 위치를 가져옴
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		// 마우스 캡쳐를 해제
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

// 키보드 입력 처리
void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		// 카메라 모드 변환
		case VK_F1:
		case VK_F2:
		case VK_F3:
			if (m_pPlayer) m_pCamera = m_pPlayer->ChangeCamera((wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
			break;
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_F8:
			break;
		case VK_F9:
			ChangeSwapChainState();		// 전체화면-창모드 변경
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

// 윈도우 메시지 처리
LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
			m_GameTimer.Stop();
		else
			m_GameTimer.Start();
		break;
	}
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

// 사용자 입력
void CGameFramework::ProcessInput()
{
	// 키보드의 상태 버퍼 반환
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pScene) bProcessedByScene = m_pScene->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene)
	{
		DWORD dwDirection = 0;
		if (::GetKeyboardState(pKeysBuffer))
		{
			if (pKeysBuffer[0x57] & 0xF0) dwDirection |= DIR_FORWARD;		// 'W'
			if (pKeysBuffer[0x53] & 0xF0) dwDirection |= DIR_BACKWARD;		// 'S'
			if (pKeysBuffer[0X41] & 0xF0) dwDirection |= DIR_LEFT;			// 'A'
			if (pKeysBuffer[0x44] & 0xF0) dwDirection |= DIR_RIGHT;			// 'D'
			if (pKeysBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;		// 'PageUp'
			if (pKeysBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;		// 'PageDown'
		}

		// 마우스 캡쳐 관련
		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		if (::GetCapture() == m_hWnd)
		{
			// 마우스 커서를 화면에서 보이지 않게 한다
			::SetCursor(NULL);

			// 현재 마우스 커서의 위치를 가져온다
			::GetCursorPos(&ptCursorPos);

			// 마우스 버튼이 눌린 상태에서 마우스가 움직인 양을 구한다
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;

			// 마우스 커서의 위치를 마우스가 눌려졌던 위치로 설정
			::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}
		// 마우스 또는 키 입력이 있으면 플레이어를 이동하거나(dwDirection) 회전한다(cxDelta 또는 cyDelta).
		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				if (pKeysBuffer[VK_RBUTTON] & 0xF0)
					m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
				else
					m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
			// 플레이어의 이동 속력 설정
			if (dwDirection) m_pPlayer->Move(dwDirection, m_pPlayer->GetSpeed() * m_GameTimer.GetTimeElapsed(), true);
		}
	}
	// 플레이어를 실제로 이동하고 카메라를 갱신(중력과 마찰력의 영향을 속도 벡터에 적용)
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

// 애니메이션 오브젝트
void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());
}

// CPU와 GPU를 동기화
// Resource를 사용하기 위해 동기화 시켜야함
void CGameFramework::WaitForGpuComplete()
{
	// GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가
	// 펜스라는 울타리를 만든다.
	const UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];			// CPU 펜스의 값을 증가
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	// 펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다림
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

// 현재 버퍼의 펜스값 갱신
void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

// 렌더링(프레임당 한번 실행됨)
void CGameFramework::FrameAdvance()
{
	// 타이머의 시간이 갱신되도록 하고 프레임 레이트를 계산
	m_GameTimer.Tick(0.0f);

	ProcessInput();				// 사용자 입력

	AnimateObjects();			// 애니메이션 오브젝트

	// 명령 할당자와 명령 리스트를 리셋
	HRESULT hResult = m_pd3dCommandAllocator->Reset();					// 메모리를 재사용할 의도, 실행중인 명령 리스트가 없어야 함
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);	// 커멘드 리스트를 리셋함과 동시에 Open상태로 만듦

	// 리소스 장벽은 리소스에 대한 상태 관리를 위한 객체이다.(Device PPT 109p)
	// 다중쓰레드 처리와 리소스에 대한 사전 처리를 가능하도록하는 역할이다.
	// 현재 렌더 타겟에 대한 프리젠트가 끝나기를 기다린다.
	// 프리젠트가 끝나면 렌더 타겟 버퍼의 상태는
	// 프리젠트 상태(D3D12_RESOURCE_STATE_PRESENT)에서
	// 렌더 타겟 상태(D3D12_RESOURCE_STATE_RENDER_TARGET)로 바뀔 것이다.
	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;								// 리소스 장벽의 유형 - 리소스 전이(사용 변화)를 나타내는 전이 장벽
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;									// 리소스 장벽을 분리하기 위한 설정
	d3dResourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];	// 상태 전이 대상 리소스
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;						// 서브리소스의 이전 사용 상태
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;					// 서브리소스의 이후 사용 상태
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;			// 서브리소스 인덱스 - 모든 서브리소스
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	// 현재의 렌더 타겟에 해당하는 서술자의 CPU 주소(핸들)를 계산
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);
	// 깊이-스텐실 서술자의 CPU 주소를 계산
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// 렌더 타겟 뷰(서술자)와 깊이-스텐실 뷰(서술자)를 출력-병합 단계(OM) 파이프라인에 연결한다.
	// 1 = 렌더타겟 서술자의 개수(최대 8개)
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);

	// 원하는 색상으로 렌더 타겟(뷰)을 지움
	float pfClearColor[4] = { 0.58f, 0.62f, 1.0f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, NULL);
	// 원하는 값으로 깊이-스텐실(뷰)을 지움
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);


	// 렌더링
	if (m_pScene) m_pScene->Render(m_pd3dCommandList, m_pCamera);

	// 3인칭 카메라일 때 플레이어가 항상 보이도록 렌더링한다.
#ifdef _WITH_PLAYER_TOP
	// 렌더 타겟은 그대로 두고 깊이 버퍼를 1.0으로 지우고 플레이어를 렌더링하면 플레이어는 무조건 그려질 것이다.
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
	// 3인칭 카메라일 때 플레이어를 렌더링한다.
	if (m_pPlayer) m_pPlayer->Render(m_pd3dCommandList, m_pCamera);


	// 현재 렌더 타겟에 대한 렌더링이 끝나기를 기다린다.
	// GPU가 렌더 타겟(버퍼)을 더 이상 사용하지 않으면 렌더 타겟 의 상태는 프리젠트 상태(D3D12_RESOURCE_STATE_PRESENT)로 바뀔 것이다.
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;	// 프리젠트 상태로 바꿈
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	// 명령 리스트를 닫힌 상태로 만듦
	hResult = m_pd3dCommandList->Close();

	// 명령 리스트를 명령 큐에 추가하여 실행
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);

	// GPU가 모든 명령 리스트를 실행할 때 까지 기다림
	// 매프레임 마다 CPU와 GPU를 Fence를 통해서 동기화
	WaitForGpuComplete();

	// 스왑체인을 프리젠트한다.
	// 프리젠트를 하면 현재 렌더 타겟(후면버퍼)의 내용이 전면버퍼로 옮겨지고 렌더 타겟 인덱스가 바뀔 것이다.
	/*
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);
	*/
	m_pdxgiSwapChain->Present(0, 0);
	
	// 현재 버퍼의 펜스값 갱신
	MoveToNextFrame();

	// 현재의 프레임 레이트를 문자열로 가져와서 주 윈도우의 타이틀로 출력한다. 
	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}