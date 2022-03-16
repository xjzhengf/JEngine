#pragma once
#include "stdafx.h"
class FRHI
{
public:
	virtual bool Initialize() = 0;
	//virtual void RSSetViewports(float TopLeftX,float TopLeftY,float Width,float Height,float MinDepth,float MaxDepth) = 0;
	//virtual void RSSetScissorRects(long left,long top,long right,long bottom) = 0;
	//virtual void OMSetStencilRef(int StencilRef) = 0;
	//
	//virtual void SetDescriptorHeaps() = 0;
	//virtual void SetGraphicsRootSignature() = 0;
	//virtual void IASet( ) = 0;
	//virtual void IASetIndexBuffer() = 0;
	//virtual void IASetPrimitiveTopology() = 0;

	virtual void Draw(const GameTimer& gt) = 0;
	virtual void Update(const GameTimer& gt) = 0;

};
