#pragma once
#include "stdafx.h"
#include "RHIResource.h"
class FRHI
{
public:
	virtual ~FRHI() {};
	virtual bool Initialize() = 0;

	virtual void ResetCommand() = 0;
	virtual void RSSetViewports(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth) = 0;
	virtual void RSSetScissorRects(long left, long top, long right, long bottom) = 0;
	virtual void ResourceBarrier(unsigned int NumberBarrier, int stateBefore, int stateAfter) {};
	virtual void ClearRenderTargetView() = 0;
	virtual void ClearDepthStencilView() = 0;
	virtual void OMSetStencilRef(int StencilRef) = 0;
	virtual void OMSetRenderTargets(int numTatgetDescriptors, bool RTsSingleHandleToDescriptorRange) = 0;
	virtual void SetDescriptorHeaps(int index) = 0;
	virtual void SetGraphicsRootSignature() = 0;
	virtual void IASetVertexBuffers() = 0;
	virtual void IASetIndexBuffer() = 0;
	virtual void IASetPrimitiveTopology() = 0;
	virtual void Offset(int index) = 0;
	virtual void SetGraphicsRootDescriptorTable(int index) {};
	virtual void SetGraphicsRoot32BitConstants() = 0;
	virtual void DrawIndexedInstanced(int index) = 0;
	virtual void ExecuteCommandLists() = 0;

	virtual void DrawPrepare() = 0;
	virtual void LoadTexture(FRHIResource* TextureResource) = 0;
	virtual void Draw(const GameTimer& gt) = 0;
	virtual void Update(const GameTimer& gt) = 0;

};
