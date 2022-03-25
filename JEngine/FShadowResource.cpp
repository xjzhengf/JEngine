#include "stdafx.h"
#include "FShadowResource.h"
#include "Engine.h"
#include "DX12RHI.h"

DXShadowResource::DXShadowResource()
{
	BuildResource();
}

DXShadowResource::~DXShadowResource()
{
}

ID3D12Resource* DXShadowResource::GetResource()
{
	return mShadowMap.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE& DXShadowResource::SRV() 
{
	return this->mhCpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE& DXShadowResource::DSV()
{
	return this->mhCpuDsv;
}

void DXShadowResource::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv)
{
	mhCpuDsv = hCpuDsv;
	mhCpuSrv = hCpuSrv;
	mhGpuSrv = hGpuSrv;
	BuildDescriptors();
}

void DXShadowResource::BuildDescriptors()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice = 0;
	DX12RHI::GetDX12RHI()->GetDevice()->CreateShaderResourceView(mShadowMap.Get(), &srvDesc, mhCpuSrv);
	// Create DSV to resource so we can render to the shadow map.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	DX12RHI::GetDX12RHI()->GetDevice()->CreateDepthStencilView(mShadowMap.Get(), &dsvDesc, mhCpuDsv);
}

void DXShadowResource::BuildResource()
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc,sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = Engine::GetEngine()->GetWindow()->GetClientWidht();
	texDesc.Height = Engine::GetEngine()->GetWindow()->GetClientHeight();
	//texDesc.Width =1024;
	//texDesc.Height =1024;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	ThrowIfFailed(DX12RHI::GetDX12RHI()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&mShadowMap)
	));
}
