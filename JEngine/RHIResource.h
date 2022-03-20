#pragma once
#include "stdafx.h"
class FRHIResource {
public:
	virtual void Release()=0;
	virtual void CreateShader()=0;
};