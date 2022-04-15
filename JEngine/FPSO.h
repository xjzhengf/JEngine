#pragma once
#include "stdafx.h"
#include "ShaderManager.h"
enum INPUT_CLASSIFICATION
{
	INPUT_CLASSIFICATION_PER_VERTEX_DATA = 0,
	INPUT_CLASSIFICATION_PER_INSTANCE_DATA = 1
};

struct INPUT_ELEMENT_DESC
{
	LPCSTR SemanticName;
	UINT SemanticIndex;
	GI_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
};
class PipelineState {
public:
	std::string PSOName;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC dxPSO;
};

class FPSO {
public:
	FPSO();
	const PipelineState& CreateFPSO(const std::string& PSOType, std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader);
protected:
	PipelineState BuildRenderFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader);
	PipelineState BuildDepthFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader);
	PipelineState BuildBloomFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader);
	PipelineState BuildBloomSetFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader);
	PipelineState BuildBloomUpFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader);
	PipelineState BuildBloomDownFPSO(std::vector<INPUT_ELEMENT_DESC> mInputLayout, FShader* shader);
	PipelineState mPso;
	std::vector<D3D12_INPUT_ELEMENT_DESC> dxInputLayout;
	std::map<std::string, PipelineState> mPsoMap;
};

