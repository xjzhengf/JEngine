#pragma once
#include "stdafx.h"

#if defined(DEBUG)||defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC

#endif
#include "FDynamicRHI.h"
#include "UploadBuffer.h"
#include "Buffer.h"
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;


struct ObjectConstants {
	//XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	glm::mat4x4 WorldViewProj = glm::identity<glm::mat4x4>();
	glm::mat4x4 Rotation = glm::identity<glm::mat4x4>();
	glm::mat4x4 Scale = glm::identity<glm::mat4x4>();
	glm::mat4x4 Translate = glm::identity<glm::mat4x4>();
	glm::mat4x4 TexTransform = glm::identity<glm::mat4x4>();
	float Time = 0.0f;
};

class DX12RHI  :public FDynamicRHI {
public:
	DX12RHI();
	DX12RHI(const DX12RHI& sm) = delete;
	DX12RHI& operator=(const DX12RHI& sm) = delete;
	virtual ~DX12RHI();
	float AspectRatio() const;
	bool Get4xMsaaState() const;
	bool IsHaveDevice() const;
	void Set4xMsaaState(bool value);

	static DX12RHI* GetDX12RHI();
	 virtual bool Initialize() override ;
	 void OnResize() ;

	void SetWindow(HWND mhMainWnd);
	void SetClientWidht(int Width);
	void SetClientHeight(int Height);
	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
private:
	void BulidDescriptorHeaps(int index);
	void BulidConstantBuffers(int index);
	void BuildShaderResourceView(int index, const std::string& Name);
	void BulidRootSignature();
	void BulidShadersAndInputLayout();
	void BuildPSO(FRHIResource* RHIResource) override;
public:
	//override RHI
	virtual void RSSetViewports(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth) override;
	virtual void ResetCommand() override;
	virtual void RSSetScissorRects(long left, long top, long right, long bottom) override;
	virtual void ResourceBarrier(unsigned int NumberBarrier, int stateBefore, int stateAfter) override;
	virtual void ClearRenderTargetView() override;
	virtual void ClearDepthStencilView() override;
	virtual void OMSetStencilRef(int StencilRef) override;
	virtual void OMSetRenderTargets(int numTatgetDescriptors, bool RTsSingleHandleToDescriptorRange)override;
	virtual void SetDescriptorHeaps(int index) override;
	virtual void SetGraphicsRootSignature() override;
	virtual void IASetVertexBuffers(Buffer* buffer) override;
	virtual void IASetIndexBuffer(Buffer* buffer) override;
	virtual void IASetPrimitiveTopology() override;
	virtual void Offset(int index) override;
	virtual void SetGraphicsRootDescriptorTable(int index) override;
	virtual void SetGraphicsRoot32BitConstants() override;
	virtual void DrawIndexedInstanced(int index) override;
	virtual void LoadTexture(FTexture* TextureResource) override;
	virtual void ExecuteCommandLists() override;
	virtual void UpdateMVP(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;
	virtual void DrawPrepare() override;

public:
	virtual Buffer* CreateBuffer(FRenderResource* renderResource) override;
protected:
	HWND mhMainWnd = nullptr;

	bool mMinimized = false;        //最小化
	bool mMaximized = false;        //最大化
	bool mResizing = false;         //调整大小是否被拖动
	bool mFullscreenState = false;  //开启全屏

	static DX12RHI* mDX12RHI;

	glm::vec3 cameraLoc;
private:

	std::vector < ComPtr<ID3D12DescriptorHeap>> mCbvSrvHeap ;
	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
	std::vector<std::unique_ptr<UploadBuffer<ObjectConstants>>> mObjectCB ;
	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;

	std::unique_ptr<DXBuffer> mGeo = nullptr;

	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;


	ComPtr<ID3DBlob> mvsByteCode = nullptr;
	ComPtr<ID3DBlob> mpsByteCode = nullptr;
	ComPtr<ID3D12RootSignature> mRootSigmature = nullptr;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	bool m4xMsaaState = false;
	UINT m4xMsaaQuality = 0;

	ComPtr<ID3D12PipelineState> mPSO = nullptr;

	glm::mat4x4 mWorld = glm::identity<glm::mat4x4>();

	POINT mLastMousePos;

	/*std::vector<MeshData> meshDataVector;*/

	float Time;

	bool IsRunDrawPrepare = true;
protected:
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSpawChain();
	void FlushCommandQueue();
	void CreateRtvAndDsvDescriptorHeaps();


	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayerModes(IDXGIOutput* output, DXGI_FORMAT format);

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


	int mClientWidht = 800;
	int mClientHeight = 600;
};