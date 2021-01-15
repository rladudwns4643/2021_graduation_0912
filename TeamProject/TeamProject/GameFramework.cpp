#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;				// ���丮 �������̽� ������
	m_pdxgiSwapChain = NULL;			// ���� ü�� �������̽� ������
	m_pd3dDevice = NULL;				// ����̽� �������̽� ������

	m_pd3dCommandAllocator = NULL;		// ��� �Ҵ���
	m_pd3dCommandQueue = NULL;			// ��� ť
	m_pd3dPipelineState = NULL;			// ���������� ���� ��ü�� ���� �������̽� ������
	m_pd3dCommandList = NULL;			// ��� ����Ʈ �������̽� ������

	// ���� Ÿ�� ���� �ʱ�ȭ
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppd3dRenderTargetBuffers[i] = NULL;
	m_pd3dRtvDescriptorHeap = NULL;		// ���� Ÿ�� ������ �� �������̽� ������
	m_nRtvDescriptorIncrementSize = 0;	// ���� Ÿ�� ������ ���� ũ�� �ʱ�ȭ

	m_pd3dDepthStencilBuffer = NULL;	// ����-���ٽ� ����
	m_pd3dDsvDescriptorHeap = NULL;		// ����-���ٽ� ������ �� �������̽� ������
	m_nDsvDescriptorIncrementSize = 0;	// ����-���ٽ� ������ ���� ũ�� �ʱ�ȭ

	m_nSwapChainBufferIndex = 0;		// ���� ����ü���� ���� �ε���

	m_hFenceEvent = NULL;				// �̺�Ʈ �ڵ�
	m_pd3dFence = NULL;					// �潺 �������̽� ������
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;	// �潺 �� �ʱ�ȭ

	m_pScene = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;		// Ŭ���̾�Ʈ ���� ũ��
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;	// Ŭ���̾�Ʈ ���� ũ��

	// Ÿ�̸� ����
	_tcscpy_s(m_pszFrameRate, _T("TeamProject("));	// ������ ĸ�ǿ� ����� ���ڿ�
}
CGameFramework::~CGameFramework()
{
}

