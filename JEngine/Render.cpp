#include "stdafx.h"
#include "Render.h"
#include "DXRHIResource.h"
#include "FShadowResource.h"
#include "FHDRResource.h"
#include "Engine.h"
#include "SceneManager.h"
#include "AssetManager.h"
bool FRender::Init()
{
	RHIFactory = std::make_unique<FRHIFactory>();
	mRHI = RHIFactory->CreateRHI();
	mRHIResource = RHIFactory->CreateRHIResource();
	mRenderScene = std::make_shared<FRenderScene>();
	if (!mRHI->Initialize()) {
		return false;
	}
	mShadowResource = RHIFactory->CreateShadowResource();
	mHDRResource = RHIFactory->CreateHDRResource();
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
		mRHI->CreateTextureResource(mRenderScene, texture.get(), false);
	}
	for (auto&& normalTexture : AssetManager::GetAssetManager()->GetNormalTextures()) {
		mRHI->CreateTextureResource(mRenderScene, normalTexture.get(), true);
	}

	mRHI->ResetCommand("Null");
	for (auto&& Actor : SceneManager::GetSceneManager()->GetAllActor())
	{
		mRHI->RenderFrameBegin(mRenderScene, Actor.first, CBIndex, "ShadowMap");
		CBIndex++;
	}
	mRHI->CreateShader(L"..\\JEngine\\Shaders\\HightLight.hlsl");
	mRHI->CreateShader( L"..\\JEngine\\Shaders\\color.hlsl");
	mRHI->CreateShader( L"..\\JEngine\\Shaders\\Shadow.hlsl");



	for (auto&& RenderItem : mRenderScene->mRenderItem)
	{
		mRHI->ChangePSOState(MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap"), MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap").mPso, MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap").GlobalShader);
		mRHI->SetPipelineState(RenderItem.second,MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap"));
	}

	for (auto&& actorPair : SceneManager::GetSceneManager()->GetAllActor())                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
	{
		mRHI->CreateCbHeapsAndSrv(actorPair.first, actorPair.second, mShadowResource.get(),mHDRResource.get(), mRenderScene);
	}
	mRHI->ExecuteCommandLists();
	mRHI->IsRunDrawPrepare = false;
} 


void FRender::SceneRender()
{
	mRHI->ResetCommand("Scene");
	BuildLight(mRenderScene);
	BuildRenderItemTrans(mRenderScene);
	int CBIndex = 0;
	for (auto&& Actor : SceneManager::GetSceneManager()->GetAllActor())
	{
		//RenderFrameBegin
		mRHI->RenderFrameBegin(mRenderScene, Actor.first, CBIndex, "ShadowMap");
		mRHI->UpdateCB(mRenderScene, Actor.first, CBIndex, MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap"));
		CBIndex++;
	}
	//SetViewportAndScissorRect
	DepthPass();
	HDRPass();
	BasePass();

	//RenderFrameEnd
	mRHI->ExecuteCommandLists();
}

void FRender::DepthPass()
{
	mRHI->RSSetViewports(0.0f, 0.0f, 2048, 2048, 0.0f, 1.0f);
	mRHI->RSSetScissorRects(0, 0, 2048, 2048);
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FShadowResource>(mShadowResource)->GetResource(), RESOURCE_STATES::RESOURCE_STATE_GENERIC_READ, RESOURCE_STATES::DEPTH_WRITE);
	//SetRenderTatget
	mRHI->ClearAndSetRenderTatget(0, std::dynamic_pointer_cast<FShadowResource>(mShadowResource)->DSV(),
		0, 0, false, std::dynamic_pointer_cast<FShadowResource>(mShadowResource)->DSV());
	for (auto&& RenderItem : mRenderScene->mRenderItem)
	{
		mRHI->ChangePSOState(MaterialManager::GetMaterialManager()->SearchMaterial(RenderItem.second->MatName), MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap").mPso, MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap").GlobalShader);
		mRHI->SetPipelineState(RenderItem.second,MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap"));
		mRHI->DrawMesh(mRenderScene, RenderItem.first, true);
	}
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FShadowResource>(mShadowResource)->GetResource(), RESOURCE_STATES::DEPTH_WRITE, RESOURCE_STATES::RESOURCE_STATE_GENERIC_READ);
}

