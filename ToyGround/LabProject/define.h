#pragma once

// �� ������ ũ�� ����
#define FRAME_BUFFER_WIDTH 1280
#define FRAME_BUFFER_HEIGHT 720

// ��ǻ�� �ػ� ���� ���߾� X,Y ��ǥ
#define WindowCenterPosX ( GetSystemMetrics( SM_CXSCREEN ) - FRAME_BUFFER_WIDTH )  / 2	// ����ȭ���� ���� ũ�� GetSystemMetrics( SM_CXSCREEN )
#define WindowCenterPosY ( GetSystemMetrics( SM_CYSCREEN ) - FRAME_BUFFER_HEIGHT ) / 2	// ����ȭ���� ���� ũ�� GetSystemMetrics( SM_CYSCREEN )

// �Ʒ��� �ּ��ϸ� ���۽� â���� ����
#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

// �����̵Ǵ� ť�� ũ��
#define STD_CUBE_SIZE 5.0f
#define PLAYER_SIZE_X 4.5f
#define PLAYER_SIZE_Y 9.0f
#define PLAYER_SIZE_Z 4.5f

// �� ũ��
#define MAP_WIDTH_BLOCK_NUM 21
#define MAP_LENGTH_BLOCK_NUM 33
#define MAP_HEIGHT_BLOCK_NUM 3

// ����
#define MAX_LIGHTS				8 
#define MAX_MATERIALS			8 

#define POINT_LIGHT				1
#define SPOT_LIGHT				2
#define DIRECTIONAL_LIGHT		3

// �ؽ���
#define TEXTURE_INDEX_Texture_01 0

#define TEXTURE_STR_Cube_01 "Texture_01"


// ������ �����ϰ� ����
#define SAFE_DELETE_PTR(ptr)	\
{								\
	if (ptr != nullptr)			\
	{							\
		delete ptr;				\
		ptr = nullptr;			\
	}							\
}	