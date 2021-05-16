#include "pch.h"
#include "GameCore.h"
#include "IGameApp.h"
#include "Timer.h"
#include "GraphicsRenderer.h"
#include "CommandContext.h"
#include "TOY_GROUND.h"
#include "InputHandler.h"
#include "netCore.h"
#include "Service.h"

using namespace Core;

namespace Core
{
#ifdef DEBUG_SERVER
	NetCore* g_netcore;
	Service* g_service;
#endif
	GameCore* g_Core = nullptr;
	GameTimer* g_GameTimer = nullptr;


	HINSTANCE			g_hAppInst = nullptr; // application instance handle
	HWND				g_hMainWnd = nullptr; // main window handle
	HWND				g_hVideoWnd = nullptr; // logo video handle
	const wchar_t* g_AppName = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Device> g_Device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> g_CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> g_DirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> g_CommandList;

	Microsoft::WRL::ComPtr<ID2D1DeviceContext2> g_D2dDeviceContext;
	Microsoft::WRL::ComPtr<IDWriteFactory> g_DWriteFactory;

	int g_DisplayWidth = 1280;
	int g_DisplayHeight = 720;
	D3D_DRIVER_TYPE g_d3dDriverType = D3D_DRIVER_TYPE_UNKNOWN;
	DXGI_FORMAT g_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT g_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	bool      g_4xMsaaState = false;    // 4X MSAA enabled
	UINT      g_4xMsaaQuality = 0;		// quality level of 4X MSAA
}

void Core::RunApplication(IGameApp& app, const wchar_t* className)
{
	g_hAppInst = GetModuleHandle(0);
	g_AppName = className;

	g_Core = GameCore::GetApp();
	g_GameTimer = GameTimer::GetApp();

#ifdef DEBUG_SERVER
	g_netcore = NetCore::GetApp();
	g_service = Service::GetApp();

	std::thread io_thread(&Service::ActiveService, g_service);
#endif

	MSG msg = {};

	g_Core->InitializeCore(app);
	g_GameTimer->Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!g_Core->UpdateCore(app))
				break;
		}
	}

	TerminateApplication(app);

#ifdef DEBUG_SERVER
	io_thread.join();
#endif
}

void Core::TerminateApplication(IGameApp& game)
{
#ifdef DEBUG_SERVER
	g_service->Clear();
	g_netcore->Destory();
#endif
	game.Cleanup();
	g_Core->ShutdownCore();

	// Context Release
	GameCore::DestroyApp();
	GameTimer::DestroyApp();
#ifdef DEBUG_SERVER
	NetCore::DestroyApp();
	Service::DestroyApp();
#endif
}

