#pragma once

// 주 윈도우 크기 설정
#define FRAME_BUFFER_WIDTH 1280
#define FRAME_BUFFER_HEIGHT 720

// 컴퓨터 해상도 기준 정중앙 X,Y 좌표
#define WindowCenterPosX ( GetSystemMetrics( SM_CXSCREEN ) - FRAME_BUFFER_WIDTH )  / 2	// 현재화면의 가로 크기 GetSystemMetrics( SM_CXSCREEN )
#define WindowCenterPosY ( GetSystemMetrics( SM_CYSCREEN ) - FRAME_BUFFER_HEIGHT ) / 2	// 현재화면의 세로 크기 GetSystemMetrics( SM_CYSCREEN )

// 아래를 주석하면 시작시 창모드로 시작
#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

// 기준이되는 큐브 크기
#define STD_CUBE_SIZE 5.0f
#define PLAYER_SIZE_X 4.5f
#define PLAYER_SIZE_Y 9.0f
#define PLAYER_SIZE_Z 4.5f

// 맵 크기
#define MAP_WIDTH_BLOCK_NUM 21
#define MAP_LENGTH_BLOCK_NUM 33
#define MAP_HEIGHT_BLOCK_NUM 3