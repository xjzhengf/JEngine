#pragma once
#include "stdafx.h"

#if defined(DEBUG)||defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC

#endif
#include "RHI.h"
#include "UploadBuffer.h"
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

struct Vertex
{
	glm::vec3 Pos;
	XMFLOAT4 Color;
	glm::vec3 Normal;
	glm::vec2 TexC;
};
struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	
};
struct ObjectConstants {
	//XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	glm::mat4x4 WorldViewProj = glm::identity<glm::mat4x4>();
	glm::mat4x4 Rotation = glm::identity<glm::mat4x4>();
	glm::mat4x4 Scale = glm::identity<glm::mat4x4>();
	glm::mat4x4 Translate = glm::identity<glm::mat4x4>();
	glm::mat4x4 TexTransform = glm::identity<glm::mat4x4>();
	float Time = 0.0f;
};

class DX12RHI  :public FRHI{
public:
	DX12RHI();
	DX12RHI(const DX12RHI& sm) = delete;
	DX12RHI& operator=(const DX12RHI& sm) = delete;
	~DX12RHI();
	float AspectRatio() const;
	bool Get4xMsaaState() const;
	bool IsHaveDevice() const;
	void Set4xMsaaState(bool value);

	 virtual bool Initialize() override ;
	 void OnResize() ;
	 virtual void Update(const GameTimer& gt) override;
	 virtual void Draw(const GameTimer& gt) override;

	void DrawPrepare() ;
	void SetWindow(HWND mhMainWnd);
	void SetClientWidht(int Width);
	void SetClientHeight(int Height);
private:
	void BulidDescriptorHeaps(int index);
	void BulidConstantBuffers(int index);
	void BuildShaderResourceView(int index, const std::string& Name);
	void BulidRootSignature();
	void BulidShadersAndInputLayout();
	void BuildStaticMeshGeometry(std::vector<MeshData> meshData);
	void BuildStaticMeshData(StaticMeshInfo* myStruct);
	void BuildPSO();
	void LoadTexture();
protected:
	HWND mhMainWnd = nullptr;

	bool mMinimized = false;        //最小化
	bool mMaximized = false;        //最大化
	bool mResizing = false;         //调整大小是否被拖动
	bool mFullscreenState = false;  //开启全屏

	static DX12RHI* mDX12RHI;
private:
	ComPtr<ID3D12RootSignature> mRootSigmature = nullptr;
	std::vector < ComPtr<ID3D12DescriptorHeap>> mCbvSrvHeap ;
	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
	std::vector<std::unique_ptr<UploadBuffer<ObjectConstants>>> mObjectCB ;
	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;

	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mNormal;

	ComPtr<ID3DBlob> mvsByteCode = nullptr;
	ComPtr<ID3DBlob> mpsByteCode = nullptr;
	
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	ComPtr<ID3D12PipelineState> mPSO = nullptr;

	glm::mat4x4 mWorld = glm::identity<glm::mat4x4>();

	POINT mLastMousePos;

	std::vector<MeshData> meshDataVector;

	float Time;

	bool IsRunDrawPrepare = true;
protected:
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSpawChain();
	void FlushCommandQueue();
	void CreateRtvAndDsvDescriptorHeaps();
	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayerModes(IDXGIOutput* output, DXGI_FORMAT format);
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
};