#pragma once
#include "stdafx.h"

#if defined(DEBUG)||defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC

#endif
#include "FDynamicRHI.h"
#include "UploadBuffer.h"
#include "Buffer.h"
#include "FDirectionalLightProperty.h"
#include "Material.h"
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;


struct ObjectConstants {
	glm::mat4x4 WorldViewProj = glm::identity<glm::mat4x4>();
	glm::mat4x4 LightViewProj = glm::identity<glm::mat4x4>();
	glm::mat4x4 ViewProj = glm::identity<glm::mat4x4>();
	glm::mat4x4 World = glm::identity<glm::mat4x4>();
	glm::mat4x4 Rotation = glm::identity<glm::mat4x4>();
	glm::mat4x4 Scale = glm::identity<glm::mat4x4>();
	glm::mat4x4 Translate = glm::identity<glm::mat4x4>();
	glm::mat4x4 TexTransform = glm::identity<glm::mat4x4>();
	FDirectionalLight directionalLight;
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
	 Microsoft::WRL::ComPtr<ID3D12Device> GetDevice();

	void SetWindow(HWND mhMainWnd);
	void SetClientWidht(int Width);
	void SetClientHeight(int Height);
	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
private:
	void BulidDescriptorHeaps(const std::string& Name);
	void BulidConstantBuffers(const std::string& Name);
	void BuildMaterial(const std::string& Name, FRenderResource* RenderResource);
	void BuildShaderResourceView(const std::string& ActorName, const std::string& Name, FRenderResource* RenderResource);
	void BulidRootSignature(FRHIResource* resource);
	void BuildPSO(FRHIResource* RHIResource,const std::string& PSOName) override;
public:
	//override RHI
	virtual void RSSetViewports(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth) override;
	virtual void ResetCommand(const std::string& PSOName) override;
	virtual void RSSetScissorRects(long left, long top, long right, long bottom) override;
	virtual void ResourceBarrier(unsigned int NumberBarrier, ID3D12Resource* Resource, int stateBefore, int stateAfter) override;
	virtual void ClearRenderTargetView(unsigned __int64 ptr) override;
	virtual void ClearDepthStencilView(unsigned __int64 ptr) override;
	virtual void OMSetStencilRef(int StencilRef) override;
	virtual void OMSetRenderTargets(int numTatgetDescriptors, unsigned __int64 RTptr, bool RTsSingleHandleToDescriptorRange, unsigned __int64 DSptr)override;
	virtual void SetDescriptorHeaps(std::string Name) override;
	virtual void SetGraphicsRootSignature() override;
	virtual void IASetVertexBuffers(Buffer* buffer) override;
	virtual void IASetIndexBuffer(Buffer* buffer) override;
	virtual void IASetPrimitiveTopology() override;
	virtual void Offset(std::string Name) override;
	virtual void SetGraphicsRootDescriptorTable(std::string Name) override;
	virtual void SetGraphicsRoot32BitConstants() override;
	virtual void SetPipelineState(const std::string& Name) override;
	virtual void DrawIndexedInstanced(std::string Name) override;
	virtual void LoadTexture(FTexture* TextureResource) override;
	virtual void ExecuteCommandLists() override;
	virtual void UpdateMVP(const GameTimer& gt) override;
	virtual void UpdateLight(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt) override;
	virtual void DrawPrepare(FRHIResource* resource, FRenderResource* renderResource) override;

public:
	virtual Buffer* CreateBuffer(FRenderResource* renderResource) override;
	virtual void CreateShader(FRHIResource* RHIResource, const std::wstring& filename) override;
protected:
	HWND mhMainWnd = nullptr;

	bool mMinimized = false;        //最小化
	bool mMaximized = false;        //最大化
	bool mResizing = false;         //调整大小是否被拖动
	bool mFullscreenState = false;  //开启全屏

	static DX12RHI* mDX12RHI;

	glm::vec3 cameraLoc;
private:
	std::map<std::string, ComPtr<ID3D12DescriptorHeap>> mCbvSrvHeap ;
	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
	std::map<std::string,std::unique_ptr<UploadBuffer<ObjectConstants>>> mObjectCB ;
	std::map<std::string,std::unique_ptr<UploadBuffer<FMaterial>>> mMaterialCB ;
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

	std::unordered_map<std::string,ComPtr<ID3D12PipelineState>> mPSO ;

	glm::mat4x4 mWorld = glm::identity<glm::mat4x4>();

	POINT mLastMousePos;

	/*std::vector<MeshData> meshDataVector;*/

	float Time;


protected:
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSpawChain();
	void FlushCommandQueue();
	void CreateRtvAndDsvDescriptorHeaps();

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
	Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap;

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