void Core::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((g_GameTimer->GetTotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		wstring fpsStr = to_wstring(fps);
		wstring mspfStr = to_wstring(mspf);

		wstring windowText = static_cast<wstring>(g_AppName) +
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr;

		SetWindowText(g_hMainWnd, windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return GameCore::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

void GameCore::InitializeCore(IGameApp& game)
{
	// Create Renderer
	m_GraphicsContext = GraphicsContext::GetApp();
	m_GraphicsRenderer = GraphicsRenderer::GetApp();

	InitMainWindow();
	InitDirect3D();
	OnResize();

	// FullScreenMode
#ifdef FULLSCREEN_MODE
	ToggleFullscreenWindow();
#endif // FULLSCREEN_MODE

	ThrowIfFailed(g_CommandList->Reset(g_DirectCmdListAlloc.Get(), nullptr));

	game.Startup();
	m_GraphicsContext->Initialize();
	m_GraphicsRenderer->Initialize();

	// Execute the initialization commands.
	ThrowIfFailed(g_CommandList->Close());
	ID3D12CommandList* cmdsLists[] = { g_CommandList.Get() };
	g_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();
}

bool GameCore::UpdateCore(IGameApp& game)
{
	g_GameTimer->Tick(60.f);

	if (!mAppPaused)
	{
		CalculateFrameStats();

		const float deltaT = g_GameTimer->GetTimeElapsed();

		game.Update(deltaT);

		//PreparePresent();

		/////////////////////////////////////////////////////////////////////////////////////////
		ThrowIfFailed(g_DirectCmdListAlloc->Reset());
		ThrowIfFailed(g_CommandList->Reset(g_DirectCmdListAlloc.Get(), Graphics::g_OpaquePSO.Get()));

		m_GraphicsRenderer->SetGraphicsDescriptorHeap();

		g_CommandList->RSSetViewports(1, &mScreenViewport);
		g_CommandList->RSSetScissorRects(1, &mScissorRect);

		// Indicate a state transition on the resource usage.
		g_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// Clear the back buffer and depth buffer.
		g_CommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::Aquamarine, 0, nullptr);
		g_CommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		g_CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		m_GraphicsRenderer->RenderGraphics();
		game.RenderScene();

		g_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		// Done recording commands.
		ThrowIfFailed(g_CommandList->Close());

		// Add the command list to the queue for execution.
		ID3D12CommandList* cmdsLists[] = { g_CommandList.Get() };
		g_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// D3D11On12 Device Rendering
		D3D11DevicePreparePresent();
		D3D11DeviceExecuteCommandList();

		// swap the back and front buffers
		MoveToNextFrame();

		FlushCommandQueue();
	}
	else
	{
#ifdef ON_PAUSE
		Sleep(100);
#endif
	}

	return !game.IsDone();
}

void GameCore::ShutdownCore()
{
	mSwapChain->SetFullscreenState(FALSE, nullptr);

	m_GraphicsContext->Release();
	m_GraphicsRenderer->Shutdown();

	if (g_Device != nullptr)
		FlushCommandQueue();

	GraphicsContext::DestroyApp();
	GraphicsRenderer::DestroyApp();
}

LRESULT GameCore::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WCHAR tc[2] = {};
	int len = 0;

	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
#ifdef ON_PAUSE
			mAppPaused = true;
			g_GameTimer->Stop();
#endif
		}
		else
		{
			mAppPaused = false;
			g_GameTimer->Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		g_DisplayWidth = LOWORD(lParam);
		g_DisplayHeight = HIWORD(lParam);
		if (g_Device)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
				//ToggleFullscreenWindow();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
#ifdef ON_PAUSE
		mAppPaused = true;
		mResizing = true;
		g_GameTimer->Stop();
#endif
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		g_GameTimer->Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_LBUTTONDOWN:
		InputHandler::OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONDOWN:
		InputHandler::OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
		InputHandler::OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONUP:
		InputHandler::OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		InputHandler::OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_KEYDOWN:
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void GameCore::PreparePresent()
{
	ThrowIfFailed(g_DirectCmdListAlloc->Reset());
	ThrowIfFailed(g_CommandList->Reset(g_DirectCmdListAlloc.Get(), Graphics::g_OpaquePSO.Get()));

	g_CommandList->RSSetViewports(1, &mScreenViewport);
	g_CommandList->RSSetScissorRects(1, &mScissorRect);

	// Indicate a state transition on the resource usage.
	g_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the back buffer and depth buffer.
	g_CommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::Black, 0, nullptr);
	g_CommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	g_CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
}

void GameCore::MoveToNextFrame()
{
	// swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
}

void GameCore::D3D11DevicePreparePresent()
{
	// Acquire our wrapped render target resource for the current back buffer.
	m_D3d11On12Device->AcquireWrappedResources(m_WrappedBackBuffers[mCurrBackBuffer].GetAddressOf(), 1);

	// Render text directly to the back buffer.
	g_D2dDeviceContext->SetTarget(m_D2dRenderTargets[mCurrBackBuffer].Get());
	g_D2dDeviceContext->BeginDraw();
}

void GameCore::D3D11DeviceExecuteCommandList()
{
	ThrowIfFailed(g_D2dDeviceContext->EndDraw());

	// Release our wrapped render target resource. Releasing 
	// transitions the back buffer resource to the state specified
	// as the OutState when the wrapped resource was created.
	m_D3d11On12Device->ReleaseWrappedResources(m_WrappedBackBuffers[mCurrBackBuffer].GetAddressOf(), 1);

	// Flush to submit the 11 command list to the shared command queue.
	m_D3d11DeviceContext->Flush();
}

void GameCore::InitMainWindow()
{
	WNDCLASS wc;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = g_hAppInst;
	wc.hIcon = LoadIcon(g_hAppInst, MAKEINTRESOURCE(107));
	wc.hCursor = LoadIcon(g_hAppInst, MAKEINTRESOURCE(129));	
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = g_AppName;

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return;
	}

#ifdef FULLSCREEN_MODE
	g_DisplayWidth = GetSystemMetrics(SM_CXSCREEN);
	g_DisplayHeight = GetSystemMetrics(SM_CYSCREEN);

	g_hMainWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		g_AppName, g_AppName,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, 0, g_hAppInst, 0);
