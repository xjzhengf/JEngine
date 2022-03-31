#include "stdafx.h"
#include "Render.h"
#include "DXRHIResource.h"
#include "FShadowResource.h"
#include "Engine.h"
#include "FRenderScene.h"
#include "SceneManager.h"
#include "AssetManager.h"
bool FRender::Init()
{
	RHIFactory = std::make_unique<FRHIFactory>();
	mRHI = RHIFactory->CreateRHI();
	mRHIResource = RHIFactory->CreateRHIResource();
	mRenderResource = std::make_shared<FRenderScene>();
	if (!mRHI->Initialize()) {
		return false;
	}
	mShadowResource = RHIFactory->CreateShadowResource();
	return true;
}


void FRender::Render(const GameTimer& gt)
{
	SceneRender(gt);
}

void FRender::RenderInit()
{
	for (auto&& texture : AssetManager::GetAssetManager()->GetTextures()) {
		mRHI->LoadTexture(texture.get());
	}
	mRHI->CreateShader( L"..\\JEngine\\Shaders\\color.hlsl");
	mRHI->BuildPSO(mRHIResource.get(), "Scene");
	mRHI->CreateShader( L"..\\JEngine\\Shaders\\Shadow.hlsl");
	mRHI->BuildPSO(mRHIResource.get(), "ShadowMap");
	mRHI->DrawPrepare();
	for (auto&& actorPair : SceneManager::GetSceneManager()->GetAllActor())
	{
		mRHI->CreateCbHeapsAndSrv(actorPair.first, actorPair.second, mShadowResource.get(),mRenderResource);
	}
	mRHI->ExecuteCommandLists();
	mRHI->IsRunDrawPrepare = false;
}

void FRender::SceneRender(const GameTimer& gt)
{
	mRHI->ResetCommand("Scene"); 
	mRHI->BuildLight(mRenderResource);
	int CBIndex = 0;
	for (auto&& Actor : SceneManager::GetSceneManager()->GetAllActor())
	{
		mRHI->BuildRenderItem(mRenderResource, Actor.second, Actor.first);
		mRHI->UpdateCB(gt, mRenderResource, Actor.first, CBIndex);
		CBIndex++;
	}
	DepthRender(gt);
	mRHI->RSSetViewports(0.0f, 0.0f, (float)Engine::GetEngine()->GetWindow()->GetClientWidht(), (float)Engine::GetEngine()->GetWindow()->GetClientHeight(), 0.0f, 1.0f);
	mRHI->RSSetScissorRects(0, 0, Engine::GetEngine()->GetWindow()->GetClientWidht(), Engine::GetEngine()->GetWindow()->GetClientHeight());
	mRHI->ResourceBarrier(1, mRHIResource->BackBuffer(), DX_RESOURCE_STATES::PRESENT, DX_RESOURCE_STATES::RENDER_TARGET);
	mRHI->ClearRenderTargetView(mRHIResource->CurrentBackBufferViewHand());
	mRHI->ClearDepthStencilView(mRHIResource->CurrentDepthStencilViewHand());
	mRHI->OMSetStencilRef(0);
	mRHI->OMSetRenderTargets(1, mRHIResource->CurrentBackBufferViewHand(), true, mRHIResource->CurrentDepthStencilViewHand());
	mRHI->SetPipelineState("Scene");

	for (auto&& Actor : SceneManager::GetSceneManager()->GetAllActor())
	{
		mRHI->SetDescriptorHeaps(Actor.first);
		mRHI->SetGraphicsRootSignature();
		mRHI->IASetVertexAndIndexBuffers(mRHI->CreateBuffer(mRenderResource,Actor.first));
		mRHI->IASetPrimitiveTopology();
		mRHI->SetGraphicsRootDescriptorTable(std::dynamic_pointer_cast<FRenderScene>(mRenderResource)->mRenderItem[Actor.first].get(),false);
		mRHI->SetGraphicsRoot32BitConstants();
		mRHI->DrawIndexedInstanced(mRenderResource,Actor.first);
	}
	mRHI->ResourceBarrier(1, mRHIResource->BackBuffer(), DX_RESOURCE_STATES::RENDER_TARGET, DX_RESOURCE_STATES::PRESENT);
	mRHI->ExecuteCommandLists();
}

void FRender::DepthRender(const GameTimer& gt)
{

	mRHI->RSSetViewports(0.0f, 0.0f, (float)Engine::GetEngine()->GetWindow()->GetClientWidht(), (float)Engine::GetEngine()->GetWindow()->GetClientHeight(), 0.0f, 1.0f);
	mRHI->RSSetScissorRects(0, 0, Engine::GetEngine()->GetWindow()->GetClientWidht(), Engine::GetEngine()->GetWindow()->GetClientHeight());
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FShadowResource>(mShadowResource)->GetResource(), DX_RESOURCE_STATES::RESOURCE_STATE_GENERIC_READ, DX_RESOURCE_STATES::DEPTH_WRITE);
	mRHI->ClearDepthStencilView(std::dynamic_pointer_cast<FShadowResource>(mShadowResource)->DSV());
	mRHI->OMSetRenderTargets(0, 0, false, std::dynamic_pointer_cast<FShadowResource>(mShadowResource)->DSV());
	mRHI->SetPipelineState("ShadowMap");
	for (auto&& Actor : SceneManager::GetSceneManager()->GetAllActor())
	{
		mRHI->SetDescriptorHeaps(Actor.first);
		mRHI->SetGraphicsRootSignature();
		mRHI->IASetVertexAndIndexBuffers(mRHI->CreateBuffer(mRenderResource, Actor.first));
		mRHI->IASetPrimitiveTopology();
		mRHI->SetGraphicsRootDescriptorTable(std::dynamic_pointer_cast<FRenderScene>(mRenderResource)->mRenderItem[Actor.first].get(), true);
		mRHI->SetGraphicsRoot32BitConstants();
		mRHI->DrawIndexedInstanced(mRenderResource,Actor.first);
	}
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FShadowResource>(mShadowResource)->GetResource(), DX_RESOURCE_STATES::DEPTH_WRITE, DX_RESOURCE_STATES::RESOURCE_STATE_GENERIC_READ);
	
}

FRender::~FRender()
{
	if (mRHI != nullptr) {
		mRHI = nullptr;
	}
}
