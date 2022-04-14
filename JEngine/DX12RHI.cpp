#include "stdafx.h"
#include "DX12RHI.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "ShaderManager.h"
#include "MaterialManager.h"
#include "Engine.h"
#include "DXRHIResource.h"
#include "FHDRResource.h"
#include "FRenderScene.h"
#include "FShadowResource.h"
DX12RHI* DX12RHI::mDX12RHI = nullptr;
DX12RHI::DX12RHI() 
{
	assert(mDX12RHI == nullptr);
	if (mDX12RHI == nullptr) {
		mDX12RHI = this;
	}
	SetClientHeight(Engine::GetEngine()->GetWindow()->GetClientHeight());
	SetClientWidht(Engine::GetEngine()->GetWindow()->GetClientWidht());
	SetWindow(Engine::GetEngine()->GetWindow()->GetHWnd());
}

DX12RHI::~DX12RHI()
{
	if (mDX12RHI != nullptr) {
		mDX12RHI = nullptr;
	}
}
 
DX12RHI* DX12RHI::GetDX12RHI()
{
	return mDX12RHI;
}

bool DX12RHI::Initialize()
{
	if (!InitDirect3D()) {
		return false;
	}

	OnResize();
	BulidDescriptorHeaps();
	mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 1000, true);
	mMaterialCB = std::make_unique<UploadBuffer<FMaterialConstants>>(md3dDevice.Get(), 1000, true);
	return true;
}

void DX12RHI::OnResize()
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

	SceneManager::GetSceneManager()->GetCamera()->SetCameraPos(2000.0f, 2000.0f, 2000.0f);
	SceneManager::GetSceneManager()->GetCamera()->SetLens(0.25f * glm::pi<float>(), AspectRatio(), 1.0f, 10000.0f);
	SceneManager::GetSceneManager()->GetCamera()->LookAt(SceneManager::GetSceneManager()->GetCamera()->GetCameraPos3f(), 
	glm::vec3(0.0f, 0.0f, 0.0f), SceneManager::GetSceneManager()->GetCamera()->GetUp());
}

Microsoft::WRL::ComPtr<ID3D12Device> DX12RHI::GetDevice()
{
	return md3dDevice;
}

float DX12RHI::AspectRatio() const
{
	return static_cast<float>(mClientWidht) / mClientHeight;
}

bool DX12RHI::Get4xMsaaState() const
{
	return m4xMsaaState;
}

bool DX12RHI::IsHaveDevice() const
{
	if (md3dDevice) {
		return true;
	}
	return false;
}

void DX12RHI::Set4xMsaaState(bool value)
{
	if (m4xMsaaState != value) {
		m4xMsaaState = value;

		CreateSpawChain();
		OnResize();
	}
}
void DX12RHI::SetWindow(HWND mhMainWnd)
{
	this->mhMainWnd = mhMainWnd;
}

void DX12RHI::SetClientWidht(int Width)
{
	this->mClientWidht = Width;
}

void DX12RHI::SetClientHeight(int Height)
{
	this->mClientHeight = Height;
}
void DX12RHI::UpdateCB(std::shared_ptr<FRenderScene> sceneResource,const std::string& Name,int CBIndex,FMaterial Mat)
{
	Time = Engine::GetEngine()->Time;
	cameraLoc = Engine::GetEngine()->GetSceneManager()->GetCamera()->GetCameraPos3f();
	    Engine::GetEngine()->GetSceneManager()->GetCamera()->UpdateViewMat();
		ObjectConstants objConstants;
		objConstants.Time = Time;
		glm::mat4x4 proj = Engine::GetEngine()->GetSceneManager()->GetCamera()->GetProj4x4();
		glm::mat4x4 view = Engine::GetEngine()->GetSceneManager()->GetCamera()->GetView4x4();
		objConstants.ViewProj = glm::transpose(proj * view);
		objConstants.TLightViewProj = sceneResource->TLightViewProj;
		objConstants.World = sceneResource->mRenderItem[Name]->World;
		objConstants.Rotation = sceneResource->mRenderItem[Name]->Rotation;
		//objConstants.TexTransform = glm::transpose(sceneResource->mRenderItem[Name]->MatTransform) * sceneResource->mRenderItem[Name]->Scale;
		objConstants.directionalLight.Brightness = Engine::GetEngine()->GetSceneManager()->DirectionalLight.Brightness;
		objConstants.directionalLight.Direction = sceneResource->LightDirection;
		objConstants.directionalLight.Location = Engine::GetEngine()->GetSceneManager()->DirectionalLight.Location;
		objConstants.LightViewProj = sceneResource->LightViewProj;
		mObjectCB->CopyData(CBIndex, objConstants);
		FMaterialConstants materialConstants;
		materialConstants = Mat.mMaterialConstants;
		//materialConstants.MatTransform = glm::transpose(sceneResource->mRenderItem[Name]->Mat.mMaterialConstants.MatTransform);
		mMaterialCB->CopyData(CBIndex, materialConstants);

}



