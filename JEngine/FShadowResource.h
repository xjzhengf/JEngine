#pragma once
#include "stdafx.h"
#include "FRenderResource.h"
class FShadowResource :public FRenderResource {
public:
	virtual void ReleaseResource() {};
	virtual std::shared_ptr<FResource> GetResource() =0;
	virtual unsigned __int64 SRV() =0;
	virtual unsigned __int64 DSV() =0;
};

class DXShadowResource :public FShadowResource {
public:
	DXShadowResource();
	DXShadowResource(const DXShadowResource& rhs) = delete;
	DXShadowResource& operator=(const DXShadowResource& rhs) = delete;
	~DXShadowResource();

	virtual std::shared_ptr<FResource> GetResource() override;
	virtual unsigned __int64 SRV() override;
	virtual unsigned __int64 DSV()override;

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
	std::shared_ptr<FResource> mResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap = nullptr;
};