// ���� ���α׷��� ����Ǿ� �� �����찡 �����Ǹ� ȣ��ȴ�.
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	// Direct3D ����̽�, ��� ť�� ��� ����Ʈ, ���� ü�� ���� ����
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilView();

	// �������� ���� ��ü�� �����Ѵ�.
	BuildObjects();

	return(true);
}
// Com ��ü���� Release�� ���ؼ� ���ش�.
void CGameFramework::OnDestroy()
{
	// GPU�� ��� ��� ����Ʈ�� ������ �� ���� ��ٸ�
	WaitForGpuComplete();

	//���� ��ü(���� ���� ��ü)�� �Ҹ�
	ReleaseObjects();

	// �ڵ��� ����
	::CloseHandle(m_hFenceEvent);

	// ���� Ÿ�� ���� �Ҹ�
	for (int i = 0; i < m_nSwapChainBuffers; i++) 
		if (m_ppd3dRenderTargetBuffers[i])
			m_ppd3dRenderTargetBuffers[i]->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();

	// ����-���ٽ� ���� �Ҹ�
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();

	// ��� �Ҵ���, ť, ����Ʈ �Ҹ�
	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	// �潺 �Ҹ�
	if (m_pd3dFence) m_pd3dFence->Release();

	// ����ü��, ����̽�, ���丮 �Ҹ�
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

// ����ü�� ����
void CGameFramework::CreateSwapChain()
{
	// Ŭ���̾�Ʈ ũ�� �޾ƿ���
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	// ����ü�� ���� - ó������ â���� ����
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));			// �ʱ�ȭ
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;					// �ĸ���� ����
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;					// ȭ�� ���� ũ��
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;				// ȭ�� ���� ũ��
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// �ĸ���� �ȼ� ���� - DXGI_FORMAT_R8G8B8A8_UNORM �� Default
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;				// ���� �ֱ�, �������� �и�
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;				// ���� �ֱ�, �������� ����
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// ���۸� � ������� �� ������ - ���� ������� ���
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;			// �������� ó���ϴ� ��� - ���۳����� ���(FLIP�� ��üȭ�鿡���� ����)
	dxgiSwapChainDesc.OutputWindow = m_hWnd;								// ����� ���
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;			// ���� ����, ���ø��� �� �� �� ������
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0; // ����Ƽ ����, 0�� �ָ� ��Ƽ�ٸ���� ���X
	dxgiSwapChainDesc.Windowed = TRUE;										// â��� ��뿩��
	// Ǯ��ũ�� ��忡�� ����ȭ���� �ػ󵵸� ����ü��(�ĸ����)�� ũ�⿡ �°� �����Ѵ�.
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;		// ���� ü���� ���ۿ� ���� ���û����� ����(0�̸� Ǯ��ũ�� �Ұ���) - �������α׷��� ���÷���(����ȭ��) ��带 ���� ����
	// Flags�� UINT �����̱� ������ | �� ����Ͽ� �������� �ɼ��� ���ÿ� ���� �����ϴ�.

	// ���� ü�� ����
	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);
	// ����� ���� �ƴ��� Ȯ��
	if (!m_pdxgiSwapChain)
	{
		MessageBox(NULL, L"Swap Chain Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}
	//��Alt+Enter��Ű�� ������ ��Ȱ��ȭ - DXGI�� �⺻���� �����ϴ� ��ȯ�� �Ͼ�� ������ �� �ǵ���� ��ȯ �� �� ����.
	// �ĸ������ ũ�⸦ ���س��� ������ ũ�Ⱑ �ٲ�� ���� ������ ����
	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	
	// ����ü���� ���� �ĸ���� �ε����� �����Ѵ�.
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

// ����ü�� ���� ����(Ǯ��ũ�� ��� - â���)
void CGameFramework::ChangeSwapChainState()
{
	// CPU��GPU ����ȭ
	WaitForGpuComplete();

	// Ǯ��ũ�� ������� ������ ������� �Ǵ��� �Ŀ� �ݴ� ���� �ٲ���
	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	// ����ȭ�� �ػ󵵷� �� ȭ���� �ٲ�
	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);	// Target�� ����ȭ���� �ǹ� - ����ȭ���� �ػ󵵸� �� �ĸ������ ũ��� �ٲ۴�.

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dRenderTargetBuffers[i])
			m_ppd3dRenderTargetBuffers[i]->Release();

	// ������ ����ü���� �ĸ� ���� ������ �����ϱ� ���� ȣ���ؾ� ��
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight,
		dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

// ����̽� ����
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

	// DXGI(DirectX Graphics Infrastructure)�� �ϵ���������� �̷������ ��ɵ��� ����ϱ� ���� ��
	// DirectX �׷����� ���� �⺻���̰� �������� �����ӿ�ũ�� ����
	// �������α׷��� �ϵ��� ������ �� �ֵ��� ���� ����
	// �ڿ� �ٴ� ���ڵ��� �ǹ� �ϴ� ���� ���� ex. IDXGIFactory4 = 1.4 ����
	// Device ������ Device PPT - 27p Ȯ��

	// �����(Adapter), �����ġ(Output), ����̽�(Device)�� �����ϰ� �ִ� �İ�ü�� Factory ����
	// �����(Adapter) - ����ī��, �����ġ(Output) - �����, ����̽�(Device) - �׷��� ī��
	// Factory�� ���ؼ� �� ��ġ���� �� �� ����Ǿ� �ִ��� ���� ������ ���� �� �ִ�.
	// __uuidof = �� ��ġ�� GUID�� �������ش�.
	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)&m_pdxgiFactory);

	// Device ���� �κ� - m_pd3dDevice �� �׷��� ī�带 �����Ѵ�.
	// ����͸� �̿��Ͽ� ���� ���� ������ ����̽��� �����Ѵ�.
	// ��� �ϵ���� ����� ���Ͽ� Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ����(D3������ ���� ��ġ��Ų��.)
	IDXGIAdapter1* pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice))) break;
	}
	// Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ������ �� ������ WARP ����̽��� ����
	if (!m_pd3dDevice)
	{
		hResult = m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void**)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}
	// ����̽��� ������ �ȵǸ� ���� ���
	if (!m_pd3dDevice)
	{
		MessageBox(NULL, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}

	// MSAA(Multi-Sampling AA) ���� ���ø� ����
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; // Msaa4x ���� ���ø�
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	// ����̽��� �����ϴ� ���� ������ ǰ�� ������ Ȯ��
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	// ���� ������ ǰ�� ������ 1���� ũ�� ���� ���ø��� Ȱ��ȭ
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	if (pd3dAdapter) pd3dAdapter->Release();

	// �潺�� ����(�⺻ �潺)
	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
	// �潺�� ����ȭ�� ���� �̺�Ʈ ��ü�� �����Ѵ�(�̺�Ʈ ��ü�� �ʱⰪ�� FALSE�̴�).
	// �̺�Ʈ�� ����Ǹ�(Signal) �̺�Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� �����Ѵ�.
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

// ��� ť / ��� �Ҵ��� / ��� ����Ʈ ����(��� �������� �����̴�)
void CGameFramework::CreateCommandQueueAndList()
{
	// ����(Direct) ��� ť�� ����
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;			// �⺻ ��� ť
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;			// GPU�� ���� ������ �� �ִ� ��� ����(List)
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);

	// ����(Direct) ��� �Ҵ��ڸ� ����
	// Command List�� �߰��� ��ɵ��� �޸𸮿� �����ϴ� ����
	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);

	// ����(Direct) ��� ����Ʈ�� �����Ѵ�. (Device PPT - 77p)
	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);

	// ��� ����Ʈ�� �����Ǹ� ����(Open) �����̹Ƿ� ����(Closed) ���·� ����
	// ���� ���¿����� ��� ť�� ���� �� �ִ�.(Excute)
	hResult = m_pd3dCommandList->Close();
}

