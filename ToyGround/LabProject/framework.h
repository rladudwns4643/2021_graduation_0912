#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <WindowsX.h>
#include <Windows.h>
#include <shellapi.h>
#include <wrl.h>
#include <Mmsystem.h>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

// For Play Video 
#include <Vfw.h>
#pragma comment(lib, "vfw32.lib")


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

// C++ 런타임 헤더 파일입니다.
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
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

// D2D (for Render Text)
#include <d2d1_3.h>
#include <dwrite.h>
#include <d3d11on12.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")

// FMOD
#include "../FMOD/inc/fmod.h"
#pragma comment(lib,"../FMOD/lib_x64/fmodL_vc.lib")

using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace DirectX::PackedVector;

#define AppContext ApplicationContext::GetApp()

#define SAFE_DELETE_PTR(ptr)	\
{								\
	if (ptr != nullptr)			\
	{							\
		delete ptr;				\
		ptr = nullptr;			\
	}							\
}

#define _MSG_BOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("System Message"), MB_OK);