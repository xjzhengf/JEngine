#include "stdafx.h"
#include "DX12RHI.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "Engine.h"
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
	if (mBoxGeo!= nullptr)
	{
		mBoxGeo = nullptr;
	}
	if (mDX12RHI != nullptr) {
		mDX12RHI = nullptr;
	}
}

bool DX12RHI::Initialize()
{
	if (!InitDirect3D()) {
		return false;
	}

	OnResize();
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
void DX12RHI::Update(const GameTimer& gt)
{
	if (IsRunDrawPrepare) {
		DrawPrepare();
		IsRunDrawPrepare = false;
	}
}

void DX12RHI::Draw(const GameTimer& gt)
{

	ThrowIfFailed(mDirectCmdListAlloc->Reset());
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get()));

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	mCommandList->OMSetStencilRef(0);
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	Time = gt.TotalTime();
	

	glm::vec3 cameraLoc = SceneManager::GetSceneManager()->GetCamera()->GetCameraPos3f();
	

	int i = 0;
	for (auto&& ActorPair : SceneManager::GetSceneManager()->GetAllActor()) {
		SceneManager::GetSceneManager()->GetCamera()->UpdateViewMat();
		ObjectConstants objConstants;
		glm::qua<float> q = glm::qua<float>(
			ActorPair.second->Transform[0].Rotation.w,
			ActorPair.second->Transform[0].Rotation.x,
			ActorPair.second->Transform[0].Rotation.y,
			ActorPair.second->Transform[0].Rotation.z
			);

		glm::vec3 location = glm::vec3(
			ActorPair.second->Transform[0].Location.x,
			ActorPair.second->Transform[0].Location.y,
			ActorPair.second->Transform[0].Location.z
		);
		glm::vec3 Scale = glm::vec3(
			ActorPair.second->Transform[0].Scale3D.x,
			ActorPair.second->Transform[0].Scale3D.y,
			ActorPair.second->Transform[0].Scale3D.z
		);
		objConstants.Rotation = glm::mat4_cast(q);
		objConstants.Translate = glm::translate(objConstants.Translate, location);
		objConstants.Scale = glm::scale(objConstants.Scale, Scale);
		objConstants.Time = Time;
		glm::mat4x4 proj = SceneManager::GetSceneManager()->GetCamera()->GetProj4x4();
		glm::mat4x4 view = SceneManager::GetSceneManager()->GetCamera()->GetView4x4();

		glm::mat4x4 W = objConstants.Translate * objConstants.Rotation * objConstants.Scale;
		glm::mat4x4 worldViewProj = proj * view * W * mWorld;
		objConstants.WorldViewProj = glm::transpose(worldViewProj);
		mObjectCB[i]->CopyData(0, objConstants);
		

		
		ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvSrvHeap[i].Get() };
		mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		mCommandList->SetGraphicsRootSignature(mRootSigmature.Get()); 

	

		mCommandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());
		mCommandList->IASetIndexBuffer(&mBoxGeo->IndexBufferView());
		mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		CD3DX12_GPU_DESCRIPTOR_HANDLE hDescriptor(mCbvSrvHeap[i]->GetGPUDescriptorHandleForHeapStart());
		hDescriptor.Offset(1, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		mCommandList->SetGraphicsRootDescriptorTable(0, mCbvSrvHeap[i]->GetGPUDescriptorHandleForHeapStart());
		mCommandList->SetGraphicsRootDescriptorTable(1, hDescriptor);
		mCommandList->SetGraphicsRoot32BitConstants(2, 3, &cameraLoc, 0);
		mCommandList->DrawIndexedInstanced(mBoxGeo->DrawArgs[std::to_string(i)].IndexCount, 1,
			(UINT)mBoxGeo->DrawArgs[std::to_string(i)].StartIndexLocation, (UINT)mBoxGeo->DrawArgs[std::to_string(i)].BaseVertexLocation, 0);
		i++;
	}


	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	ThrowIfFailed(mCommandList->Close());

	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);


	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	FlushCommandQueue();
}

void DX12RHI::DrawPrepare()
{
	LoadTexture();
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	size_t SceneSize = SceneManager::GetSceneManager()->GetAllActor().size();
	BulidShadersAndInputLayout();
	BulidRootSignature();
	mCbvSrvHeap.resize(SceneSize);
	mObjectCB.resize(SceneSize);
	int i = 0;
	for (auto&& ActorPair : SceneManager::GetSceneManager()->GetAllActor()) {
		StaticMeshInfo* MeshInfo = AssetManager::GetAssetManager()->FindAssetByActor(*ActorPair.second);
		BuildStaticMeshData(MeshInfo);
		BulidDescriptorHeaps(i);
		BulidConstantBuffers(i);
		BuildShaderResourceView(i,MeshInfo->StaticMeshName);
		i++;
	}

	BuildStaticMeshGeometry(meshDataVector);
	BuildPSO();
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	FlushCommandQueue();
}