void DX12RHI::BuildRenderItem(std::shared_ptr<FRenderScene> sceneResource, const std::string& Name, const std::string& MatName)
{

	if (sceneResource->mRenderItem[Name]->mGeo->Name != "") {
		return;
	}
	std::unordered_map<std::string, MeshData> meshData = sceneResource->BuildMeshData();
	//std::string glod = std::string("StaticMeshActor_5");
	/*glod.resize(glod.size()+1);*/
	//if (Name == glod) {
	//	sceneResource->mRenderItem[Name]->Mat.mMaterialConstants.DiffuseAlbedo= { 1.0f, 1.0f, 1.0f, 1.0f };
	//	sceneResource->mRenderItem[Name]->Mat.mMaterialConstants.FresnelR0= { 0.5f, 0.5f, 0.5f };
	//	sceneResource->mRenderItem[Name]->Mat.mMaterialConstants.Roughness= 0.01f;
	//}
	sceneResource->mRenderItem[Name]->MatName = MatName;
	const UINT vbByteSize = (UINT)meshData[Name].vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)meshData[Name].indices.size() * sizeof(uint32_t);
	sceneResource->mRenderItem[Name]->mGeo->Name = Name;
	ThrowIfFailed(D3DCreateBlob(vbByteSize, &sceneResource->mRenderItem[Name]->mGeo->VertexBufferCPU));
	CopyMemory(sceneResource->mRenderItem[Name]->mGeo->VertexBufferCPU->GetBufferPointer(), meshData[Name].vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &sceneResource->mRenderItem[Name]->mGeo->IndexBufferCPU));
	CopyMemory(sceneResource->mRenderItem[Name]->mGeo->IndexBufferCPU->GetBufferPointer(), meshData[Name].indices.data(), ibByteSize);

	sceneResource->mRenderItem[Name]->mGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(), meshData[Name].vertices.data(), vbByteSize, sceneResource->mRenderItem[Name]->mGeo->VertexBufferUploader);
	sceneResource->mRenderItem[Name]->mGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(), meshData[Name].indices.data(), ibByteSize, sceneResource->mRenderItem[Name]->mGeo->IndexBufferUploader);

	sceneResource->mRenderItem[Name]->mGeo->VertexByteStride = sizeof(Vertex);
	sceneResource->mRenderItem[Name]->mGeo->VertexBufferByteSize = vbByteSize;
	sceneResource->mRenderItem[Name]->mGeo->IndexFormat = DXGI_FORMAT_R32_UINT;
	sceneResource->mRenderItem[Name]->mGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)meshData[Name].indices.size();

	submesh.StartIndexLocation = sceneResource->mRenderItem[Name]->StartIndexLocation;
	submesh.BaseVertexLocation = sceneResource->mRenderItem[Name]->BaseVertexLocation;

	sceneResource->mRenderItem[Name]->mGeo->DrawArgs[Name] = submesh;
}

void DX12RHI::RenderFrameBegin(std::shared_ptr<FRenderScene> renderResource, const std::string& ActorName, int RenderItemIndex, const std::string& MatName)
{
	if (renderResource->mRenderItem[ActorName] == nullptr) {
		renderResource->mRenderItem[ActorName] = std::make_shared<RenderItem>();
	}
	if (renderResource->mRenderItem[ActorName]->mGeo == nullptr) {
		renderResource->mRenderItem[ActorName]->mGeo = std::make_unique<DXBuffer>();
	}
	BuildRenderItem(renderResource, ActorName, MatName);
}

