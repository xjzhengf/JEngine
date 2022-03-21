#include "stdafx.h"
#include "FRHIFactory.h"
#include "DX12RHI.h"
#include "DXRHIResource.h"
std::shared_ptr<FDynamicRHI> FRHIFactory::CreateRHI()
{
#ifdef _WIN32
	return std::make_shared<DX12RHI>();
#else 
	return nullptr;
#endif
}

std::shared_ptr<FRHIResource> FRHIFactory::CreateRHIResource()
{
#ifdef _WIN32
	return std::make_shared<DXRHIResource>();
#else 
	return nullptr;
#endif
}