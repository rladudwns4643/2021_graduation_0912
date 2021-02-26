#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����
#include <windows.h>
// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// Direct3D ��� �߰�
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

// ����Ʈ ���̺귯�� �߰�
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")

// C ���̺귯��
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;
