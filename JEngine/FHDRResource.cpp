#include "stdafx.h"
#include "Engine.h"
#include "DX12RHI.h"
#include "FHDRResource.h"

DXHDRResource::DXHDRResource()
{
	BuildResource();
	mResource = std::make_shared<FResource>();
}

DXHDRResource::~DXHDRResource()
{
}

std::shared_ptr<FResource> DXHDRResource::GetResource()
{
	mResource->Resource = mHDR.Get();
	return mResource;
}

unsigned __int64 DXHDRResource::SRV()
{
	return this->mhCpuSrv.ptr;
}

unsigned __int64 DXHDRResource::DSV()
{
	return this->mhCpuDsv.ptr;
}

unsigned __int64 DXHDRResource::RTV()
{
	return this->mhCpuRtv.ptr;
}

void DXHDRResource::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv,CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv)
{
	mhCpuDsv = hCpuDsv;
	mhCpuSrv = hCpuSrv;
	mhGpuSrv = hGpuSrv;
	mhCpuRtv = hCpuRtv;
	BuildDescriptors();
}

void DXHDRResource::BuildDescriptors()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice = 0;
	DX12RHI::GetDX12RHI()->GetDevice()->CreateShaderResourceView(mHDR.Get(), &srvDesc, mhCpuSrv);
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;
	DX12RHI::GetDX12RHI()->GetDevice()->CreateRenderTargetView(mHDR.Get(), &rtvDesc, mhCpuRtv);
	// Create DSV to resource so we can render to the shadow map.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	DX12RHI::GetDX12RHI()->GetDevice()->CreateDepthStencilView(mShadow.Get(), &dsvDesc, mhCpuDsv);
}

void DXHDRResource::BuildResource()
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = Engine::GetEngine()->GetWindow()->GetClientWidht();
	texDesc.Height = Engine::GetEngine()->GetWindow()->GetClientHeight();
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	ThrowIfFailed(DX12RHI::GetDX12RHI()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(&mHDR)
	));

	D3D12_RESOURCE_DESC shadowTexDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	shadowTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	shadowTexDesc.Alignment = 0;
	shadowTexDesc.Width = 2048;
	shadowTexDesc.Height = 2048;
	shadowTexDesc.DepthOrArraySize = 1;
	shadowTexDesc.MipLevels = 1;
	shadowTexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	shadowTexDesc.SampleDesc.Count = 1;
	shadowTexDesc.SampleDesc.Quality = 0;
	shadowTexDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	shadowTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE shadowOptClear;
	shadowOptClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	shadowOptClear.DepthStencil.Depth = 1.0f;
	shadowOptClear.DepthStencil.Stencil = 0;

	ThrowIfFailed(DX12RHI::GetDX12RHI()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&shadowTexDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&shadowOptClear,
		IID_PPV_ARGS(&mShadow)
	));
}
