#pragma once
#include "stdafx.h"
#include "FRenderResource.h"
class FHDRResource :public FRenderResource {
public:
	virtual void ReleaseResource() {};
	virtual std::shared_ptr<FResource> GetResource() = 0;
	virtual unsigned __int64 SRV() = 0;
	virtual unsigned __int64 DSV() = 0;
	virtual unsigned __int64 RTV() = 0;

};

class DXHDRResource : public FHDRResource {
public:
	DXHDRResource();
	DXHDRResource(const DXHDRResource& rhs) = delete;
	DXHDRResource& operator=(const DXHDRResource& rhs) = delete;
	~DXHDRResource();

	virtual std::shared_ptr<FResource> GetResource() override;
	virtual unsigned __int64 SRV() override;
	virtual unsigned __int64 DSV()override;
	virtual unsigned __int64 RTV() override;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv
	);
private:
	void BuildDescriptors();
	void BuildResource();

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuDsv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuRtv;
	std::shared_ptr<FResource> mResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> mHDR = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> mShadow = nullptr;
};