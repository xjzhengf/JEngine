#pragma once
#include "stdafx.h"
#include "FRenderResource.h"
class FHDRResource :public FRenderResource {
public:
	virtual void ReleaseResource() {};
	virtual std::shared_ptr<FResource> GetRTVResource(int index) = 0;
	virtual std::shared_ptr<FResource> GetDSVResource(int index) = 0;
	virtual unsigned __int64 SRV(int index) = 0;
	virtual unsigned __int64 DSV(int index) = 0;
	virtual unsigned __int64 RTV(int index) = 0;

	std::vector<int> width;
	std::vector<int> height;
};

class DXHDRResource : public FHDRResource {
public:
	DXHDRResource();
	DXHDRResource(const DXHDRResource& rhs) = delete;
	DXHDRResource& operator=(const DXHDRResource& rhs) = delete;
	~DXHDRResource();

	virtual std::shared_ptr<FResource> GetRTVResource(int index) override;
	virtual std::shared_ptr<FResource> GetDSVResource(int index) override;
	virtual unsigned __int64 SRV(int index)override;
	virtual unsigned __int64 DSV(int index)override;
	virtual unsigned __int64 RTV(int index)override;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv,
		int index,
		int width, 
		int height
	);


	int HDRSize = 5;
private:
	void BuildDescriptors(int index);
	void BuildResource(int width, int height, int index);

	std::vector < CD3DX12_CPU_DESCRIPTOR_HANDLE> mhCpuSrv;
	std::vector < CD3DX12_GPU_DESCRIPTOR_HANDLE >mhGpuSrv;
	std::vector < CD3DX12_CPU_DESCRIPTOR_HANDLE >mhCpuDsv;
	std::vector < CD3DX12_CPU_DESCRIPTOR_HANDLE >mhCpuRtv;
	std::shared_ptr<FResource> mResource;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> mHDRs;
	std::vector < Microsoft::WRL::ComPtr<ID3D12Resource>> mShadows;


	int currentIndex = 0;
};