void DX12RHI::DrawMesh(std::shared_ptr<FRenderScene> renderResource, const std::string& renderItemName,bool IsDrawDepth,bool isNeedRTV)
{
	IASetVertexAndIndexBuffers(CreateBuffer(renderResource, renderItemName));
	IASetPrimitiveTopology();
	SetGraphicsRootDescriptorTable(renderResource->mRenderItem[renderItemName].get(), IsDrawDepth, isNeedRTV);
	DrawIndexedInstanced(renderResource, renderItemName);
}

void DX12RHI::ClearAndSetRenderTatget(unsigned __int64 ClearRenderTargetHand, unsigned __int64 ClearDepthStencilHand, int numTatgetDescriptors, unsigned __int64 SetRenderTargetHand, bool RTsSingleHandleToDescriptorRange, unsigned __int64 SetDepthStencilHand)
{
	if (ClearRenderTargetHand != 0) {
		ClearRenderTargetView(ClearRenderTargetHand);
	}
	ClearDepthStencilView(ClearDepthStencilHand);
	OMSetRenderTargets(numTatgetDescriptors, SetRenderTargetHand, RTsSingleHandleToDescriptorRange, SetDepthStencilHand);
	SetDescriptorHeaps();
	SetGraphicsRootSignature();
}



Buffer* DX12RHI::CreateBuffer(std::shared_ptr<FRenderScene> sceneResource,const std::string& Name)
{
	return sceneResource->mRenderItem[Name]->mGeo.get();
}


void DX12RHI::CreateShader(const std::wstring& filename)
{
	ShaderManager::GetShaderManager()->CompileShader(filename);
}

void DX12RHI::CreateCbHeapsAndSrv(const std::string& ActorName, ActorStruct* Actor, FRenderResource* shadowResource, FRenderResource* HDRResource, std::shared_ptr<FRenderScene> sceneResource)
{
	StaticMeshInfo* MeshInfo = Engine::GetEngine()->GetAssetManager()->FindAssetByActor(*Actor);
	BulidConstantBuffers(ActorName, sceneResource->mRenderItem[ActorName].get());
	BuildShaderResourceView(ActorName, MeshInfo->StaticMeshName, shadowResource, HDRResource, sceneResource);
}


void DX12RHI::BulidDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors =1000;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCbvSrvHeaps)));

}

void DX12RHI::BulidConstantBuffers(const std::string& Name,RenderItem* renderItem)
{

	UINT objecBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mCbvSrvHeaps->GetCPUDescriptorHandleForHeapStart());
	hDescriptor.Offset(offsetIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	cbAddress += CBindex * objecBByteSize;
	renderItem->ObjCBIndex = offsetIndex;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	md3dDevice->CreateConstantBufferView(&cbvDesc, hDescriptor);


	UINT materialByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FMaterialConstants));
	D3D12_GPU_VIRTUAL_ADDRESS materialCbAddress = mMaterialCB->Resource()->GetGPUVirtualAddress();

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor2(mCbvSrvHeaps->GetCPUDescriptorHandleForHeapStart());
	hDescriptor2.Offset(++offsetIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	materialCbAddress += CBindex * materialByteSize;
	renderItem->MaterialCBIndex = offsetIndex;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc2;
	cbvDesc2.BufferLocation = materialCbAddress;
	cbvDesc2.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(FMaterialConstants));
	md3dDevice->CreateConstantBufferView(&cbvDesc2, hDescriptor2);
	CBindex++;
}

void DX12RHI::BuildMaterial(const std::string& Name,FRenderResource* RenderResource)
{
	//mMaterialCB[Name] = std::make_unique<UploadBuffer<FMaterial>>(md3dDevice.Get(),1, true);
	//UINT matBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FMaterial));

}

