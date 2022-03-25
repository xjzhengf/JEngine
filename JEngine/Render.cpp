#include "stdafx.h"
#include "Render.h"
#include "DXRHIResource.h"
#include "FShadowResource.h"
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
	mShadowResource = RHIFactory->CreateShadowResource();
	return true;
}


void FRender::Render(const GameTimer& gt)
{
	SceneRender(gt);
}

void FRender::RenderBegin()
{
	for (auto&& texture : AssetManager::GetAssetManager()->GetTextures()) {
		mRHI->LoadTexture(texture.get());
	}
	mRHI->CreateShader(mRHIResource.get(), L"..\\JEngine\\Shaders\\color.hlsl");
	mRHI->DrawPrepare(mRHIResource.get(),mShadowResource.get());
	mRHI->BuildPSO(mRHIResource.get(), "Scene");
	mRHI->CreateShader(mRHIResource.get(), L"..\\JEngine\\Shaders\\Shadow.hlsl");
	mRHI->BuildPSO(mRHIResource.get(), "ShadowMap");
	mRHI->ExecuteCommandLists();
	mRHI->IsRunDrawPrepare = false;
}

void FRender::SceneRender(const GameTimer& gt)
{
	mRHI->ResetCommand("Scene"); 
	DepthRender(gt);
	mRHI->RSSetViewports(0.0f, 0.0f, (float)Engine::GetEngine()->GetWindow()->GetClientWidht(), (float)Engine::GetEngine()->GetWindow()->GetClientHeight(), 0.0f, 1.0f);
	mRHI->RSSetScissorRects(0, 0, Engine::GetEngine()->GetWindow()->GetClientWidht(), Engine::GetEngine()->GetWindow()->GetClientHeight());
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<DXRHIResource>(mRHIResource)->BackBuffer(), DX_RESOURCE_STATES::PRESENT, DX_RESOURCE_STATES::RENDER_TARGET);
	mRHI->ClearRenderTargetView(std::dynamic_pointer_cast<DXRHIResource>(mRHIResource)->CurrentBackBufferView());
	mRHI->ClearDepthStencilView(std::dynamic_pointer_cast<DXRHIResource>(mRHIResource)->CurrentDepthStencilView());
	mRHI->OMSetStencilRef(0);
	mRHI->OMSetRenderTargets(1, std::dynamic_pointer_cast<DXRHIResource>(mRHIResource)->CurrentBackBufferView(), true, std::dynamic_pointer_cast<DXRHIResource>(mRHIResource)->CurrentDepthStencilView());
	mRHI->SetPipelineState("Scene");
	mRHI->UpdateMVP(gt);
	for (auto&& Actor : SceneManager::GetSceneManager()->GetAllActor())
	{
		mRHI->SetDescriptorHeaps(Actor.first);
		mRHI->SetGraphicsRootSignature();
		mRHI->IASetVertexBuffers(mRHI->CreateBuffer(mRenderResource.get()));
		mRHI->IASetIndexBuffer(mRHI->CreateBuffer(mRenderResource.get()));
		mRHI->IASetPrimitiveTopology();
		mRHI->SetGraphicsRootDescriptorTable(Actor.first,false);
		mRHI->SetGraphicsRoot32BitConstants();
		mRHI->DrawIndexedInstanced(Actor.first);
	}
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<DXRHIResource>(mRHIResource)->BackBuffer(), DX_RESOURCE_STATES::RENDER_TARGET, DX_RESOURCE_STATES::PRESENT);
	
	mRHI->ExecuteCommandLists();
}

void FRender::DepthRender(const GameTimer& gt)
{

	mRHI->RSSetViewports(0.0f, 0.0f, (float)Engine::GetEngine()->GetWindow()->GetClientWidht(), (float)Engine::GetEngine()->GetWindow()->GetClientHeight(), 0.0f, 1.0f);
	mRHI->RSSetScissorRects(0, 0, Engine::GetEngine()->GetWindow()->GetClientWidht(), Engine::GetEngine()->GetWindow()->GetClientHeight());
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<DXShadowResource>(mShadowResource)->GetResource(), DX_RESOURCE_STATES::RESOURCE_STATE_GENERIC_READ, DX_RESOURCE_STATES::DEPTH_WRITE);
	mRHI->ClearDepthStencilView(std::dynamic_pointer_cast<DXShadowResource>(mShadowResource)->DSV().ptr);
	mRHI->OMSetRenderTargets(0, 0, false, std::dynamic_pointer_cast<DXShadowResource>(mShadowResource)->DSV().ptr);
	mRHI->SetPipelineState("ShadowMap");
	mRHI->UpdateMVP(gt);
	for (auto&& Actor : SceneManager::GetSceneManager()->GetAllActor())
	{

		mRHI->SetDescriptorHeaps(Actor.first);
		mRHI->SetGraphicsRootSignature();
		mRHI->IASetVertexBuffers(mRHI->CreateBuffer(mRenderResource.get()));
		mRHI->IASetIndexBuffer(mRHI->CreateBuffer(mRenderResource.get()));
		mRHI->IASetPrimitiveTopology();
		mRHI->SetGraphicsRootDescriptorTable(Actor.first,true);
		mRHI->SetGraphicsRoot32BitConstants();
		mRHI->DrawIndexedInstanced(Actor.first);
	}
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<DXShadowResource>(mShadowResource)->GetResource(), DX_RESOURCE_STATES::DEPTH_WRITE, DX_RESOURCE_STATES::RESOURCE_STATE_GENERIC_READ);
	
}

FRender::~FRender()
{
	if (mRHI != nullptr) {
		mRHI = nullptr;
	}
}
