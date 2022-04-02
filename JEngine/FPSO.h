#pragma once
#include "stdafx.h"
#include "ShaderManager.h"
enum INPUT_CLASSIFICATION
{
	INPUT_CLASSIFICATION_PER_VERTEX_DATA = 0,
	INPUT_CLASSIFICATION_PER_INSTANCE_DATA = 1
};
enum LAYOUTTFORMAT
{

	FORMAT_R32G32B32_FLOAT = 6,
	FORMAT_R32G32B32A32_FLOAT = 2,
	FORMAT_R32G32_FLOAT = 16,
};
struct INPUT_ELEMENT_DESC
{
	LPCSTR SemanticName;
	UINT SemanticIndex;
	LAYOUTTFORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
};
class PipelineState {
public:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC dxPSO;
};

class FPSO {
public:
	PipelineState CreateFPSO(const std::string& PSOType, std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader);
	PipelineState BuildRenderFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader);
	PipelineState BuildDepthFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader);

	std::map<std::string, PipelineState> mPsoMap;
};

