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
	mRenderResource = std::make_shared<FRenderScene>();
	if (!mRHI->Initialize()) {
		return false;
	}
	mShadowResource = RHIFactory->CreateShadowResource();
	return true;
}


void FRender::Render()
{
	SceneRender();
}

void FRender::RenderInit()
{
	int CBIndex = 0;

	for (auto&& texture : AssetManager::GetAssetManager()->GetTextures()) {
		mRHI->LoadTexture(texture.get());
	}
	mRHI->ResetCommand("Null");
	for (auto&& Actor : SceneManager::GetSceneManager()->GetAllActor())
	{
		mRHI->RenderFrameBegin(mRenderResource, Actor.first, CBIndex);
		CBIndex++;
	}
	mRHI->CreateShader( L"..\\JEngine\\Shaders\\color.hlsl");
	mRHI->CreateShader( L"..\\JEngine\\Shaders\\Shadow.hlsl");


	for (auto&& RenderItem : mRenderResource->mRenderItem)
	{
		mRHI->ChangePSOState(RenderItem.second.get(), "ShadowMap");
		mRHI->SetPipelineState(RenderItem.second);
	}

	for (auto&& actorPair : SceneManager::GetSceneManager()->GetAllActor())
	{
		mRHI->CreateCbHeapsAndSrv(actorPair.first, actorPair.second, mShadowResource.get(), mRenderResource);
	}
	mRHI->ExecuteCommandLists();
	mRHI->IsRunDrawPrepare = false;
}

void FRender::SceneRender()
{
	mRHI->ResetCommand("Scene");
	BuildLight(mRenderResource);
	BuildRenderItemTrans(mRenderResource);
	int CBIndex = 0;
	for (auto&& Actor : SceneManager::GetSceneManager()->GetAllActor())
	{
		//RenderFrameBegin
		mRHI->RenderFrameBegin(mRenderResource, Actor.first, CBIndex);
		CBIndex++;
	}
	//SetViewportAndScissorRect
	mRHI->RSSetViewports(0.0f, 0.0f, (float)Engine::GetEngine()->GetWindow()->GetClientWidht(), (float)Engine::GetEngine()->GetWindow()->GetClientHeight(), 0.0f, 1.0f);
	mRHI->RSSetScissorRects(0, 0, Engine::GetEngine()->GetWindow()->GetClientWidht(), Engine::GetEngine()->GetWindow()->GetClientHeight());
	DepthPass();
	BasePass();
	//RenderFrameEnd
	mRHI->ExecuteCommandLists();
}

void FRender::DepthPass()
{
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FShadowResource>(mShadowResource)->GetResource(), DX_RESOURCE_STATES::RESOURCE_STATE_GENERIC_READ, DX_RESOURCE_STATES::DEPTH_WRITE);
	//SetRenderTatget
	mRHI->ClearAndSetRenderTatget(0, std::dynamic_pointer_cast<FShadowResource>(mShadowResource)->DSV(),
		0, 0, false, std::dynamic_pointer_cast<FShadowResource>(mShadowResource)->DSV());
	for (auto&& RenderItem : mRenderResource->mRenderItem)
	{
		mRHI->ChangePSOState(RenderItem.second.get(), "ShadowMap");
		mRHI->SetPipelineState(RenderItem.second);
		mRHI->DrawMesh(mRenderResource, RenderItem.first, true);
	}
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FShadowResource>(mShadowResource)->GetResource(), DX_RESOURCE_STATES::DEPTH_WRITE, DX_RESOURCE_STATES::RESOURCE_STATE_GENERIC_READ);
}

void FRender::BasePass()
{
	mRHI->ResourceBarrier(1, mRHIResource->BackBuffer(), DX_RESOURCE_STATES::PRESENT, DX_RESOURCE_STATES::RENDER_TARGET);
	//ClearAndSetRenderTatget
	mRHI->ClearAndSetRenderTatget(mRHIResource->CurrentBackBufferViewHand(), mRHIResource->CurrentDepthStencilViewHand(),
		1, mRHIResource->CurrentBackBufferViewHand(), true, mRHIResource->CurrentDepthStencilViewHand());
	//DrawMesh
	for (auto&& RenderItem : mRenderResource->mRenderItem)
	{
		mRHI->ChangePSOState(RenderItem.second.get(), "Scene");
		mRHI->SetPipelineState(RenderItem.second);
		mRHI->DrawMesh(mRenderResource, RenderItem.first, false);
	}
	mRHI->ResourceBarrier(1, mRHIResource->BackBuffer(), DX_RESOURCE_STATES::RENDER_TARGET, DX_RESOURCE_STATES::PRESENT);
}



void FRender::BuildLight(std::shared_ptr<FRenderScene> sceneResource)
{
	float Radius = 2500;
	glm::vec3 lightPos = -2.0f * Radius * SceneManager::GetSceneManager()->DirectionalLight.Direction;

	glm::mat4x4 lightView = glm::lookAtLH(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec3 sphereCenterLS = MathHelper::Vector3TransformCoord(glm::vec3(0.0f, 0.0f, 0.0f), lightView);

	float l = sphereCenterLS.x - Radius;
	float b = sphereCenterLS.y - Radius;
	float n = sphereCenterLS.z - Radius;
	float r = sphereCenterLS.x + Radius;
	float t = sphereCenterLS.y + Radius;
	float f = sphereCenterLS.z + Radius;
	glm::mat4x4 lightProj = glm::orthoLH_ZO(l, r, b, t, n, f);
	glm::mat4 T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	sceneResource->LightViewProj = glm::transpose(lightProj * lightView);
	sceneResource->TLightViewProj = glm::transpose(T * lightProj * lightView);
}

void FRender::BuildRenderItemTrans(std::shared_ptr<FRenderScene> sceneResource)
{
	for (auto&& Actor : SceneManager::GetSceneManager()->GetAllActor())
	{
		glm::qua<float> q = glm::qua<float>(
			Actor.second->Transform[0].Rotation.w,
			Actor.second->Transform[0].Rotation.x,
			Actor.second->Transform[0].Rotation.y,
			Actor.second->Transform[0].Rotation.z
			);
		glm::vec3 location = glm::vec3(
			Actor.second->Transform[0].Location.x,
			Actor.second->Transform[0].Location.y,
			Actor.second->Transform[0].Location.z
		);
		glm::vec3 actorScale = glm::vec3(
			Actor.second->Transform[0].Scale3D.x,
			Actor.second->Transform[0].Scale3D.y,
			Actor.second->Transform[0].Scale3D.z
		);

		glm::mat4x4 Rotation = glm::mat4_cast(q);
		glm::mat4x4 Translate = glm::identity<glm::mat4x4>();
		Translate = glm::translate(Translate, location);
		glm::mat4x4 Scale = glm::identity<glm::mat4x4>();
		Scale = glm::scale(Scale, actorScale);

		glm::mat4x4 W = Translate * Rotation * Scale;
		sceneResource->mRenderItem[Actor.first]->World = glm::transpose(W * mWorld);
	}
}

FRender::~FRender()
{
	if (mRHI != nullptr) {
		mRHI = nullptr;
	}
}
