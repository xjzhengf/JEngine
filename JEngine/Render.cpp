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

	mRHI->RenderFrameBegin(mRenderScene, "ShadowMap");
	
	for (auto&& RenderItem : mRenderScene->mRenderItem)
	{
		mRHI->ChangePSOState(MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap"), MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap").mPso, MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap").GlobalShader);
		mRHI->SetPipelineState(RenderItem.second,MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap"));
	}

	for (auto&& actorPair : SceneManager::GetSceneManager()->GetAllActor())                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
	{
		mRHI->CreateCbHeapsAndSrv(actorPair.first, actorPair.second->StaticMeshAssetName[0], mRenderScene->mRenderItem[actorPair.first].get(), mShadowResource.get(),mHDRResource.get(), mRenderScene);
	}
	mRHI->CreateCbHeapsAndSrv("HDRTriangle","HDRTriangle", mRenderScene->HDRTriangle.get(), mShadowResource.get(), mHDRResource.get(), mRenderScene);
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
		//mRHI->RenderFrameBegin(mRenderScene, Actor.first, CBIndex, "ShadowMap");
		mRHI->UpdateCB(mRenderScene, mRenderScene->mRenderItem[Actor.first].get(),Actor.first, CBIndex, MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap"));
		CBIndex++;
	}
	mRHI->UpdateCB(mRenderScene, mRenderScene->HDRTriangle.get(), "HDRTriangle", CBIndex, MaterialManager::GetMaterialManager()->SearchMaterial("ShadowMap"));
	//SetViewportAndScissorRect
	DepthPass();
	HDRPass();
	BloomPass(1,"BloomSet");
    BloomPass(2, "BloomDown");
    BloomPass(3, "BloomDown");
    BloomPass(4, "BloomUp");
    BloomPass(5, "SunMerge");
	ToneMapPass();

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
		mRHI->DrawMesh(RenderItem.second, RenderItem.first, true,false,0, 1024, 768);
	}
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FShadowResource>(mShadowResource)->GetResource(), RESOURCE_STATES::DEPTH_WRITE, RESOURCE_STATES::RESOURCE_STATE_GENERIC_READ);
}

void FRender::BloomPass(int index,const std::string& PSOName)
{
	mRHI->RSSetViewports(0.0f, 0.0f, (float)Engine::GetEngine()->GetWindow()->GetClientWidht(), (float)Engine::GetEngine()->GetWindow()->GetClientHeight(), 0.0f, 1.0f);
	mRHI->RSSetScissorRects(0, 0, Engine::GetEngine()->GetWindow()->GetClientWidht(), Engine::GetEngine()->GetWindow()->GetClientHeight());

	//mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->GetDSVResource(index), RESOURCE_STATES::RESOURCE_STATE_GENERIC_READ, RESOURCE_STATES::DEPTH_WRITE);
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->GetRTVResource(index), RESOURCE_STATES::COMMON, RESOURCE_STATES::RENDER_TARGET);
	//ClearAndSetRenderTatget

	mRHI->ClearAndSetRenderTatget(std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->RTV(index), std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->DSV(index),
		1, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->RTV(index), true, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->DSV(index));
	//DrawMesh
	
	mRHI->ChangePSOState(MaterialManager::GetMaterialManager()->SearchMaterial(mRenderScene->HDRTriangle->MatName), MaterialManager::GetMaterialManager()->SearchMaterial(PSOName).mPso, MaterialManager::GetMaterialManager()->SearchMaterial(PSOName).GlobalShader);
	mRHI->SetPipelineState(mRenderScene->HDRTriangle, MaterialManager::GetMaterialManager()->SearchMaterial(PSOName));

	mRHI->DrawMesh(mRenderScene->HDRTriangle, "HDRTriangle", false, true, index,std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->width[index], std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->height[index]);
	
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->GetRTVResource(index), RESOURCE_STATES::RENDER_TARGET, RESOURCE_STATES::COMMON);
	//mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->GetDSVResource(index), RESOURCE_STATES::DEPTH_WRITE, RESOURCE_STATES::RESOURCE_STATE_GENERIC_READ);
}

