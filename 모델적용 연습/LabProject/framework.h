#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <WindowsX.h>
#include <Windows.h>
#include <shellapi.h>
#include <wrl.h>
#include <Mmsystem.h>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

// C 런타임 헤더 파일입니다.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <fstream>
#include <comdef.h>
#include <float.h>
#include <cstdint>

// C++ 런타임 헤더 파일 입니다.
#include <iostream>
#include <array>
#include <list>
#include <vector>
#include <deque>
#include <algorithm>
#include <iterator>
#include <assert.h>
#include <string>
#include <cmath>
#include <map>
#include <set>
#include <unordered_map>
#include <sstream>

#include <thread>
#include <condition_variable>
#include <mutex>

// DirectX 런타임 헤더 파일입니다.
#include "d3dx12.h"
//#include <d3d11_1.h>
#include <dxgi1_4.h>
#include <dxgidebug.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")

// D2D (for Render Text)
#include <d2d1_3.h>
#include <dwrite.h>
#include <d3d11on12.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;