// ���� Ÿ��, ����-���ٽ� ������ �� ����
// ������ �� - ���ҽ��� �����ϴ� �����ڵ��� �����ϴ� �������� �޸� ����(�迭)
void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	// ���� Ÿ�� ������ �� ���̺� ���� ����
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;					// �������� ����(������ ����)
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;				// ����Ÿ�� ��
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;				// Default ��
	d3dDescriptorHeapDesc.NodeMask = 0;											// ���� GPU:0
	// ���� Ÿ�� ������ �� ����
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);
	// ���� Ÿ�� ������ ���� ������ ũ�⸦ ���� - GetDescriptorHandleIncrementSize(ũ�⸦ �������� �Լ�)
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// ����-���ٽ� ������ �� ���̺� ���� ����
	d3dDescriptorHeapDesc.NumDescriptors = 1;									// �������� ���� - 1
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;				// ����-���ٽ� ��
	// ����-���ٽ� ������ �� ����
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
	// ����-���ٽ� ������ ���� ������ ũ�⸦ ����
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

// ����ü���� �� ���ۿ� ���� ���� Ÿ�� �並 ���� �� ���� ����
// ������ ���� �����ϴ� ���(�߿�)
void CGameFramework::CreateRenderTargetViews()
{
	// ����Ÿ�� ������ ���� ���� ������ �ڵ鿡 ����
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppd3dRenderTargetBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

// ����: ������ȯ �� ����ȭ�� [0,1]�� ���� ��(float��)
// ���ٽ�: �ȼ��� ����ŷ �ϴ� ����(����)
// ���ٽ� ���� 0���� �ʱ�ȭ �� �� �ǹ� �ִ� �κп��� ���� �־� ������ �˻��Ҷ� 0�κ��� �پ� �Ѿ(�׸��� �ʾƼ�) �ӵ��鿡�� �����ϴ�.
// ��, �ʿ��� �κи� ���ٽ� ���ۿ� ä�� ������ �ȴ�.
// ���� ����: ������ ������ ��� �ִ�.(�ʱ�ȭ�� 1�� �ʱ�ȭ)
// ������(Ÿ��) ����: ���� ������ ��� �ִ�.(�ʱ�ȭ�� ���ϴ� ������ �ʱ�ȭ)
// ���ٽ� ����: Ư�� ������ ������ �Ǵ� ���� ���� ���� �뵵�� ���ȴ�.
// �ؽ���, ���� = ���ҽ� -> �信 �����ؾ��� �� -> ������ ���� ����

// ����-���ٽ� �� ����
void CGameFramework::CreateDepthStencilView()
{
	// ���ҽ��� ��� �ٷ��� ����
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// �Ը� - 2D �ؽ��ĸ� ���(���ۿ� 2D �ؽ����� ����)
	d3dResourceDesc.Alignment = 0;									// ���� ���� - 0(64KB) - Default��
	d3dResourceDesc.Width = m_nWndClientWidth;						// �迭 ���� ũ��
	d3dResourceDesc.Height = m_nWndClientHeight;					// �迭 ���� ũ��
	d3dResourceDesc.DepthOrArraySize = 1;							// ���� �Ǵ� �迭�� ũ��(3������ ��Ÿ��)
	d3dResourceDesc.MipLevels = 0;									// �ʸʷ���, 0(�ڵ����)
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			// ���ҽ� ����
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;			// ������ ���ҽ��� 1���� ���ҽ��� �����ϱ� ���� ���
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;// ���ҽ��� �뵵 - ����-���ٽ� ���۷� ���

	// ���ҽ��� �����ϴ� ������ �� ����
	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;						// ���� ����(CPU�� ���� ���� ����) - GPU�� �а��� ����, CPU�� ���� �Ұ�
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// ���� ���� CPU ������ �Ӽ�
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;		// ���� ���� �޸� Ǯ
	d3dHeapProperties.CreationNodeMask = 1;									// ���� GPU �����
	d3dHeapProperties.VisibleNodeMask = 1;									// ���� ����Ϳ��� ���ҽ��� �� �� �ִ� ����� ������ ��Ÿ���� ��Ʈ

	// ���ҽ��� ������ ������� �ʱ�ȭ�� �� ����
	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;						// ���� ��(�ʱ�ȭ ���� �� 1.0f)(0.0f~1.0f)
	d3dClearValue.DepthStencil.Stencil = 0;							// ���ٽ� ��(0~255)

	// ����-���ٽ� ���۸� ����
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);

	// ����-���ٽ� ������ ���� ���� ������ �ڵ鿡 ����(Device PPT - 90p)
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// ����-���ٽ� ���� �並 ���� �� ����-���ٽ� ���� ����
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
}

