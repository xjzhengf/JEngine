#include "stdafx.h"
#include "DXRHIResource.h"
#include "DX12RHI.h"

void DXRHIResource::CreateShader()
{
	HRESULT hr = S_OK;
	mvsByteCode = d3dUtil::CompileShader(L"..\\JEngine\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	mpsByteCode = d3dUtil::CompileShader(L"..\\JEngine\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
}

D3D12_CPU_DESCRIPTOR_HANDLE DXRHIResource::BackBufferView()
{
	return DX12RHI::GetDX12RHI()->CurrentBackBufferView();
}

void DXRHIResource::BuildPSO()
{
	
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { mInputLayout.data(),(UINT)mInputLayout.size() };

	psoDesc.VS = {
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
		mvsByteCode->GetBufferSize()
	};
	psoDesc.PS = {
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
		mpsByteCode->GetBufferSize()
	};


	D3D12_DEPTH_STENCIL_DESC stencilDesc;
	//反面
	stencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	stencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	stencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	stencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
	stencilDesc.DepthEnable = true;
	stencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	stencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//正面
	stencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	stencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	stencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	stencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
	stencilDesc.StencilEnable = true;
	stencilDesc.StencilReadMask = 0xff;
	stencilDesc.StencilWriteMask = 0xff;


	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	psoDesc.DepthStencilState = stencilDesc;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;

}
