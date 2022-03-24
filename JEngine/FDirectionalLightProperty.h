#pragma once
#include "stdafx.h"
#include "FRenderResource.h"
class FDirectionalLight :FRenderResource
{
public:
	
	float Brightness;
	glm::vec3 Direction;
	glm::vec4 Location;
};

