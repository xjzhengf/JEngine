#include "stdafx.h"
#include "DX12Render.h"
#include "SceneManager.h"
#include "AssetManager.h"

DX12Render::DX12Render() :D3DInit()
{
}

DX12Render::~DX12Render()
{
}

bool DX12Render::Initialize()
{


	if (!D3DInit::Initialize())
		return false;

	LoadTexture();

	return true;
}

void DX12Render::OnResize()
{
	D3DInit::OnResize();
	SceneManager::GetSceneManager()->GetCamera()->SetCameraPos(2000.0f, 2000.0f, 2000.0f);
	SceneManager::GetSceneManager()->GetCamera()->SetLens(0.25f * glm::pi<float>(), AspectRatio(), 1.0f, 10000.0f);
	SceneManager::GetSceneManager()->GetCamera()->LookAt(SceneManager::GetSceneManager()->GetCamera()->GetCameraPos3f(), 
		glm::vec3(0.0f, 0.0f, 0.0f), SceneManager::GetSceneManager()->GetCamera()->GetUp());
}

void DX12Render::Update(const GameTimer& gt)
{
}

void DX12Render::Draw(const GameTimer& gt)
{
	if (DoOnce) {
		DrawPrepare();
		DoOnce = false;
	}
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

		ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap[i].Get() };
		mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		mCommandList->SetGraphicsRootSignature(mRootSigmature.Get());

		mCommandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());
		mCommandList->IASetIndexBuffer(&mBoxGeo->IndexBufferView());
		mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mCommandList->SetGraphicsRootDescriptorTable(0, mCbvHeap[i]->GetGPUDescriptorHandleForHeapStart());
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

void DX12Render::DrawPrepare()
{
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	size_t SceneSize = SceneManager::GetSceneManager()->GetAllActor().size();

	BulidRootSignature();
	BulidShadersAndInputLayout();
	mCbvHeap.resize(SceneSize);
	mObjectCB.resize(SceneSize);
	int i = 0;
	for (auto&& ActorPair : SceneManager::GetSceneManager()->GetAllActor()) {
		StaticMeshInfo* MeshInfo = AssetManager::GetAssetManager()->FindAssetByActor(*ActorPair.second);
		BuildStaticMeshData(MeshInfo);
		BulidDescriptorHeaps(i);
		BulidConstantBuffers(i);
		i++;
	}
	BuildShaderResourceView();
	BuildStaticMeshGeometry(meshDataVector);
	BuildPSO();
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	FlushCommandQueue();
	isUpdateDraw = false;
}


std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> DX12Render::GetStaticSamplers()
{
	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}

void DX12Render::BulidDescriptorHeaps(int index)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCbvHeap[index])));
}

void DX12Render::BulidConstantBuffers(int index)
{
	mObjectCB[index] = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 1, true);
	UINT objecBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB[index]->Resource()->GetGPUVirtualAddress();
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objecBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	md3dDevice->CreateConstantBufferView(&cbvDesc, mCbvHeap[index]->GetCPUDescriptorHandleForHeapStart());

}

void DX12Render::BuildShaderResourceView()
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));
	//获取偏移
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	auto woodCrateTex = mTextures["ZLStaticMesh"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = woodCrateTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = woodCrateTex->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	md3dDevice->CreateShaderResourceView(woodCrateTex.Get(), &srvDesc, hDescriptor);
}



void DX12Render::BulidRootSignature()
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);
	slotRootParameter[1].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	auto staticSamplers = GetStaticSamplers();
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, (UINT)staticSamplers.size(), staticSamplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());
	if (errorBlob != nullptr) {
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(md3dDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&mRootSigmature)));
}

void DX12Render::BulidShadersAndInputLayout()
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

void DX12Render::BuildStaticMeshGeometry(std::vector<MeshData> meshData)
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
	mBoxGeo->Name = "DX12Render";
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

void DX12Render::BuildStaticMeshData(StaticMeshInfo* myStruct)
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

void DX12Render::BuildPSO()
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

void DX12Render::LoadTexture()
{
	auto createTex = std::make_unique<Texture>();
	createTex->Name = "ZLStaticMesh";
	createTex->Filename = L"..\\JEngine\\StaticMeshInfo\\UV\\em080_00_BML.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(), mCommandList.Get(), createTex->Filename.c_str(), createTex->Resource, createTex->UploadHeap));
	mTextures[createTex->Name] = std::move(createTex);
}

