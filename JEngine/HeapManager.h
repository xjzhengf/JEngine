#pragma once
#include "stdafx.h"
class FHeapManager {
public:
	FHeapManager(int NumDescriptors);
	void AddDesc();
	void DeleteDesc();
	int offsetIndex = 0;
};

enum DescType {
	CBV = 0,
	SRV = 1,
	UAV = 2
};
