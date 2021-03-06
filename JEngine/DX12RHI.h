#pragma once
#include "stdafx.h"

#if defined(DEBUG)||defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC

#endif
#include "FDynamicRHI.h"
#include "UploadBuffer.h"
#include "Buffer.h"
#include "FDirectionalLightProperty.h"
#include "ShaderManager.h"
#include "Material.h"
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

using namespace DirectX::PackedVector;


struct ObjectConstants {
	glm::mat4x4 TLightViewProj = glm::identity<glm::mat4x4>();
	glm::mat4x4 LightViewProj = glm::identity<glm::mat4x4>();
	glm::mat4x4 ViewProj = glm::identity<glm::mat4x4>();
	glm::mat4x4 World = glm::identity<glm::mat4x4>();
	glm::mat4x4 Rotation = glm::identity<glm::mat4x4>();
	glm::mat4x4 TexTransform = glm::identity<glm::mat4x4>();
	float Time = 0.0f;
	FDirectionalLight directionalLight;
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
	void BulidDescriptorHeaps();
	void BulidConstantBuffers(const std::string& Name, RenderItem* renderItem);
	void BuildMaterial(const std::string& Name, FRenderResource* RenderResource);
	void BuildShaderResourceView(const std::string& ActorName, RenderItem* renderItem, const std::string& Name, FRenderResource* RenderResource, FRenderResource* HDRResource, std::shared_ptr<FRenderScene> renderScene);
	void BulidRootSignature(FShader* shader);
	void BuildPSO(std::shared_ptr<RenderItem> renderItem, FMaterial Mat) override;

public:
	void IASetVertexAndIndexBuffers(Buffer* buffer);
	void IASetPrimitiveTopology();
	void SetGraphicsRootDescriptorTable(RenderItem* renderItem, bool isDepth, bool isNeedRTV, int RTVNumber,int width,int height);
	void DrawIndexedInstanced(std::shared_ptr<RenderItem> renderItem, const std::string& Name);
	void SetGraphicsRoot32BitConstants(int width, int height);
	void ClearRenderTargetView(unsigned __int64 ptr);
	void ClearDepthStencilView(unsigned __int64 ptr);
	void OMSetStencilRef(int StencilRef);
	void OMSetRenderTargets(int numTatgetDescriptors, unsigned __int64 RTptr, bool RTsSingleHandleToDescriptorRange, unsigned __int64 DSptr);
	void SetDescriptorHeaps();
	void SetGraphicsRootSignature();

	void BuildRenderItem(std::shared_ptr<FRenderScene> sceneResource, const std::string& MatName);
public:
	//override RHI
	virtual void UpdateCB(std::shared_ptr<FRenderScene> sceneResource, RenderItem* renderItem, const std::string& Name, int CBIndex, FMaterial Mat) override;
	virtual void RSSetViewports(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth) override;
	virtual void ResetCommand(const std::string& PSOName) override;
	virtual void RSSetScissorRects(long left, long top, long right, long bottom) override;
	virtual void ResourceBarrier(unsigned int NumberBarrier, std::shared_ptr<FResource> Resource, int stateBefore, int stateAfter) override;
	virtual void SetPipelineState(std::shared_ptr<RenderItem> renderItem, FMaterial Mat) override;
	virtual void CreateTextureResource(std::shared_ptr<FRenderScene> renderResource, FTexture* TextureResource, bool isNormal) override;
	virtual void ExecuteCommandLists() override;
	virtual void ChangePSOState(FMaterial Mat, const PipelineState& PSO, const std::wstring& Shader)override;


	virtual void RenderFrameBegin(std::shared_ptr<FRenderScene> renderResource, const std::string& MatName) override;
	virtual void DrawMesh(std::shared_ptr<RenderItem> renderItem, const std::string& renderItemName, bool IsDrawDepth, bool isNeedRTV, int RTVNumber, int width , int height ) override;
	virtual void ClearAndSetRenderTatget(unsigned __int64 ClearRenderTargetHand, unsigned __int64 ClearDepthStencilHand, int numTatgetDescriptors, unsigned __int64 SetRenderTargetHand,
		bool RTsSingleHandleToDescriptorRange, unsigned __int64 SetDepthStencilHand) override;
public:
	virtual void BeginEvent(const std::string EventName);
	virtual void EndEvent();
	virtual Buffer* CreateBuffer(std::shared_ptr<RenderItem> renderItem, const std::string& Name) override;
	virtual void CreateShader( const std::wstring& filename) override;
	virtual void CreateCbHeapsAndSrv(const std::string& ActorName, const std::string& MeshName, RenderItem* renderItem, FRenderResource* shadowResource,  FRenderResource* HDRResource, std::shared_ptr<FRenderScene> sceneResource) override;
protected:
	HWND mhMainWnd = nullptr;

	bool mMinimized = false;        //??????
	bool mMaximized = false;        //??????
	bool mResizing = false;         //??????????????????
	bool mFullscreenState = false;  //????????

	static DX12RHI* mDX12RHI;
	int offsetIndex = 0;
	int CBindex = 0;
	SIZE_T vertexOffset =0;
	SIZE_T indexOffset =0 ;
	glm::vec3 cameraLoc;
private:
	std::set<std::string> PSONames;
	std::string currentPSOName;
	ComPtr<ID3D12DescriptorHeap> mCbvSrvHeaps;
    std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB ;
	std::unique_ptr<UploadBuffer<FMaterialConstants>> mMaterialCB ;

	std::unique_ptr<DXBuffer> mGeo = nullptr;

	ComPtr<ID3DBlob> mvsByteCode = nullptr;
	ComPtr<ID3DBlob> mpsByteCode = nullptr;
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	bool m4xMsaaState = false;
	UINT m4xMsaaQuality = 0;

	std::unordered_map<std::string,ComPtr<ID3D12PipelineState>> mPSO ;
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