// ��ü ����
void CGameFramework::BuildObjects()
{
	// ��ɾ� ����Ʈ�� ���� open ���·� ����(�ϳ��� ��ɾ� ����Ʈ�� ��� �����ϱ� ����)
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	// �� ��ü�� �����ϰ� ���� ���Ե� ���� ��ü���� ����
	m_pScene = new CScene();
	if (m_pScene) m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	// �÷��̾� ����
	m_pPlayer = new CTestPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->GetTerrain(), 1);
	m_pCamera = m_pPlayer->GetCamera();

	// �� ��ü�� �����ϱ� ���Ͽ� �ʿ��� �׷��� ��� ����Ʈ���� ��� ť�� �߰�
	m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	// ��ɾ� ť�� ��ɾ� ����Ʈ�� �־���
	// ť�� �־����ڸ��� ��ɾ���� �ٷ� ������� ����(�߿�!!)
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	// �׷��� ��� ����Ʈ���� ��� ����� ������ ��ٸ���.
	WaitForGpuComplete();

	// �׷��� ���ҽ����� �����ϴ� ������ ������ ���ε� ���۵��� �Ҹ�
	if (m_pScene) m_pScene->ReleaseUploadBuffers();

	// Ÿ�̸� ����
	m_GameTimer.Reset();
}
// ��ü �Ҹ�
void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
}

// ���콺 �Է� ó��
void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		// ���콺 ĸ�ĸ� �ϰ� ���� ���콺 ��ġ�� ������
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		// ���콺 ĸ�ĸ� ����
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

// Ű���� �Է� ó��
void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		// ī�޶� ��� ��ȯ
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
			ChangeSwapChainState();		// ��üȭ��-â��� ����
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

// ������ �޽��� ó��
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

// ����� �Է�
void CGameFramework::ProcessInput()
{
	// Ű������ ���� ���� ��ȯ
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

		// ���콺 ĸ�� ����
		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		if (::GetCapture() == m_hWnd)
		{
			// ���콺 Ŀ���� ȭ�鿡�� ������ �ʰ� �Ѵ�
			::SetCursor(NULL);

			// ���� ���콺 Ŀ���� ��ġ�� �����´�
			::GetCursorPos(&ptCursorPos);

			// ���콺 ��ư�� ���� ���¿��� ���콺�� ������ ���� ���Ѵ�
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;

			// ���콺 Ŀ���� ��ġ�� ���콺�� �������� ��ġ�� ����
			::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}
		// ���콺 �Ǵ� Ű �Է��� ������ �÷��̾ �̵��ϰų�(dwDirection) ȸ���Ѵ�(cxDelta �Ǵ� cyDelta).
		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				if (pKeysBuffer[VK_RBUTTON] & 0xF0)
					m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
				else
					m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
			// �÷��̾��� �̵� �ӷ� ����
			if (dwDirection) m_pPlayer->Move(dwDirection, m_pPlayer->GetSpeed() * m_GameTimer.GetTimeElapsed(), true);
		}
	}
	// �÷��̾ ������ �̵��ϰ� ī�޶� ����(�߷°� �������� ������ �ӵ� ���Ϳ� ����)
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

// �ִϸ��̼� ������Ʈ
void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());
}

// CPU�� GPU�� ����ȭ
// Resource�� ����ϱ� ���� ����ȭ ���Ѿ���
void CGameFramework::WaitForGpuComplete()
{
	// GPU�� �潺�� ���� �����ϴ� ����� ��� ť�� �߰�
	// �潺��� ��Ÿ���� �����.
	const UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];			// CPU �潺�� ���� ����
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	// �潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ�
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

// ���� ������ �潺�� ����
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

