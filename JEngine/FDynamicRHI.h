#pragma once
#include "stdafx.h"
#include "RHIResource.h"
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
public:
	virtual bool Initialize() = 0;

	virtual void BuildPSO(FRHIResource* RHIResource) = 0;

	virtual void ResetCommand() = 0;
	virtual void RSSetViewports(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth) = 0;
	virtual void RSSetScissorRects(long left, long top, long right, long bottom) = 0;
	virtual void ResourceBarrier(unsigned int NumberBarrier, int stateBefore, int stateAfter) {};
	virtual void ClearRenderTargetView() = 0;
	virtual void ClearDepthStencilView() = 0;
	virtual void OMSetStencilRef(int StencilRef) = 0;
	virtual void OMSetRenderTargets(int numTatgetDescriptors, bool RTsSingleHandleToDescriptorRange) = 0;
	virtual void SetDescriptorHeaps(std::string Name) = 0;
	virtual void SetGraphicsRootSignature() = 0;
	virtual void IASetVertexBuffers(Buffer* buffer) = 0;
	virtual void IASetIndexBuffer(Buffer* buffer) = 0;
	virtual void IASetPrimitiveTopology() = 0;
	virtual void Offset(std::string Name) = 0;
	virtual void SetGraphicsRootDescriptorTable(std::string Name) {};
	virtual void SetGraphicsRoot32BitConstants() = 0;
	virtual void DrawIndexedInstanced(std::string Name) = 0;
	virtual void ExecuteCommandLists() = 0;

	virtual void DrawPrepare() = 0;
	virtual void LoadTexture(FTexture* TextureResource) = 0;
	virtual void Draw(const GameTimer& gt) = 0;
	virtual void UpdateMVP(const GameTimer& gt) = 0;


public:
	virtual Buffer* CreateBuffer(FRenderResource* renderResource) = 0;
};