void FRender::BasePass()
{
	mRHI->RSSetViewports(0.0f, 0.0f, (float)Engine::GetEngine()->GetWindow()->GetClientWidht(), (float)Engine::GetEngine()->GetWindow()->GetClientHeight(), 0.0f, 1.0f);
	mRHI->RSSetScissorRects(0, 0, Engine::GetEngine()->GetWindow()->GetClientWidht(), Engine::GetEngine()->GetWindow()->GetClientHeight());

	mRHI->ResourceBarrier(1, mRHIResource->BackBuffer(), RESOURCE_STATES::PRESENT, RESOURCE_STATES::RENDER_TARGET);
	//ClearAndSetRenderTatget
	mRHI->ClearAndSetRenderTatget(mRHIResource->CurrentBackBufferViewHand(), mRHIResource->CurrentDepthStencilViewHand(),
		1, mRHIResource->CurrentBackBufferViewHand(), true, mRHIResource->CurrentDepthStencilViewHand());
	//DrawMesh
	for (auto&& RenderItem : mRenderScene->mRenderItem)
	{
		mRHI->ChangePSOState(MaterialManager::GetMaterialManager()->SearchMaterial(RenderItem.second->MatName), MaterialManager::GetMaterialManager()->SearchMaterial("Scene").mPso, MaterialManager::GetMaterialManager()->SearchMaterial("Scene").GlobalShader);
		mRHI->SetPipelineState(RenderItem.second, MaterialManager::GetMaterialManager()->SearchMaterial("Scene"));
		mRHI->DrawMesh(mRenderScene, RenderItem.first, false);
	}
	mRHI->ResourceBarrier(1, mRHIResource->BackBuffer(), RESOURCE_STATES::RENDER_TARGET, RESOURCE_STATES::PRESENT);
}

void FRender::HDRPass()
{
	mRHI->RSSetViewports(0.0f, 0.0f, (float)Engine::GetEngine()->GetWindow()->GetClientWidht(), (float)Engine::GetEngine()->GetWindow()->GetClientHeight(), 0.0f, 1.0f);
	mRHI->RSSetScissorRects(0, 0, Engine::GetEngine()->GetWindow()->GetClientWidht(), Engine::GetEngine()->GetWindow()->GetClientHeight());

	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->GetResource(), RESOURCE_STATES::COMMON, RESOURCE_STATES::RENDER_TARGET);
	//SetRenderTatget
	mRHI->ClearAndSetRenderTatget(std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->RTV(), std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->DSV(),
		1, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->RTV(), true, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->DSV());
	for (auto&& RenderItem : mRenderScene->mRenderItem)
	{
		mRHI->ChangePSOState(MaterialManager::GetMaterialManager()->SearchMaterial(RenderItem.second->MatName), MaterialManager::GetMaterialManager()->SearchMaterial("Bloom").mPso, MaterialManager::GetMaterialManager()->SearchMaterial("Bloom").GlobalShader);
		mRHI->SetPipelineState(RenderItem.second, MaterialManager::GetMaterialManager()->SearchMaterial("Bloom"));
		mRHI->DrawMesh(mRenderScene, RenderItem.first, false);
	}
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->GetResource(), RESOURCE_STATES::RENDER_TARGET, RESOURCE_STATES::COMMON);

}



void FRender::BuildLight(std::shared_ptr<FRenderScene> sceneResource)
{
	float Radius = 2500;
	glm::vec3 direction = SceneManager::GetSceneManager()->DirectionalLight.Direction;
	glm::vec3 lightPos = -2.0f * Radius * SceneManager::GetSceneManager()->DirectionalLight.Direction;
	float Time =Engine::GetEngine()->Time/3;
	direction.x = direction.x * glm::cos(Time) - direction.y * glm::sin(Time);
	direction.y = direction.y * glm::cos(Time) + direction.x * glm::sin(Time);
	lightPos.x = lightPos.x * glm::cos(Time) - lightPos.y * glm::sin(Time);
	lightPos.y = lightPos.y * glm::cos(Time) + lightPos.x * glm::sin(Time);
	sceneResource->LightDirection = direction;
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
		sceneResource->mRenderItem[Actor.first]->Rotation = glm::transpose(Rotation);
		sceneResource->mRenderItem[Actor.first]->Scale = glm::transpose(Scale);
		sceneResource->mRenderItem[Actor.first]->World = glm::transpose(W * mWorld);
	}
}

FRender::~FRender()
{
	if (mRHI != nullptr) {
		mRHI = nullptr;
	}
}
