#pragma once

// �� ������ ũ�� ����
#define FRAME_BUFFER_WIDTH 1280
#define FRAME_BUFFER_HEIGHT 720

// �����̵Ǵ� ť�� ũ��
#define STD_CUBE_SIZE 5.0f
#define PLAYER_SIZE_X 4.5f
#define PLAYER_SIZE_Y 9.0f
#define PLAYER_SIZE_Z 4.5f

// �� ť�� ����
#define MAP_WIDTH_BLOCK_NUM 21
#define MAP_DEPTH_BLOCK_NUM 33
#define MAP_HEIGHT_BLOCK_NUM 5

// ��ī�̹ڽ� ũ��
#define SKYBOX_SCALE 5000.f

// ����
#define MAX_LIGHTS				8 
#define MAX_MATERIALS			8 

#define POINT_LIGHT				1
#define SPOT_LIGHT				2
#define DIRECTIONAL_LIGHT		3

// ���� �̸�
#define LIGHT_NAME_DIRECTIONAL "Directional" 

// �ؽ���
#define TEXTURE_INDEX_SkyBox 0
#define TEXTURE_INDEX_Cartoon_CubeWorld_Texture 1

#define TEXTURE_STR_Cartoon_CubeWorld_Texture "Cartoon_CubeWorld_Texture"

// ĳ����
#define CHARACTER_TEST "TEST_CHARCTER"

// ������Ʈ �޽�
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

// ������ �����ϰ� ����
#define SAFE_DELETE_PTR(ptr)	\
{								\
	if (ptr != nullptr)			\
	{							\
		delete ptr;				\
		ptr = nullptr;			\
	}							\
}	

// ���콺 ����(DPI = Dots Per Inch)
#define MOUSE_DPI_X 0.3f
#define MOUSE_DPI_Y 0.3f

// �̵����� Ű����
#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_UP						0x10
#define DIR_DOWN					0x20

// ī�޶� ����
#define CAMERA_ZNEAR 1.f
#define CAMERA_ZFAR 5000.f

// enum class
enum class SceneType { eLobby = 0, eGamePlay = 1, eCount = 2};
enum class CameraType { eFirst = 0, eThird = 1, eFree = 2 };

// �� �����ΰ��� �������̿�����
#define GAME_MAP			0
#define TEMP_SECOND_MAP		1 // �ι�° ��

// �ΰ��� ��
#define MAP_STR_GAME_MAP	"origin"
#define MAP_STR_SECONDMAP	"second_map"

// ���� ��ġ
#define SPAWN_RED_TEAM 0
#define SPAWN_BLUE_TEAM 1
#define SPAWN_CENTER 2