void FRender::HDRPass()
{
	mRHI->RSSetViewports(0.0f, 0.0f, (float)Engine::GetEngine()->GetWindow()->GetClientWidht(), (float)Engine::GetEngine()->GetWindow()->GetClientHeight(), 0.0f, 1.0f);
	mRHI->RSSetScissorRects(0, 0, Engine::GetEngine()->GetWindow()->GetClientWidht(), Engine::GetEngine()->GetWindow()->GetClientHeight());
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->GetRTVResource(0), RESOURCE_STATES::COMMON, RESOURCE_STATES::RENDER_TARGET);
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->GetDSVResource(0), RESOURCE_STATES::COMMON, RESOURCE_STATES::DEPTH_WRITE);
	//SetRenderTatget
	mRHI->ClearAndSetRenderTatget(std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->RTV(0), std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->DSV(0),
		1, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->RTV(0), true, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->DSV(0));
	for (auto&& RenderItem : mRenderScene->mRenderItem)
	{
		mRHI->ChangePSOState(MaterialManager::GetMaterialManager()->SearchMaterial(RenderItem.second->MatName), MaterialManager::GetMaterialManager()->SearchMaterial("Bloom").mPso, MaterialManager::GetMaterialManager()->SearchMaterial("Bloom").GlobalShader);
		mRHI->SetPipelineState(RenderItem.second, MaterialManager::GetMaterialManager()->SearchMaterial("Bloom"));
		mRHI->DrawMesh(RenderItem.second, RenderItem.first, false,false, 0,1024,768);
	}
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->GetRTVResource(0), RESOURCE_STATES::RENDER_TARGET, RESOURCE_STATES::COMMON);
	mRHI->ResourceBarrier(1, std::dynamic_pointer_cast<FHDRResource>(mHDRResource)->GetDSVResource(0), RESOURCE_STATES::DEPTH_WRITE, RESOURCE_STATES::COMMON);
}

void FRender::ToneMapPass()
{
	mRHI->RSSetViewports(0.0f, 0.0f, (float)Engine::GetEngine()->GetWindow()->GetClientWidht(), (float)Engine::GetEngine()->GetWindow()->GetClientHeight(), 0.0f, 1.0f);
	mRHI->RSSetScissorRects(0, 0, Engine::GetEngine()->GetWindow()->GetClientWidht(), Engine::GetEngine()->GetWindow()->GetClientHeight());

	mRHI->ResourceBarrier(1, mRHIResource->BackBuffer(), RESOURCE_STATES::PRESENT, RESOURCE_STATES::RENDER_TARGET);
	//ClearAndSetRenderTatget
	mRHI->ClearAndSetRenderTatget(mRHIResource->CurrentBackBufferViewHand(), mRHIResource->CurrentDepthStencilViewHand(),
		1, mRHIResource->CurrentBackBufferViewHand(), true, mRHIResource->CurrentDepthStencilViewHand());
	//DrawMesh

	mRHI->ChangePSOState(MaterialManager::GetMaterialManager()->SearchMaterial("ToneMap"), MaterialManager::GetMaterialManager()->SearchMaterial("ToneMap").mPso, MaterialManager::GetMaterialManager()->SearchMaterial("ToneMap").GlobalShader);
	mRHI->SetPipelineState(mRenderScene->HDRTriangle, MaterialManager::GetMaterialManager()->SearchMaterial("ToneMap"));
	mRHI->DrawMesh(mRenderScene->HDRTriangle, "HDRTriangle", false, true, 6, 1024, 768);
	mRHI->ResourceBarrier(1, mRHIResource->BackBuffer(), RESOURCE_STATES::RENDER_TARGET, RESOURCE_STATES::PRESENT);
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
