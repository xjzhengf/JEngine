#pragma once
#include "stdafx.h"
#if defined(DEBUG)||defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC

#endif
#include "D3DUtil.h"
#include "GameTimer.h"
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")


class D3DInit {
protected:
	D3DInit();
	D3DInit(const D3DInit& d3d) = delete;
	D3DInit& operator=(const D3DInit& d3d) = delete;
	virtual ~D3DInit();
public:
	static D3DInit* GetApp();
	float AspectRatio() const;
	bool Get4xMsaaState() const;
	bool GetAppPause()const;
	bool IsHaveDevice() const;
	void Set4xMsaaState(bool value);
	void SetWindow(HWND mhMainWnd);
	void SetClientWidht(int Width);
	void SetClientHeight(int Height);
	virtual bool Initialize();
	virtual void DrawPrepare(){};
protected:
	virtual void CreateRtvAndDsvDescriptorHeaps();
public:
	virtual void OnResize();
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Draw(const GameTimer& gt) = 0;

	//virtual void OnMouseDown(WPARAM btnState,int x ,int y){}
	//virtual void OnMouseUp(WPARAM btnState, int x, int y){}
	//virtual void OnMouseMove(WPARAM btnState, int x, int y){}
protected:
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSpawChain();
	void FlushCommandQueue();

	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayerModes(IDXGIOutput* output, DXGI_FORMAT format);
protected:
	static D3DInit* mApp;

	HWND mhMainWnd = nullptr;
	bool mAppPause = false;         //是否暂停
	bool mMinimized = false;        //最小化
	bool mMaximized = false;        //最大化
	bool mResizing = false;         //调整大小是否被拖动
	bool mFullscreenState = false;  //开启全屏

	bool m4xMsaaState = false;
	UINT m4xMsaaQuality = 0;

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;

	UINT64 mCurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr <ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>mDsvHeap;

	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	std::wstring mMainWndCaption = L"D3D App";
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int mClientWidht = 800;
	int mClientHeight = 600;


public:


	bool isUpdateDraw = false;
};
