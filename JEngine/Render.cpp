#include "stdafx.h"
#include "Render.h"
#include "DX12RHI.h"
#include "DXRHIResource.h"
#include "Engine.h"
#include "SceneManager.h"
#include "AssetManager.h"
bool FRender::Init()
{
	mRHI = std::make_unique<DX12RHI>();
	mRHIResource = std::make_unique<DXRHIResource>();
	if (!mRHI->Initialize()) {
		return false;
	}
	return true;
}

void FRender::RenderBegin()
{
	for (auto&& texture : AssetManager::GetAssetManager()->GetTextures()) {
		mRHI->LoadTexture(texture.get());
	}
	
	mRHI->DrawPrepare();
}

void FRender::Render(const GameTimer& gt)
{
	mRHI->ResetCommand();
	mRHI->RSSetViewports(0.0f, 0.0f, (float)Engine::GetEngine()->GetWindow()->GetClientWidht(), (float)Engine::GetEngine()->GetWindow()->GetClientHeight(), 0.0f, 1.0f);
	mRHI->RSSetScissorRects(0, 0, Engine::GetEngine()->GetWindow()->GetClientWidht(), Engine::GetEngine()->GetWindow()->GetClientHeight());
	mRHI->ResourceBarrier(1, DX_RESOURCE_STATES::PRESENT,DX_RESOURCE_STATES::RENDER_TARGET);
	mRHI->ClearRenderTargetView();
	mRHI->ClearDepthStencilView();
	mRHI->OMSetStencilRef(0);
	mRHI->OMSetRenderTargets(1,true);

	for (int i = 0; i < SceneManager::GetSceneManager()->GetAllActor().size(); i++) {
		mRHI->SetDescriptorHeaps(i);
		mRHI->SetGraphicsRootSignature();
		mRHI->IASetVertexBuffers();
		mRHI->IASetIndexBuffer();
		mRHI->IASetPrimitiveTopology();
		mRHI->SetGraphicsRootDescriptorTable(i);
		mRHI->SetGraphicsRoot32BitConstants();
		mRHI->DrawIndexedInstanced(i);
	}
	mRHI->ResourceBarrier(1, DX_RESOURCE_STATES::RENDER_TARGET, DX_RESOURCE_STATES::PRESENT);
	mRHI->Update(gt);
	mRHI->ExecuteCommandLists();
}

FRender::~FRender()
{
	if (mRHI != nullptr) {
		mRHI = nullptr;
	}
}