void DX12RHI::BulidDescriptorHeaps(int index)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 3;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCbvSrvHeap[index])));
}

void DX12RHI::BulidConstantBuffers(int index)
{
	mObjectCB[index] = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 1, true);
	UINT objecBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB[index]->Resource()->GetGPUVirtualAddress();
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objecBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	md3dDevice->CreateConstantBufferView(&cbvDesc, mCbvSrvHeap[index]->GetCPUDescriptorHandleForHeapStart());

}

void DX12RHI::BuildShaderResourceView(int index,const std::string& Name)
{
		//获取偏移
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mCbvSrvHeap[index]->GetCPUDescriptorHandleForHeapStart());
	hDescriptor.Offset(1, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	std::string ResourceName;

	//消除虚幻导出的'\0'
	std::string str(Name.c_str());
	str.resize(str.size());

	if (mTextures.find(str) == mTextures.end()) {
		ResourceName = "Null";
	}
	else {
		ResourceName = str;
	}
	auto woodCrateTex = mTextures[ResourceName]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = woodCrateTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = woodCrateTex->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	md3dDevice->CreateShaderResourceView(woodCrateTex.Get(), &srvDesc, hDescriptor);

	ComPtr<ID3D12Resource>  woodCrateNormal;
	if (ResourceName == "Null")
	{
		woodCrateNormal = mNormal["Null"]->Resource;
	}
	else
	{
		woodCrateNormal = mTextures[ResourceName]->Resource;
	}
	hDescriptor.Offset(1, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2 = {};
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.Format = woodCrateNormal->GetDesc().Format;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MostDetailedMip = 0;
	srvDesc2.Texture2D.MipLevels = woodCrateNormal->GetDesc().MipLevels;
	srvDesc2.Texture2D.ResourceMinLODClamp = 0.0f;
	md3dDevice->CreateShaderResourceView(woodCrateNormal.Get(), &srvDesc2, hDescriptor);
}



void DX12RHI::BulidRootSignature()
{
	ThrowIfFailed(md3dDevice->CreateRootSignature(0, mvsByteCode->GetBufferPointer(), mvsByteCode->GetBufferSize(), IID_PPV_ARGS(&mRootSigmature)));
}
void DX12RHI::BulidShadersAndInputLayout()
{
	HRESULT hr = S_OK;
	mvsByteCode = d3dUtil::CompileShader(L"..\\JEngine\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	mpsByteCode = d3dUtil::CompileShader(L"..\\JEngine\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
}

void DX12RHI::BuildStaticMeshGeometry(std::vector<MeshData> meshData)
{
	//将模型数据数组里的数据合并为一个大数据
	size_t totalVertexSize = 0;
	size_t totalIndexSize = 0;
	std::vector<size_t> vertexOffset(meshData.size());
	std::vector<size_t> indexOffset(meshData.size());
	for (size_t i = 0; i < meshData.size(); i++) {
		if (i == 0) {
			vertexOffset[i] = 0;
			indexOffset[i] = 0;
		}
		else
		{
			vertexOffset[i] = meshData[i - 1].vertices.size() + vertexOffset[i - 1];
			indexOffset[i] = meshData[i - 1].indices.size() + indexOffset[i - 1];
		}
	}
	std::vector<Vertex> vertices(totalVertexSize);
	std::vector<uint32_t> indices(totalIndexSize);
	for (size_t i = 0; i < meshData.size(); i++) {


		for (size_t k = 0; k < meshData[i].vertices.size(); k++)
		{
			vertices.push_back(meshData[i].vertices[k]);
		}
		for (size_t k = 0; k < meshData[i].indices.size(); k++)
		{
			indices.push_back(meshData[i].indices[k]);
		}
	}
	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(uint32_t);

	mBoxGeo = std::make_unique<MeshGeometry>();
	mBoxGeo->Name = "DX12RHI";
	ThrowIfFailed(D3DCreateBlob(vbByteSize, &mBoxGeo->VertexBufferCPU));
	CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &mBoxGeo->IndexBufferCPU));
	CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	mBoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(), vertices.data(), vbByteSize, mBoxGeo->VertexBufferUploader);
	mBoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(), indices.data(), ibByteSize, mBoxGeo->IndexBufferUploader);

	mBoxGeo->VertexByteStride = sizeof(Vertex);
	mBoxGeo->VertexBufferByteSize = vbByteSize;
	mBoxGeo->IndexFormat = DXGI_FORMAT_R32_UINT;
	mBoxGeo->IndexBufferByteSize = ibByteSize;


	for (int i = 0; i < meshData.size(); i++) {
		totalVertexSize += meshData[i].vertices.size();
		totalIndexSize += meshData[i].indices.size();
		SubmeshGeometry submesh;
		submesh.IndexCount = (UINT)meshData[i].indices.size();
		submesh.StartIndexLocation = indexOffset[i];
		submesh.BaseVertexLocation = vertexOffset[i];
		std::string name = std::to_string(i);
		mBoxGeo->DrawArgs[name] = submesh;
	}
}

