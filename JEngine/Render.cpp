#include "stdafx.h"
#include "Render.h"
#include "DXRHIResource.h"
#include "Engine.h"
#include "SceneManager.h"
#include "AssetManager.h"
bool FRender::Init()
{
	RHIFactory = std::make_unique<FRHIFactory>();
	mRHI = RHIFactory->CreateRHI();
	mRHIResource = RHIFactory->CreateRHIResource();
	mRenderResource = std::make_unique<FRenderResource>();
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
	mRHI->BuildPSO(mRHIResource.get(),"Scene");
	mRHI->ExecuteCommandLists();
}

void FRender::Render(const GameTimer& gt)
{
	mRHI->ResetCommand("Scene");
	mRHI->RSSetViewports(0.0f, 0.0f, (float)Engine::GetEngine()->GetWindow()->GetClientWidht(), (float)Engine::GetEngine()->GetWindow()->GetClientHeight(), 0.0f, 1.0f);
	mRHI->RSSetScissorRects(0, 0, Engine::GetEngine()->GetWindow()->GetClientWidht(), Engine::GetEngine()->GetWindow()->GetClientHeight());
	mRHI->ResourceBarrier(1, DX_RESOURCE_STATES::PRESENT,DX_RESOURCE_STATES::RENDER_TARGET);
	mRHI->ClearRenderTargetView();
	mRHI->ClearDepthStencilView();
	mRHI->OMSetStencilRef(0);
	mRHI->OMSetRenderTargets(1,true);


	for (auto&& Actor : SceneManager::GetSceneManager()->GetAllActor())
	{
		mRHI->SetDescriptorHeaps(Actor.first);
		mRHI->SetGraphicsRootSignature();
		mRHI->IASetVertexBuffers(mRHI->CreateBuffer(mRenderResource.get()));
		mRHI->IASetIndexBuffer(mRHI->CreateBuffer(mRenderResource.get()));
		mRHI->IASetPrimitiveTopology();
		mRHI->SetGraphicsRootDescriptorTable(Actor.first);
		mRHI->SetGraphicsRoot32BitConstants();
		mRHI->DrawIndexedInstanced(Actor.first);
	}
	mRHI->ResourceBarrier(1, DX_RESOURCE_STATES::RENDER_TARGET, DX_RESOURCE_STATES::PRESENT);
	mRHI->UpdateMVP(gt);
	mRHI->ExecuteCommandLists();
}

FRender::~FRender()
{
	if (mRHI != nullptr) {
		mRHI = nullptr;
	}
}
