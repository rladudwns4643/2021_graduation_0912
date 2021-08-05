// pch.h: �̸� �����ϵ� ��� �����Դϴ�.
// �Ʒ� ������ ������ �� ���� �����ϵǾ�����, ���� ���忡 ���� ���� ������ ��� ��Ų��.
// �׷��� ���⿡ ������ ������ ���� �� ������Ʈ�Ǵ� ��� ��� �ٽ� ������ �Ǳ� ������
// ���⿡ ���� ������Ʈ�� ������ �߰��ϸ� ������ ���ϵȴ�.
#pragma once

#ifndef PCH_H
#define PCH_H

// ���⿡ �̸� �������Ϸ��� ��� �߰�
#include "framework.h"
#include "MathHelper.h"
#include "define.h"
#include "global.h"
#include "d3dUtil.h"

// Ŭ�� ���� �۾��и�
#define DEBUG_CLIENT 0x00FF1
//#define DEBUG_SERVER 0x00FF2

#define FRUSTUM_CULLMODE 0x00FF3
// Ǯ��ũ�� ���
//#define FULLSCREEN_MODE 0x00FF4
#define ON_PAUSE 0x00FF5

// �ܼ�â ����
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#endif

#endif //PCH_H

// ���� ���ҽ��� �����ϴ� �Լ�
extern ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nBytes,
	D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD,
	D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
	ID3D12Resource** ppd3dUploadBuffer = NULL);