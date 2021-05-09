#pragma once
#include "Singleton.h"

class IGameApp;
class GraphicsRenderer;
class GraphicsContext;

namespace Core
{
	void RunApplication(IGameApp& app, const wchar_t* className);
	void TerminateApplication(IGameApp& game);

	void CalculateFrameStats();
}

namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_OpaquePSO;
}

#define CREATE_APPLICATION( app_class ) \
    IGameApp* app = app_class::GetApp(); \
    Core::RunApplication( *app, L#app_class ); \
    app_class::DestroyApp(); \
    return 0; \

class GameCore : public TemplateSingleton<GameCore>
{
	friend class GraphicsRenderer;
	GraphicsRenderer* m_GraphicsRenderer;
	GraphicsContext* m_GraphicsContext;

public:
	void InitializeCore(IGameApp& game);
	bool UpdateCore(IGameApp& game);
	void ShutdownCore();

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void PreparePresent();
	void MoveToNextFrame();

	void D3D11DevicePreparePresent();
	void D3D11DeviceExecuteCommandList();

private:
	void InitMainWindow();
	void ToggleFullscreenWindow(IDXGISwapChain* pSwapChain = nullptr);

	void InitDirect3D();
	void OnResize();

	void CreateID3D11On12Device(UINT dxgiFactoryFlags, UINT d3d11DeviceFlags, D2D1_FACTORY_OPTIONS d2dFactoryOptions);
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();

	void FlushCommandQueue();

	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

public:
	bool IsPause() const;

private:
	bool      mAppPaused = false;  // is the application paused?
	bool      mMinimized = false;  // is the application minimized?
	bool      mMaximized = false;  // is the application maximized?
	bool      mResizing = false;   // are the resize bars being dragged?
	bool      mFullscreenState = false;// fullscreen enabled

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;

	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	UINT64 mCurrentFence = 0;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

private:
	//D3D11On12
	Microsoft::WRL::ComPtr<ID3D11On12Device> m_D3d11On12Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_D3d11DeviceContext;

	Microsoft::WRL::ComPtr<ID3D11Resource> m_WrappedBackBuffers[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_D2dRenderTargets[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID2D1Factory3> m_D2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device2> m_D2dDevice;
	D2D1_BITMAP_PROPERTIES1 m_BitmapProperties;

private:
	POINT										mLastMousePos;
};

