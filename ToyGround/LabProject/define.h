#pragma once

// 주 윈도우 크기 설정
#define FRAME_BUFFER_WIDTH 1280
#define FRAME_BUFFER_HEIGHT 720

// 기준이되는 큐브 크기
#define STD_CUBE_SIZE 5.0f
#define PLAYER_SIZE_X 4.5f
#define PLAYER_SIZE_Y 9.0f
#define PLAYER_SIZE_Z 4.5f

// 맵 큐브 개수
#define MAP_WIDTH_BLOCK_NUM 21
#define MAP_DEPTH_BLOCK_NUM 33
#define MAP_HEIGHT_BLOCK_NUM 5

// 스카이박스 크기
#define SKYBOX_SCALE 5000.f

// 조명
#define MAX_LIGHTS				8 
#define MAX_MATERIALS			8 

#define POINT_LIGHT				1
#define SPOT_LIGHT				2
#define DIRECTIONAL_LIGHT		3

// 조명 이름
#define LIGHT_NAME_DIRECTIONAL "Directional" 

// 텍스쳐
#define TEXTURE_INDEX_SkyBox 0
#define TEXTURE_INDEX_Cartoon_CubeWorld_Texture 1

#define TEXTURE_STR_Cartoon_CubeWorld_Texture "Cartoon_CubeWorld_Texture"

// 캐릭터
#define CHARACTER_TEST "TEST_CHARCTER"

// 오브젝트 메쉬
#define OBJECT_MESH_STR_TEST_PLAYER		"Test_Player"
#define OBJECT_MESH_STR_CUBE_01			"Cube_01"
#define OBJECT_MESH_STR_CUBE_02			"Cube_02"
#define OBJECT_MESH_STR_CUBE_03			"Cube_03"
#define OBJECT_MESH_STR_CUBE_04			"Cube_04"
#define OBJECT_MESH_STR_CUBE_BRIDGE		"Cube_Bridge"
#define OBJECT_MESH_STR_PLANT			"Plant"
#define OBJECT_MESH_STR_TILE_01			"Tile_01"
#define OBJECT_MESH_STR_TILE_02			"Tile_02"
#define OBJECT_MESH_STR_TREE_01			"Tree_01"
#define OBJECT_MESH_STR_TREE_02			"Tree_02"
#define OBJECT_MESH_STR_BARREL			"Barrel"
#define OBJECT_MESH_STR_CHEST			"Chest"
#define OBJECT_MESH_STR_FENCE_01		"Fence_01"
#define OBJECT_MESH_STR_FENCE_02		"Fence_02"
#define OBJECT_MESH_STR_GRAVESTONE		"Gravestone"
#define OBJECT_MESH_STR_LOGS			"Logs"
//
#define MESH_GEOID			"geo"
#define MESH_GEOID_RECT		"rect"
#define MESH_GEOID_SPHERE	"sphere"
#define MESH_GEOID_GRID		"grid"
#define MESH_GEOID_CYLINDER "cylinder"

// 포인터 안전하게 삭제
#define SAFE_DELETE_PTR(ptr)	\
{								\
	if (ptr != nullptr)			\
	{							\
		delete ptr;				\
		ptr = nullptr;			\
	}							\
}	

// 마우스 관련(DPI = Dots Per Inch)
#define MOUSE_DPI_X 0.3f
#define MOUSE_DPI_Y 0.3f

// 이동관련 키보드
#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_UP						0x10
#define DIR_DOWN					0x20

// 카메라 관련
#define CAMERA_ZNEAR 1.f
#define CAMERA_ZFAR 5000.f

// enum class
enum class SceneType { eLobby = 0, eGamePlay = 1, eCount = 2};
enum class CameraType { eFirst = 0, eThird = 1, eFree = 2 };

// 맵 디파인값이 연속적이여야함
#define GAME_MAP			0
#define TEMP_SECOND_MAP		1 // 두번째 맵

// 인게임 맵
#define MAP_STR_GAME_MAP	"origin"
#define MAP_STR_SECONDMAP	"second_map"

// 스폰 위치
#define SPAWN_RED_TEAM 0
#define SPAWN_BLUE_TEAM 1
#define SPAWN_CENTER 2