void DX12RHI::BuildShaderResourceView(const std::string& ActorName, const std::string& Name, FRenderResource* RenderResource, FRenderResource* HDRResource, std::shared_ptr<FRenderScene> renderScene)
{
	renderScene->mRenderItem[ActorName]->ObjSrvIndex = ++offsetIndex;
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mCbvSrvHeaps->GetCPUDescriptorHandleForHeapStart());
	hDescriptor.Offset(offsetIndex, mCbvSrvUavDescriptorSize);
	std::string ResourceName;
	//消除虚幻导出的'\0'
	std::string str(Name.c_str());
	str.resize(str.size());
	ComPtr<ID3D12Resource>  woodCrateNormal;
	ComPtr<ID3D12Resource>  woodCrateTex;
	if (renderScene->mNormalTextures.find(str) == renderScene->mNormalTextures.end()) {
		ResourceName = "Null";
		woodCrateNormal = renderScene->mTextures[ResourceName]->Resource;
	}
	else {
		ResourceName = str;
		woodCrateNormal = renderScene->mNormalTextures[ResourceName]->Resource;

	}
	if (renderScene->mTextures.find(str) == renderScene->mTextures.end()) {
		ResourceName = "Null";
		woodCrateTex = renderScene->mTextures[ResourceName]->Resource;
	}
	else
	{
		ResourceName = str;
		woodCrateTex = renderScene->mTextures[ResourceName]->Resource;
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = woodCrateTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = woodCrateTex->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	md3dDevice->CreateShaderResourceView(woodCrateTex.Get(), &srvDesc, hDescriptor);

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor2(mCbvSrvHeaps->GetCPUDescriptorHandleForHeapStart());
	hDescriptor2.Offset(++offsetIndex, mCbvSrvUavDescriptorSize);
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2 = {};
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.Format = woodCrateNormal->GetDesc().Format;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MostDetailedMip = 0;
	srvDesc2.Texture2D.MipLevels = woodCrateNormal->GetDesc().MipLevels;
	srvDesc2.Texture2D.ResourceMinLODClamp = 0.0f;
	md3dDevice->CreateShaderResourceView(woodCrateNormal.Get(), &srvDesc2, hDescriptor2);

	auto srvCpuStart = mCbvSrvHeaps->GetCPUDescriptorHandleForHeapStart();
	auto srvGpuStart = mCbvSrvHeaps->GetGPUDescriptorHandleForHeapStart();
	auto dsvCpuStart = mDsvHeap->GetCPUDescriptorHandleForHeapStart();
	  dynamic_cast<DXShadowResource*>(RenderResource)->BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, ++offsetIndex, mCbvSrvUavDescriptorSize),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, offsetIndex, mCbvSrvUavDescriptorSize),
		CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 1, mDsvDescriptorSize));
    auto rtvCpuStart = mRtvHeap->GetCPUDescriptorHandleForHeapStart();
	  dynamic_cast<DXHDRResource*>(HDRResource)->BuildDescriptors(
		  CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, ++offsetIndex, mCbvSrvUavDescriptorSize),
		  CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, offsetIndex, mCbvSrvUavDescriptorSize),
		  CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 2, mDsvDescriptorSize),
		  CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvCpuStart, 2, mRtvDescriptorSize));
	  renderScene->mRenderItem[ActorName]->ObjRtvIndex = offsetIndex;
	 offsetIndex++;
}



void DX12RHI::BulidRootSignature(FShader* shader)
{
	ThrowIfFailed(md3dDevice->CreateRootSignature(0, shader->mvsByteCode->GetBufferPointer(), shader->mvsByteCode->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));
}

void DX12RHI::BuildPSO(std::shared_ptr<RenderItem> renderItem,FMaterial Mat)
{

	if (PSONames.find(Mat.mPso.PSOName) != PSONames.end()) {
		return;
	}
	BulidRootSignature(ShaderManager::GetShaderManager()->CompileShader(Mat.GlobalShader));
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PSOState;
	Mat.mPso.dxPSO.pRootSignature = mRootSignature.Get();
	Mat.mPso.dxPSO.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	Mat.mPso.dxPSO.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	PSOState = Mat.mPso.dxPSO;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&PSOState, IID_PPV_ARGS(&mPSO[Mat.mPso.PSOName])));
	PSONames.insert(Mat.mPso.PSOName);
}

void DX12RHI::CreateTextureResource(std::shared_ptr<FRenderScene> renderResource, FTexture* TextureResource, bool isNormal)
{
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	auto texture = dynamic_cast<FTexture*>(TextureResource);

	auto createNullTex = std::make_unique<Texture>();
	createNullTex->Name = texture->Name;
	createNullTex->Filename =texture->FilePath;
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(), mCommandList.Get(), createNullTex->Filename.c_str(), createNullTex->Resource, createNullTex->UploadHeap));
	if (isNormal)
	{
		renderResource->mNormalTextures[createNullTex->Name] = std::move(createNullTex);
	}
	else
	{
		renderResource->mTextures[createNullTex->Name] = std::move(createNullTex);
	}

	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

