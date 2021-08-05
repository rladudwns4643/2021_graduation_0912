// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상 시킨다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일 되기 때문에
// 여기에 자주 업데이트할 파일을 추가하면 성능이 저하된다.
#pragma once

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"
#include "MathHelper.h"
#include "define.h"
#include "global.h"
#include "d3dUtil.h"

// 클라 서버 작업분리
#define DEBUG_CLIENT 0x00FF1
//#define DEBUG_SERVER 0x00FF2

#define FRUSTUM_CULLMODE 0x00FF3
// 풀스크린 모드
//#define FULLSCREEN_MODE 0x00FF4
#define ON_PAUSE 0x00FF5

// 콘솔창 띄우기
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#endif

#endif //PCH_H

// 버퍼 리소스를 생성하는 함수
extern ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nBytes,
	D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD,
	D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
	ID3D12Resource** ppd3dUploadBuffer = NULL);