// ������(�����Ӵ� �ѹ� �����)
void CGameFramework::FrameAdvance()
{
	// Ÿ�̸��� �ð��� ���ŵǵ��� �ϰ� ������ ����Ʈ�� ���
	m_GameTimer.Tick(0.0f);

	ProcessInput();				// ����� �Է�

	AnimateObjects();			// �ִϸ��̼� ������Ʈ

	// ��� �Ҵ��ڿ� ��� ����Ʈ�� ����
	HRESULT hResult = m_pd3dCommandAllocator->Reset();					// �޸𸮸� ������ �ǵ�, �������� ��� ����Ʈ�� ����� ��
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);	// Ŀ��� ����Ʈ�� �����԰� ���ÿ� Open���·� ����

	// ���ҽ� �庮�� ���ҽ��� ���� ���� ������ ���� ��ü�̴�.(Device PPT 109p)
	// ���߾����� ó���� ���ҽ��� ���� ���� ó���� �����ϵ����ϴ� �����̴�.
	// ���� ���� Ÿ�ٿ� ���� ������Ʈ�� �����⸦ ��ٸ���.
	// ������Ʈ�� ������ ���� Ÿ�� ������ ���´�
	// ������Ʈ ����(D3D12_RESOURCE_STATE_PRESENT)����
	// ���� Ÿ�� ����(D3D12_RESOURCE_STATE_RENDER_TARGET)�� �ٲ� ���̴�.
	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;								// ���ҽ� �庮�� ���� - ���ҽ� ����(��� ��ȭ)�� ��Ÿ���� ���� �庮
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;									// ���ҽ� �庮�� �и��ϱ� ���� ����
	d3dResourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];	// ���� ���� ��� ���ҽ�
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;						// ���긮�ҽ��� ���� ��� ����
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;					// ���긮�ҽ��� ���� ��� ����
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;			// ���긮�ҽ� �ε��� - ��� ���긮�ҽ�
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	// ������ ���� Ÿ�ٿ� �ش��ϴ� �������� CPU �ּ�(�ڵ�)�� ���
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);
	// ����-���ٽ� �������� CPU �ּҸ� ���
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// ���� Ÿ�� ��(������)�� ����-���ٽ� ��(������)�� ���-���� �ܰ�(OM) ���������ο� �����Ѵ�.
	// 1 = ����Ÿ�� �������� ����(�ִ� 8��)
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);

	// ���ϴ� �������� ���� Ÿ��(��)�� ����
	float pfClearColor[4] = { 0.58f, 0.62f, 1.0f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, NULL);
	// ���ϴ� ������ ����-���ٽ�(��)�� ����
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);


	// ������
	if (m_pScene) m_pScene->Render(m_pd3dCommandList, m_pCamera);

	// 3��Ī ī�޶��� �� �÷��̾ �׻� ���̵��� �������Ѵ�.
#ifdef _WITH_PLAYER_TOP
	// ���� Ÿ���� �״�� �ΰ� ���� ���۸� 1.0���� ����� �÷��̾ �������ϸ� �÷��̾�� ������ �׷��� ���̴�.
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
	// 3��Ī ī�޶��� �� �÷��̾ �������Ѵ�.
	if (m_pPlayer) m_pPlayer->Render(m_pd3dCommandList, m_pCamera);


	// ���� ���� Ÿ�ٿ� ���� �������� �����⸦ ��ٸ���.
	// GPU�� ���� Ÿ��(����)�� �� �̻� ������� ������ ���� Ÿ�� �� ���´� ������Ʈ ����(D3D12_RESOURCE_STATE_PRESENT)�� �ٲ� ���̴�.
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;	// ������Ʈ ���·� �ٲ�
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	// ��� ����Ʈ�� ���� ���·� ����
	hResult = m_pd3dCommandList->Close();

	// ��� ����Ʈ�� ��� ť�� �߰��Ͽ� ����
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);

	// GPU�� ��� ��� ����Ʈ�� ������ �� ���� ��ٸ�
	// �������� ���� CPU�� GPU�� Fence�� ���ؼ� ����ȭ
	WaitForGpuComplete();

	// ����ü���� ������Ʈ�Ѵ�.
	// ������Ʈ�� �ϸ� ���� ���� Ÿ��(�ĸ����)�� ������ ������۷� �Ű����� ���� Ÿ�� �ε����� �ٲ� ���̴�.
	/*
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);
	*/
	m_pdxgiSwapChain->Present(0, 0);
	
	// ���� ������ �潺�� ����
	MoveToNextFrame();

	// ������ ������ ����Ʈ�� ���ڿ��� �����ͼ� �� �������� Ÿ��Ʋ�� ����Ѵ�. 
	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}