void DX12RHI::ExecuteCommandLists()
{
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
	FlushCommandQueue();
	currentPSOName = "";
}

void DX12RHI::ChangePSOState(FMaterial Mat,const PipelineState& PSO, const std::wstring& Shader)
{
	//renderItem->Mat.GlobalShader = MaterialManager::GetMaterialManager()->SearchMaterial(PSOName).GlobalShader;
	//renderItem->Mat.mPso = MaterialManager::GetMaterialManager()->SearchMaterial(PSOName).mPso;
	Mat.GlobalShader = Shader;
	Mat.mPso = PSO;
}

void DX12RHI::RSSetViewports(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth)
{
	mScreenViewport.Height = Height;
	mScreenViewport.MaxDepth = MaxDepth;
	mScreenViewport.MinDepth = MinDepth;
	mScreenViewport.TopLeftX = TopLeftX;
	mScreenViewport.TopLeftY = TopLeftY;
	mScreenViewport.Width = Width;
	mCommandList->RSSetViewports(1, &mScreenViewport);
}

void DX12RHI::ResetCommand(const std::string& PSOName)
{
	if (PSOName == "Null") {
		ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	}
	else
	{
		ThrowIfFailed(mDirectCmdListAlloc->Reset());
		ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO[PSOName].Get()));
	}

}

void DX12RHI::RSSetScissorRects(long left, long top, long right, long bottom)
{
	mScissorRect.bottom = bottom;
	mScissorRect.right = right;
	mScissorRect.left = left;
	mScissorRect.top = top;
	mCommandList->RSSetScissorRects(1, &mScissorRect);
}

void DX12RHI::ResourceBarrier(unsigned int NumberBarrier, std::shared_ptr<FResource> Resource, int stateBefore , int stateAfter)
{
	mCommandList->ResourceBarrier(NumberBarrier, &CD3DX12_RESOURCE_BARRIER::Transition(Resource->Resource, D3D12_RESOURCE_STATES(stateBefore), D3D12_RESOURCE_STATES(stateAfter)));
}

void DX12RHI::ClearRenderTargetView(unsigned __int64 ptr)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = ptr;
	mCommandList->ClearRenderTargetView(handle, Colors::LightSteelBlue, 0, nullptr);
}

