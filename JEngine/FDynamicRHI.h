#pragma once
#include "stdafx.h"
#include "RHIResource.h"
#include "FRenderScene.h"
#include "FTexture.h"
#include "Buffer.h"
class FDynamicRHI
{
public:
	FDynamicRHI();
	virtual ~FDynamicRHI() {};
	FDynamicRHI* GetRHI();
	void DestroyRHI();
	static FDynamicRHI* mRHI;

	bool IsRunDrawPrepare = true;
public:
	virtual bool Initialize() = 0;
	virtual void BuildPSO(std::shared_ptr<RenderItem> renderItem) = 0;
	virtual void ResetCommand(const std::string& PSOName) = 0;
	virtual void RSSetViewports(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth) = 0;
	virtual void RSSetScissorRects(long left, long top, long right, long bottom) = 0;
	virtual void ResourceBarrier(unsigned int NumberBarrier, std::shared_ptr<FResource> Resource, int stateBefore, int stateAfter) {};

	virtual void SetPipelineState(std::shared_ptr<RenderItem> renderItem) = 0;
	virtual void ChangePSOState(RenderItem* renderItem,const std::string& PSOName) = 0;
	virtual void ExecuteCommandLists() = 0;
	virtual void DrawPrepare(std::shared_ptr<RenderItem> renderItem) = 0;
	virtual void CreateTextureResource(std::shared_ptr<FRenderScene> renderResource, FTexture* TextureResource, bool isNormal) = 0;

	virtual void RenderFrameBegin(std::shared_ptr<FRenderScene> renderResource, const std::string& ActorName,int RenderItemIndex)=0;
	virtual void ClearAndSetRenderTatget(unsigned __int64 ClearRenderTargetHand, unsigned __int64 ClearDepthStencilHand, int numTatgetDescriptors, unsigned __int64 SetRenderTargetHand, bool RTsSingleHandleToDescriptorRange, unsigned __int64 SetDepthStencilHand) = 0;
	virtual void DrawMesh(std::shared_ptr<FRenderScene> renderResource, const std::string& renderItemName, bool IsDrawDepth) = 0;

public:
	virtual Buffer* CreateBuffer(std::shared_ptr<FRenderScene> renderResource, const std::string& Name) = 0;
	virtual void CreateShader( const std::wstring& filename)=0;
	virtual void CreateCbHeapsAndSrv(const std::string& ActorName, ActorStruct* Actor, FRenderResource* shadowResource, std::shared_ptr<FRenderScene> sceneResource) {};
};
