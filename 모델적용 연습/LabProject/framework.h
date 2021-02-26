#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// Direct3D 헤더 추가
#include<string>
#include<wrl.h>
#include<shellapi.h>

#include<d3d12.h>
#include<dxgi1_4.h>

#include<d3dcompiler.h>

#include<DirectXMath.h>
#include<DirectXPackedVector.h>
#include<DirectXColors.h>
#include<DirectXCollision.h>

#include<dxgidebug.h>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// 임포트 라이브러리 추가
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")

// C 라이브러리
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;