#else
	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, g_DisplayWidth, g_DisplayHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	g_hMainWnd = CreateWindow(
		g_AppName, g_AppName,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, g_hAppInst, 0);
#endif // FULLSCREEN_MODE

	if (!g_hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return;
	}

#ifdef FULLSCREEN_MODE
	ShowWindow(g_hMainWnd, SW_MAXIMIZE);
#else
	ShowWindow(g_hMainWnd, SW_SHOW);
#endif // FULLSCREEN_MODE

	ShowWindow(g_hMainWnd, SW_SHOW);
	UpdateWindow(g_hMainWnd);

	//InputHandler::ClipCursorToScreen();
}

void GameCore::ToggleFullscreenWindow(IDXGISwapChain* pSwapChain)
{
	if (mFullscreenState)
	{
		//SetWindowLong(g_hMainWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

		//SetWindowPos(
		//	g_hMainWnd,
		//	HWND_NOTOPMOST,
		//	m_windowRect.left,
		//	m_windowRect.top,
		//	m_windowRect.right - m_windowRect.left,
		//	m_windowRect.bottom - m_windowRect.top,
		//	SWP_FRAMECHANGED | SWP_NOACTIVATE);

		//ShowWindow(g_hMainWnd, SW_NORMAL);
	}
	else
	{
		// Save the old window rect so we can restore it when exiting fullscreen mode.
		// GetWindowRect(m_hwnd, &m_windowRect);

		// Make the window borderless so that the client area can fill the screen.
		SetWindowLong(g_hMainWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

		RECT fullscreenWindowRect;

		if (pSwapChain)
		{
			// Get the settings of the display on which the app's window is currently displayed
			ComPtr<IDXGIOutput> pOutput;
			ThrowIfFailed(pSwapChain->GetContainingOutput(&pOutput));
			DXGI_OUTPUT_DESC Desc;
			ThrowIfFailed(pOutput->GetDesc(&Desc));
			fullscreenWindowRect = Desc.DesktopCoordinates;
		}
		else
		{
			// Get the settings of the primary display
			DEVMODE devMode = {};
			devMode.dmSize = sizeof(DEVMODE);
			EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

			// Set global param
			g_DisplayWidth = devMode.dmPelsWidth;
			g_DisplayHeight = devMode.dmPelsHeight;

			fullscreenWindowRect = {
				devMode.dmPosition.x,
				devMode.dmPosition.y,
				devMode.dmPosition.x + static_cast<LONG>(devMode.dmPelsWidth),
				devMode.dmPosition.y + static_cast<LONG>(devMode.dmPelsHeight)
			};

			SetWindowPos(
				g_hMainWnd,
				HWND_TOPMOST,
				fullscreenWindowRect.left,
				fullscreenWindowRect.top,
				fullscreenWindowRect.right,
				fullscreenWindowRect.bottom,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);


			ShowWindow(g_hMainWnd, SW_MAXIMIZE);
		}
	}

	mFullscreenState = !mFullscreenState;
}

void GameCore::InitDirect3D()
{
	UINT dxgiFactoryFlags = 0;
	UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();

		// Enable additional debug layers.
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	}
#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

	std::size_t ui64VideoMemory;
	Microsoft::WRL::ComPtr<IDXGIAdapter> pAdapter;
	DXGI_ADAPTER_DESC adapterDesc;

	// 그래픽 카드 어뎁터 체크 
	// 요청한 그래픽 카드 인터페이스에 대한 어뎁터가 없습니다.
	ThrowIfFailed(mdxgiFactory->EnumAdapters(0, (IDXGIAdapter**)&pAdapter));

	pAdapter->GetDesc(&adapterDesc);
	ui64VideoMemory = (std::size_t)(adapterDesc.DedicatedVideoMemory + adapterDesc.SharedSystemMemory);

	// Compare Video Memory and Find better Gpu
	int gpu_idx = 0;
	int select = 0;
	std::size_t comparison_videoMemory;

	while (mdxgiFactory->EnumAdapters(gpu_idx, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		pAdapter->GetDesc(&adapterDesc);
		comparison_videoMemory = adapterDesc.DedicatedVideoMemory + adapterDesc.SharedSystemMemory;

		if (comparison_videoMemory > ui64VideoMemory)
		{
			ui64VideoMemory = comparison_videoMemory;
			select = gpu_idx;
		}
		++gpu_idx;
	}

	mdxgiFactory->EnumAdapters(select, &pAdapter);

	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		pAdapter.Get(),             // the best adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&g_Device));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&g_Device)));
	}

	ThrowIfFailed(g_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&mFence)));

	mRtvDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = g_BackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(g_Device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	g_4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(g_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
	LogAdapters();
#endif

	CreateCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

	CreateID3D11On12Device(dxgiFactoryFlags, d3d11DeviceFlags, d2dFactoryOptions);
}

void GameCore::OnResize()
{
	assert(g_Device);
	assert(mSwapChain);
	assert(g_DirectCmdListAlloc);

	// Flush before changing any resources.
	FlushCommandQueue();

	ThrowIfFailed(g_CommandList->Reset(g_DirectCmdListAlloc.Get(), nullptr));

	// Release the previous resources we will be recreating.
	for (int i = 0; i < SwapChainBufferCount; ++i) {
		mSwapChainBuffer[i].Reset();
		m_WrappedBackBuffers[i].Reset();
		m_D2dRenderTargets[i].Reset();
	}
	mDepthStencilBuffer.Reset();

	// DirectX11은 리소스의 '지연된 파괴'를 하기떄문에 일반적으로 파기해야하는 경우
	// 리소스를 렌더타겟에서 완전히 바인딩 해제하고 Flush해야한다.
	g_D2dDeviceContext->SetTarget(nullptr);
	m_D3d11DeviceContext->Flush();

	// Resize the swap chain.
	ThrowIfFailed(mSwapChain->ResizeBuffers(
		SwapChainBufferCount,
		g_DisplayWidth, g_DisplayHeight,
		g_BackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	mCurrBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		g_Device->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);

		// Create a wrapped 11On12 resource of this back buffer.
		D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
		ThrowIfFailed(m_D3d11On12Device->CreateWrappedResource(
			mSwapChainBuffer[i].Get(),
			&d3d11Flags,
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			IID_PPV_ARGS(&m_WrappedBackBuffers[i])
		));

		// Create a render target for D2D to draw directly to this back buffer.
		ComPtr<IDXGISurface> surface;
		ThrowIfFailed(m_WrappedBackBuffers[i].As(&surface));
		ThrowIfFailed(g_D2dDeviceContext->CreateBitmapFromDxgiSurface(
			surface.Get(),
			&m_BitmapProperties,
			&m_D2dRenderTargets[i]
		));

		rtvHeapHandle.Offset(1, mRtvDescriptorSize);

		ThrowIfFailed(g_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_DirectCmdListAlloc)));
	}

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = g_DisplayWidth;
	depthStencilDesc.Height = g_DisplayHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = g_4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = g_4xMsaaState ? (g_4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = g_DepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(g_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = g_DepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	g_Device->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer.
	g_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	// Execute the resize commands.
	ThrowIfFailed(g_CommandList->Close());
	ID3D12CommandList* cmdsLists[] = { g_CommandList.Get() };
	g_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	FlushCommandQueue();

	// Update the viewport transform to cover the client area.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(g_DisplayWidth);
	mScreenViewport.Height = static_cast<float>(g_DisplayHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0, 0, g_DisplayWidth, g_DisplayHeight };

	// Set Lens
	if (TOY_GROUND::GetApp())
	{
		TOY_GROUND::GetApp()->OnResize();
	}

	//InputHandler::ClipCursorToScreen();
}

void GameCore::CreateID3D11On12Device(UINT dxgiFactoryFlags, UINT d3d11DeviceFlags, D2D1_FACTORY_OPTIONS d2dFactoryOptions)
{
	// Create an 11 device wrapped around the 12 device and share
// 12's command queue.
	Microsoft::WRL::ComPtr<ID3D11Device> d3d11Device;
	ThrowIfFailed(D3D11On12CreateDevice(
		g_Device.Get(),
		d3d11DeviceFlags,
		nullptr,
		0,
		reinterpret_cast<IUnknown**>(g_CommandQueue.GetAddressOf()),
		1,
		0,
		&d3d11Device,
		&m_D3d11DeviceContext,
		nullptr
	));

	ThrowIfFailed(d3d11Device.As(&m_D3d11On12Device));

	// Create D2D Factory
	// Create D2D/DWrite components.
	D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
	ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &m_D2dFactory));
	ComPtr<IDXGIDevice> dxgiDevice;
	ThrowIfFailed(m_D3d11On12Device.As(&dxgiDevice));
	ThrowIfFailed(m_D2dFactory->CreateDevice(dxgiDevice.Get(), &m_D2dDevice));
	ThrowIfFailed(m_D2dDevice->CreateDeviceContext(deviceOptions, &g_D2dDeviceContext));
	ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &g_DWriteFactory));

	// Query the desktop's dpi settings, which will be used to create
	// D2D's render targets.
	float dpiX;
	float dpiY;
	dpiX = GetDpiForWindow(Core::g_hMainWnd);
	dpiY = GetDpiForWindow(Core::g_hMainWnd);
	m_BitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpiX,
		dpiY
	);
}

