#pragma once
#include "stdafx.h"
#include "FRenderResource.h"
class FShadowResource :public FRenderResource {
public:
	virtual void ReleaseResource() {};
};

class DXShadowResource :public FShadowResource {
public:
	DXShadowResource();
	DXShadowResource(const DXShadowResource& rhs) = delete;
	DXShadowResource& operator=(const DXShadowResource& rhs) = delete;
	~DXShadowResource();

	ID3D12Resource* GetResource();
	CD3DX12_CPU_DESCRIPTOR_HANDLE& SRV() ;
	CD3DX12_CPU_DESCRIPTOR_HANDLE& DSV();

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv
	);
private:
	void BuildDescriptors();
	void BuildResource();

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuDsv;

	Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap = nullptr;
};