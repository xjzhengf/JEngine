#pragma once
#include "stdafx.h"
class FRHIResource {
public:
	virtual void SetResoureInMap(std::string name){};
protected:
	std::unordered_map<std::string, std::shared_ptr<FRHIResource>> ResoureMap;
};