void GameCore::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(g_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_CommandQueue)));

	ThrowIfFailed(g_Device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(g_DirectCmdListAlloc.GetAddressOf())));

	ThrowIfFailed(g_Device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		g_DirectCmdListAlloc.Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(g_CommandList.GetAddressOf())));

	// 닫힌 상태로 시작한. 이후 명령 리스트를 처음 참조할 때 
	// Reset을 호출하는데, Reset을 호출하려면 명령 리스트가
	// 닫혀 있어야하기 때문이다.
	g_CommandList->Close();
}

void GameCore::CreateSwapChain()
{
	// Release the previous swapchain we will be recreating.
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = g_DisplayWidth;
	sd.BufferDesc.Height = g_DisplayHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = g_BackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = g_4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = g_4xMsaaState ? (g_4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = g_hMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
	ThrowIfFailed(mdxgiFactory->CreateSwapChain(
		g_CommandQueue.Get(),
		&sd,
		mSwapChain.GetAddressOf()));
}

void GameCore::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(g_Device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 2;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(g_Device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

void GameCore::FlushCommandQueue()
{
	// Advance the fence value to mark commands up to this fence point.
	mCurrentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(g_CommandQueue->Signal(mFence.Get(), mCurrentFence));

	// Wait until the GPU has completed commands up to this fence point.
	if (mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, (LPCWSTR)false, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

ID3D12Resource* GameCore::CurrentBackBuffer() const
{
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE GameCore::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrBackBuffer,
		mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE GameCore::DepthStencilView() const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void GameCore::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		adapterList.push_back(adapter);

		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		LogAdapterOutputs(adapterList[i]);
		ReleaseCom(adapterList[i]);
	}
}

void GameCore::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, g_BackBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void GameCore::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	// Call with nullptr to get list count.
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}
}

bool GameCore::IsPause() const
{
	return mAppPaused;
}
