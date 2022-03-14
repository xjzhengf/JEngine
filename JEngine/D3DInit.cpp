#include "stdafx.h"
#include "D3DInit.h"
#include "Engine.h"
using Microsoft::WRL::ComPtr;

using namespace DirectX;


D3DInit* D3DInit::mApp = nullptr;
D3DInit::D3DInit()
{
	assert(mApp == nullptr);
	mApp = this;
	SetClientHeight(Engine::GetEngine()->GetWindow()->GetClientHeight());
	SetClientWidht(Engine::GetEngine()->GetWindow()->GetClientWidht());
	SetWindow(Engine::GetEngine()->GetWindow()->GetHWnd());
}

D3DInit::~D3DInit()
{
	if (md3dDevice != nullptr) {
		FlushCommandQueue();
	}
}

D3DInit* D3DInit::GetApp()
{
	return mApp;
}


float D3DInit::AspectRatio() const
{
	return static_cast<float>(mClientWidht)/mClientHeight;
}

bool D3DInit::Get4xMsaaState() const
{
	return m4xMsaaState;
}

bool D3DInit::GetAppPause() const
{
	return mAppPause;
}

bool D3DInit::IsHaveDevice() const
{
	if (md3dDevice) {
		return true;
	}
	return false;
}

void D3DInit::Set4xMsaaState(bool value)
{
	if (m4xMsaaState != value) {
		m4xMsaaState = value;

		CreateSpawChain();
		OnResize();
	}
}

void D3DInit::SetWindow(HWND mhMainWnd)
{
	this->mhMainWnd = mhMainWnd;
}

void D3DInit::SetClientWidht(int Width)
{
	this->mClientWidht = Width;
}

void D3DInit::SetClientHeight(int Height)
{
	this->mClientHeight = Height;
}





bool D3DInit::Initialize()
{

	if (!InitDirect3D()) {
		return false;
	}

	OnResize();
	return true;
}


void D3DInit::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.NodeMask = 0;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = 1;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

void D3DInit::OnResize()
{
	assert(md3dDevice);
	assert(mSwapChain);
	assert(mDirectCmdListAlloc);
	//更改之前刷新
	FlushCommandQueue();

	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	//释放之前的资源 重新创建
	for (int i = 0; i < SwapChainBufferCount; i++)
	{
		mSwapChainBuffer[i].Reset();
	}
		mDepthStencilBuffer.Reset();

		//调整交换链
		ThrowIfFailed(mSwapChain->ResizeBuffers(SwapChainBufferCount, mClientWidht, mClientHeight, mBackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

		mCurrBackBuffer = 0;

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < SwapChainBufferCount; i++) {
			ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
			//Device创建RenderTargetView
			md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, mRtvDescriptorSize);
		}
	
		//创建深度缓冲区结构体
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; 
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = mClientWidht;
		depthStencilDesc.Height = mClientHeight;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;

		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = mDepthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f; 
		optClear.DepthStencil.Stencil = 0;
		//Device创建CommittedResource
		ThrowIfFailed(md3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), 
			D3D12_HEAP_FLAG_NONE, 
			&depthStencilDesc, 
			D3D12_RESOURCE_STATE_COMMON,
			&optClear, 
			IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = mDepthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;
		//Device创建DepthStencilView
		md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());
		//将资源从初始状态转换为用作深度缓冲区。
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));


		ThrowIfFailed(mCommandList->Close());
		ID3D12CommandList* mCmdsLists[] = { mCommandList.Get() };

		mCommandQueue->ExecuteCommandLists(_countof(mCmdsLists), mCmdsLists);
		FlushCommandQueue();

		mScreenViewport.TopLeftX = 0;
		mScreenViewport.TopLeftY = 0;
		mScreenViewport.Width = static_cast<float>(mClientWidht);
		mScreenViewport.Height = static_cast<float>(mClientHeight);
		mScreenViewport.MaxDepth = 1.0f;
		mScreenViewport.MinDepth = 0.0f;

		mScissorRect = { 0,0,mClientWidht,mClientHeight };
}


bool D3DInit::InitDirect3D()
{
#if defined(DEBUG) || defined(_DEBUG)
	{
		//开启D3D12 DEBUG
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

	HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&md3dDevice));

	if (FAILED(hardwareResult)) {
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&md3dDevice)));
	}
		ThrowIfFailed(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));

		mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = mBackBufferFormat;
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		ThrowIfFailed(md3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));
		m4xMsaaQuality = msQualityLevels.NumQualityLevels;
		assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
		LogAdapters();
#endif
		CreateCommandObjects();
		CreateSpawChain();
		CreateRtvAndDsvDescriptorHeaps();
	
	return true;
}

void D3DInit::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));
	ThrowIfFailed(md3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));
	ThrowIfFailed(md3dDevice->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,mDirectCmdListAlloc.Get(),nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())));
	mCommandList->Close();
}

void D3DInit::CreateSpawChain()
{
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidht;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = mhMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(mdxgiFactory->CreateSwapChain(mCommandQueue.Get(), &sd, mSwapChain.GetAddressOf()));

}

void D3DInit::FlushCommandQueue()
{
	mCurrentFence++;
	//设置一个栅栏点，如果gpu上任务未完成则栏点设置失败
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));
	//等待直到GPU完成这个栅栏点的命令。
	if (mFence->GetCompletedValue() < mCurrentFence) {
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	
	}
    
}

ID3D12Resource* D3DInit::CurrentBackBuffer() const
{
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DInit::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(),mCurrBackBuffer,mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DInit::DepthStencilView() const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}


void D3DInit::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (mdxgiFactory->EnumAdapters(i,&adapter)!= DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter";
		text += desc.Description;
		text += L"\n";
		OutputDebugString(text.c_str());

		adapterList.push_back(adapter);
		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i) {
		LogAdapterOutputs(adapterList[i]);
		ReleaseCom(adapterList[i]);
	}
}

void D3DInit::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i,&output)!=DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);
		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayerModes(output, mBackBufferFormat);

		ReleaseCom(output);
		++i;
	}
}

void D3DInit::LogOutputDisplayerModes(IDXGIOutput* output, DXGI_FORMAT format)
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