void DX12RHI::ClearDepthStencilView(unsigned __int64 ptr)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = ptr;
	mCommandList->ClearDepthStencilView(handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void DX12RHI::OMSetStencilRef(int StencilRef)
{
	mCommandList->OMSetStencilRef(StencilRef);
}

void DX12RHI::OMSetRenderTargets(int numTatgetDescriptors, unsigned __int64 RTptr, bool RTsSingleHandleToDescriptorRange, unsigned __int64 DSptr)
{
	std::unique_ptr<CD3DX12_CPU_DESCRIPTOR_HANDLE> RThandle = std::make_unique<CD3DX12_CPU_DESCRIPTOR_HANDLE>();
	std::unique_ptr<CD3DX12_CPU_DESCRIPTOR_HANDLE> DShandle = std::make_unique<CD3DX12_CPU_DESCRIPTOR_HANDLE>();

	if (RTptr != 0) {
		RThandle->ptr = RTptr;
	}
	else
	{
		RThandle = nullptr;
	}

	if (DSptr != 0) {
		DShandle->ptr = DSptr;
	}
	else
	{
		DShandle = nullptr;
	}
	//mCommandList->OMSetRenderTargets(numTatgetDescriptors, &CurrentBackBufferView(), RTsSingleHandleToDescriptorRange, &DepthStencilView());
	mCommandList->OMSetRenderTargets(numTatgetDescriptors, RThandle.get(), RTsSingleHandleToDescriptorRange, DShandle.get());
}

void DX12RHI::SetDescriptorHeaps()
{
	//ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvSrvHeap[Name].Get() };
	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvSrvHeaps.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

void DX12RHI::SetGraphicsRootSignature()
{
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
}

void DX12RHI::IASetVertexAndIndexBuffers(Buffer* buffer)
{
	auto dxBuffer = dynamic_cast<DXBuffer*>(buffer);

	mCommandList->IASetVertexBuffers(0, 1, &dxBuffer->VertexBufferView());
	mCommandList->IASetIndexBuffer(&dxBuffer->IndexBufferView());
}

void DX12RHI::IASetPrimitiveTopology()
{
	mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void DX12RHI::SetGraphicsRootDescriptorTable(RenderItem* renderItem,bool isDepth,bool isNeedRTV)
{
	//CD3DX12_GPU_DESCRIPTOR_HANDLE hDescriptor(mCbvSrvHeap[Name]->GetGPUDescriptorHandleForHeapStart());
	//mCommandList->SetGraphicsRootDescriptorTable(0, mCbvSrvHeap[Name]->GetGPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE hDescriptor(mCbvSrvHeaps->GetGPUDescriptorHandleForHeapStart());
	hDescriptor.Offset(renderItem->ObjCBIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	mCommandList->SetGraphicsRootDescriptorTable(0, hDescriptor);
	if (!isDepth) {
		CD3DX12_GPU_DESCRIPTOR_HANDLE hDescriptor2(mCbvSrvHeaps->GetGPUDescriptorHandleForHeapStart());
		hDescriptor2.Offset(renderItem->ObjSrvIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		mCommandList->SetGraphicsRootDescriptorTable(1, hDescriptor2);
	}
	if (isNeedRTV) {
		CD3DX12_GPU_DESCRIPTOR_HANDLE hDescriptor3(mCbvSrvHeaps->GetGPUDescriptorHandleForHeapStart());
		hDescriptor3.Offset(renderItem->ObjRtvIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		mCommandList->SetGraphicsRootDescriptorTable(2, hDescriptor3);
	}
	SetGraphicsRoot32BitConstants();
}

void DX12RHI::SetGraphicsRoot32BitConstants()
{
	mCommandList->SetGraphicsRoot32BitConstants(3, 3, &cameraLoc, 0);
	int RenderTargetSize[2] = {Engine::GetEngine()->GetWindow()->GetClientWidht(), Engine::GetEngine()->GetWindow()->GetClientHeight()};
	mCommandList->SetGraphicsRoot32BitConstants(3, 2, &RenderTargetSize, 0);
}

void DX12RHI::SetPipelineState(std::shared_ptr<RenderItem> renderItem,FMaterial Mat)
{
	BuildPSO(renderItem, Mat);
	if (Mat.mPso.PSOName != currentPSOName) {
		mCommandList->SetPipelineState(mPSO[Mat.mPso.PSOName].Get());
	}
	else {
		return;
	}
	currentPSOName = Mat.mPso.PSOName;
}

void DX12RHI::DrawIndexedInstanced(std::shared_ptr<FRenderScene> sceneResource, const std::string& Name)
{
	mCommandList->DrawIndexedInstanced(sceneResource->mRenderItem[Name]->mGeo->DrawArgs[Name].IndexCount, 1,
		(UINT)sceneResource->mRenderItem[Name]->mGeo->DrawArgs[Name].StartIndexLocation,
		(UINT)sceneResource->mRenderItem[Name]->mGeo->DrawArgs[Name].BaseVertexLocation, 0);

}


bool DX12RHI::InitDirect3D()
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

void DX12RHI::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));
	ThrowIfFailed(md3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));
	ThrowIfFailed(md3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mDirectCmdListAlloc.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())));
	mCommandList->Close();
}
void DX12RHI::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.NodeMask = 0;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NumDescriptors = 3;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors =3;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}
void DX12RHI::CreateSpawChain()
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

void DX12RHI::FlushCommandQueue()
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

ID3D12Resource* DX12RHI::CurrentBackBuffer() const
{
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}


D3D12_CPU_DESCRIPTOR_HANDLE DX12RHI::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackBuffer, mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12RHI::DepthStencilView() const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}


void DX12RHI::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
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

void DX12RHI::LogAdapterOutputs(IDXGIAdapter* adapter)
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

		LogOutputDisplayerModes(output, mBackBufferFormat);

		ReleaseCom(output);
		++i;
	}
}

void DX12RHI::LogOutputDisplayerModes(IDXGIOutput* output, DXGI_FORMAT format)
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
