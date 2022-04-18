#pragma once
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#include <set>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include <windowsx.h>
#include <functional>
#include <Windows.h>
#include <commdlg.h>  
#include <DirectXMath.h>
#include <cstdint>
#include <assert.h>
#include <algorithm>
#include <memory>
#include <crtdbg.h>
#include <wrl.h>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "d3dx12.h"
#include "DDSTextureLoader.h"
#include "MathHelper.h"
#include"iostream"
#include "vector"
#include "GameTimer.h"
#include "Buffer.h"
#include "MathHelper.h"
#include "MeshProperty.h"
#include "D3DUtil.h"
#include "RenderItem.h"
#include <pix.h>
#define _CRTDBG_MAP_ALLOC

#define USE_BLOOMDOWN  0;
using namespace DirectX;
using CallBackInString = std::function<void(std::string)>;
