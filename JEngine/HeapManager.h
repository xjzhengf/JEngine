#pragma once
#include "stdafx.h"
class FHeapManager {
public:
	FHeapManager(int NumDescriptors);
	void AddDesc();
	void DeleteDesc();
	int offsetIndex = 0;
};