void DX12RHI::BuildStaticMeshData(StaticMeshInfo* myStruct)
{
	MeshData meshData;
	meshData.indices = myStruct->Indices;

	meshData.indices.resize(myStruct->Indices.size());
	size_t VerticesLen = myStruct->Vertices.size();
	meshData.vertices.resize(VerticesLen);
	for (int i = 0; i < VerticesLen; i++) {
		meshData.vertices[i].Pos.x = myStruct->Vertices[i].x;
		meshData.vertices[i].Pos.y = myStruct->Vertices[i].y;
		meshData.vertices[i].Pos.z = myStruct->Vertices[i].z;
		meshData.vertices[i].TexC.x = myStruct->UV[i].x;
		meshData.vertices[i].TexC.y = myStruct->UV[i].y;

	}

	for (size_t i = 0; i < (myStruct->Indices.size()) / 3; i++) {
		UINT i0 = meshData.indices[i * 3 + 0];
		UINT i1 = meshData.indices[i * 3 + 1];
		UINT i2 = meshData.indices[i * 3 + 2];

		Vertex v0 = meshData.vertices[i0];
		Vertex v1 = meshData.vertices[i1];
		Vertex v2 = meshData.vertices[i2];

		glm::vec3 e0 = v1.Pos - v0.Pos;
		glm::vec3 e1 = v2.Pos - v0.Pos;
		glm::vec3 faceNormal = glm::cross(e0, e1);

		meshData.vertices[i0].Normal += faceNormal;
		meshData.vertices[i1].Normal += faceNormal;
		meshData.vertices[i2].Normal += faceNormal;
	}

	for (UINT i = 0; i < VerticesLen; i++)
	{
		meshData.vertices[i].Normal = glm::normalize(meshData.vertices[i].Normal);
	}
	meshDataVector.push_back(std::move(meshData));
}

void DX12RHI::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { mInputLayout.data(),(UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSigmature.Get();
	psoDesc.VS = {
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
		mvsByteCode->GetBufferSize()
	};
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
		mpsByteCode->GetBufferSize()
	};


	D3D12_DEPTH_STENCIL_DESC stencilDesc;
	//反面
	stencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	stencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	stencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	stencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
	stencilDesc.DepthEnable = true;
	stencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	stencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//正面
	stencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	stencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	stencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	stencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
	stencilDesc.StencilEnable = true;
	stencilDesc.StencilReadMask = 0xff;
	stencilDesc.StencilWriteMask = 0xff;


	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	//psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	//psoDesc.DepthStencilState.StencilEnable = TRUE;
	//psoDesc.DepthStencilState.StencilReadMask = 1;
	//psoDesc.DepthStencilState.StencilWriteMask = 1;
	psoDesc.DepthStencilState = stencilDesc;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = mBackBufferFormat;
	psoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	psoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}

void DX12RHI::LoadTexture()
{
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	auto createTex = std::make_unique<Texture>();
	createTex->Name = "ZLStaticMesh";
	createTex->Filename = L"..\\JEngine\\StaticMeshInfo\\UV\\em080_00_BML.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(), mCommandList.Get(), createTex->Filename.c_str(), createTex->Resource, createTex->UploadHeap));
	mTextures[createTex->Name] = std::move(createTex);

	auto createTex2 = std::make_unique<Texture>();
	createTex2->Name = "GLStaticMesh";
	createTex2->Filename = L"..\\JEngine\\StaticMeshInfo\\UV\\em012_BM_01.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(), mCommandList.Get(), createTex2->Filename.c_str(), createTex2->Resource, createTex2->UploadHeap));
	mTextures[createTex2->Name] = std::move(createTex2);

	auto createNullTex = std::make_unique<Texture>();
	createNullTex->Name = "Null";
	createNullTex->Filename = L"..\\JEngine\\StaticMeshInfo\\UV\\tile.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(), mCommandList.Get(), createNullTex->Filename.c_str(), createNullTex->Resource, createNullTex->UploadHeap));
	mTextures[createNullTex->Name] = std::move(createNullTex);


	auto createNormal = std::make_unique<Texture>();
	createNormal->Name = "Null";
	createNormal->Filename = L"..\\JEngine\\StaticMeshInfo\\UV\\tile_nmap.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(), mCommandList.Get(), createNormal->Filename.c_str(), createNormal->Resource, createNormal->UploadHeap));
	mNormal[createNormal->Name] = std::move(createNormal);


	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
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
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = 1;
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
