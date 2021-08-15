#pragma once

// 중력
#define GRAVITY 9.8f

// 주 윈도우 크기 설정
#define FRAME_BUFFER_WIDTH 1280
#define FRAME_BUFFER_HEIGHT 720

// 마우스 관련(DPI = Dots Per Inch)
#define MOUSE_DPI_X 0.05f
#define MOUSE_DPI_Y 0.05f

// 크기변환
#define ScaleConverter 20.f

// 기준이되는 큐브 크기
#define STD_CUBE_SIZE (5.0f * ScaleConverter)
#define PLAYER_SIZE_X (3.0f * ScaleConverter)
#define PLAYER_SIZE_Y (9.5f * ScaleConverter)
#define PLAYER_SIZE_Z (3.0f * ScaleConverter)

// 플레이어 스피드
#define PLAYER_SPEED (16.f * ScaleConverter)
#define BULLET_SPEED 75.f
#define SKILL_BULLET_SPEED 90.f 

// 맵 큐브 개수
#define MAP_WIDTH_BLOCK_NUM 21
#define MAP_DEPTH_BLOCK_NUM 33
#define MAP_HEIGHT_BLOCK_NUM 5

// 물 개수
#define MAP_WATER_NUM 86

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

// 텍스쳐 인덱스
#define TEXTURE_INDEX_SkyBox									0
#define TEXTURE_INDEX_Cartoon_CubeWorld_Texture					1
#define TEXTURE_INDEX_PolygonMinis_Texture_01_A					2
#define TEXTURE_INDEX_Polygon_Plane_Texture_02					3
#define TEXTURE_INDEX_Polygon_Plane_Texture_03					4
#define TEXTURE_INDEX_Floor_Texture								5

// UI
#define TEXTURE_INDEX_UI_MATCHINGROOM_BACKGROUND				6
#define TEXTURE_INDEX_UI_MATCHINGROOM_LOGO_TOYGROUND			7
#define TEXTURE_INDEX_UI_MATCHINGROOM_READY1_RELEASED			8
#define TEXTURE_INDEX_UI_MATCHINGROOM_READY1_PRESSED			9
#define TEXTURE_INDEX_UI_MATCHINGROOM_READY2_RELEASED			10
#define TEXTURE_INDEX_UI_MATCHINGROOM_READY2_PRESSED			11

#define TEXTURE_INDEX_UI_LOBBY_BACKGROUND						12
#define TEXTURE_INDEX_UI_LOBBY_LOGO_TOYGROUND					13
#define TEXTURE_INDEX_UI_LOBBY_LOGINUI							14
#define TEXTURE_INDEX_UI_LOBBY_ID_INPUTATIVATE					15
#define TEXTURE_INDEX_UI_LOBBY_PW_INPUTATIVATE					16
#define TEXTURE_INDEX_UI_LOBBY_LOADINGUI						17
#define TEXTURE_INDEX_UI_LOBBY_LOADING_BAR						18

#define TEXTURE_INDEX_UI_GAMEPLAY_AIM							19
#define TEXTURE_INDEX_UI_GAMEPLAY_PLAYER1_SCORE					20
#define TEXTURE_INDEX_UI_GAMEPLAY_PLAYER2_SCORE					21
#define TEXTURE_INDEX_UI_GAMEPLAY_TIMER							22
#define TEXTURE_INDEX_UI_GAMEPLAY_HEALTH						23
#define TEXTURE_INDEX_UI_GAMEPLAY_ATTACK_GAUGE					24
#define TEXTURE_INDEX_UI_GAMEPLAY_SKILL_GAUGE					25
#define TEXTURE_INDEX_UI_GAMEPLAY_STATE_FRONT					26
#define TEXTURE_INDEX_UI_GAMEPLAY_STATE_BACK					27
#define TEXTURE_INDEX_UI_GAMEPLAY_WINNERBOARD					28

// 텍스쳐 스트링
#define TEXTURE_STR_Cartoon_CubeWorld_Texture			"Cartoon_CubeWorld_Texture"
#define TEXTURE_STR_PolygonMinis_Texture_01_A			"PolygonMinis_Texture_01_A"
#define TEXTURE_STR_Polygon_Plane_Texture_02			"Polygon_Plane_Texture_02"
#define TEXTURE_STR_Polygon_Plane_Texture_03			"Polygon_Plane_Texture_03"
#define TEXTURE_STR_Floor_Texture						"Floor_Texture"

#define TEXTURE_STR_UI_MATCHINGROOM_BACKGROUND				"UI_MATCHINGROOM_BACKGROUND"
#define TEXTURE_STR_UI_MATCHINGROOM_LOGO_TOYGROUND			"UI_MATCHINGROOM_LOGO_TOYGROUND"
#define TEXTURE_STR_UI_MATCHINGROOM_READY1_BUTTON_RELEASED	"UI_MATCHINGROOM_READY1_BUTTON_RELEASED"
#define TEXTURE_STR_UI_MATCHINGROOM_READY1_BUTTON_PRESSED	"UI_MATCHINGROOM_READY1_BUTTON_PRESSED"
#define TEXTURE_STR_UI_MATCHINGROOM_READY2_BUTTON_RELEASED	"UI_MATCHINGROOM_READY2_BUTTON_RELEASED"
#define TEXTURE_STR_UI_MATCHINGROOM_READY2_BUTTON_PRESSED	"UI_MATCHINGROOM_READY2_BUTTON_PRESSED"

#define TEXTURE_STR_UI_LOBBY_BACKGROUND					"UI_LOBBY_BACKGROUND"
#define TEXTURE_STR_UI_LOBBY_LOGO_TOYGROUND				"UI_LOBBY_LOGO_TOYGROUND"
#define TEXTURE_STR_UI_LOBBY_LOGINUI					"UI_LOBBY_LOGINUI"	
#define TEXTURE_STR_UI_LOBBY_ID_INPUTATIVATE			"UI_LOBBY_ID_INPUTATIVATE"
#define TEXTURE_STR_UI_LOBBY_PW_INPUTATIVATE			"UI_LOBBY_PW_INPUTATIVATE"
#define TEXTURE_STR_UI_LOBBY_LOADINGUI					"UI_LOBBY_LOADINGUI"
#define TEXTURE_STR_UI_LOBBY_LOADING_BAR				"UI_LOBBY_LOADING_BAR"

#define TEXTURE_STR_UI_GAMEPLAY_AIM						"UI_GAMEPLAY_AIM"
#define TEXTURE_STR_UI_GAMEPLAY_PLAYER1_SCORE			"UI_GAMEPLAY_PLAYER1_SCORE"
#define TEXTURE_STR_UI_GAMEPLAY_PLAYER2_SCORE			"UI_GAMEPLAY_PLAYER2_SCORE"
#define TEXTURE_STR_UI_GAMEPLAY_TIMER					"UI_GAMEPLAY_TIMER"
#define TEXTURE_STR_UI_GAMEPLAY_HEALTH					"UI_GAMEPLAY_HEALTH"
#define TEXTURE_STR_UI_GAMEPLAY_ATTACK_GAUGE			"UI_GAMEPLAY_ATTACK_GAUGE"
#define TEXTURE_STR_UI_GAMEPLAY_SKILL_GAUGE				"UI_GAMEPLAY_SKILL_GAUGE"
#define TEXTURE_STR_UI_GAMEPLAY_STATE_FRONT				"UI_GAMEPLAY_STATE_FRONT"
#define TEXTURE_STR_UI_GAMEPLAY_STATE_BACK				"UI_GAMEPLAY_STATE_BACK"
#define TEXTURE_STR_UI_GAMEPLAY_WINNERBOARD				"UI_GAMEPLAY_WINNERBOARD"

// 캐릭터
enum BoneIndex { Cowboy = 0, GunMan, Count };

// UI
#define OBJECT_TYPE_UI2D						"UI_2D"
#define OBJECT_TYPE_UI2D_PLAYBOARD				"UI_2D_PLAYBOARD"
#define OBJECT_TYPE_AIM							"UI_AIM"
#define OBJECT_TYPE_ID_INPUTATIVATE				"UI_ID_INPUTATiVATE"
#define OBJECT_TYPE_PW_INPUTATIVATE				"UI_PW_INPUTATiVATE"
#define OBJECT_TYPE_STATE_FRONT					"UI_STATE_FRONT"
#define OBJECT_TYPE_STATE_BACK					"UI_STATE_BACK"
#define OBJECT_TYPE_WINNERBOARD					"UI_WINNERBOARD"
#define OBJECT_TYPE_LOADINGUI					"UI_LOBBY_LOADINGUI"
#define OBJECT_TYPE_LOADING_BAR					"UI_LOADING_BAR"
#define OBJECT_TYPE_READY1_PRESSED				"UI_READY1_PRESSED"
#define OBJECT_TYPE_READY2_PRESSED				"UI_READY2_PRESSED"

#define OBJECT_NAME_MATCHINGROOM_BACKGROUND			"BACKGROUND"
#define OBJECT_NAME_MATCHINGROOM_LOGO_TOYGROUND		"LOGO_TOYGROUND"
#define OBJECT_NAME_MATCHINGROOM_READY1_BUTTON_R	"READY1_BUTTON_R"
#define OBJECT_NAME_MATCHINGROOM_READY1_BUTTON_P	"READY1_BUTTON_P"
#define OBJECT_NAME_MATCHINGROOM_READY2_BUTTON_R	"READY2_BUTTON_R"
#define OBJECT_NAME_MATCHINGROOM_READY2_BUTTON_P	"READY2_BUTTON_P"

#define OBJECT_NAME_LOBBY_BACKGROUND			"LOBBY_BACKGROUND"
#define OBJECT_NAME_LOBBY_LOGO_TOYGORUND		"LOBBY_LOGO_TOYGROUND"
#define OBJECT_NAME_LOBBY_LOGINUI				"LOBBY_LOGINUI"
#define OBJECT_NAME_LOBBY_ID_INPUTATIVATE		"LOBBY_ID_INPUTATIVATE"
#define OBJECT_NAME_LOBBY_PW_INPUTATIVATE		"LOBBY_PW_INPUTATIVATE"
#define OBJECT_NAME_LOBBY_LOADINGUI				"LOBBY_LOADINGUI"
#define OBJECT_NAME_LOBBY_LOADING_BAR			"LOBBY_LOADING_BAR"

#define OBJECT_NAME_GAMEPLAY_AIM				"GAMEPLAY_AIM"
#define OBJECT_NAME_GAMEPLAY_PLAYER1_SCORE		"GAMEPLAY_PLAYER1_SCORE"
#define OBJECT_NAME_GAMEPLAY_PLAYER2_SCORE		"GAMEPLAY_PLAYER2_SCORE"
#define OBJECT_NAME_GAMEPLAY_TIMER				"GAMEPLAY_TIMER"
#define OBJECT_NAME_GAMEPLAY_HEALTH				"GAMEPLAY_HEALTH"
#define OBJECT_NAME_GAMEPLAY_ATTACK_GAUGE		"GAMEPLAY_ATTACK_GAUGE"
#define OBJECT_NAME_GAMEPLAY_SKILL_GAUGE		"GAMEPLAY_SKILL_GAUGE"
#define OBJECT_NAME_GAMEPLAY_STATE_FRONT		"GAMEPLAY_STATE_FRONT"
#define OBJECT_NAME_GAMEPLAY_STATE_BACK			"GAMEPLAY_STATE_BACK"
#define OBJECT_NAME_GAMEPLAY_WINNERBOARD		"GAMEPLAY_WINNERBOARD"

// UI Layer
#define UI_LAYER_BACK -1
#define UI_LAYER_0 0
#define UI_LAYER_1 1

// 오브젝트 메쉬
#define OBJECT_MESH_STR_CUBE_01			"Cube_01"
#define OBJECT_MESH_STR_CUBE_02			"Cube_02"
#define OBJECT_MESH_STR_CUBE_03			"Cube_03"
#define OBJECT_MESH_STR_CUBE_04			"Cube_04"
#define OBJECT_MESH_STR_CUBE_PLAT_01	"Cube_Plat_01"
#define OBJECT_MESH_STR_CUBE_PLAT_02	"Cube_Plat_02"
#define OBJECT_MESH_STR_CUBE_PLAT_03	"Cube_Plat_03"
#define OBJECT_MESH_STR_CUBE_PLAT_04	"Cube_Plat_04"
#define OBJECT_MESH_STR_WALL_21			"Wall_21"
#define OBJECT_MESH_STR_WALL_33			"Wall_33"
#define OBJECT_MESH_STR_CUBE_BRIDGE		"Cube_Bridge"
#define OBJECT_MESH_STR_PLANT			"Plant"
#define OBJECT_MESH_STR_TILE_01			"Tile_01"
#define OBJECT_MESH_STR_TILE_02			"Tile_02"
#define OBJECT_MESH_STR_TREE_01			"Tree_01"
#define OBJECT_MESH_STR_TREE_02			"Tree_02"
#define OBJECT_MESH_STR_BARREL			"Barrel"
#define OBJECT_MESH_STR_CHEST_DOWN		"Chest_Down"
#define OBJECT_MESH_STR_CHEST_UP		"Chest_Up"
#define OBJECT_MESH_STR_FENCE_01		"Fence_01"
#define OBJECT_MESH_STR_FENCE_02		"Fence_02"
#define OBJECT_MESH_STR_GRAVESTONE		"Gravestone"
#define OBJECT_MESH_STR_LOGS			"Logs"
#define OBJECT_MESH_STR_REVOLVER		"Revolver"
#define OBJECT_MESH_STR_ATTACK_BOX		"Attack_Box"
#define OBJECT_MESH_STR_GEM				"Gem"
#define OBJECT_MESH_STR_BULLET_01		"Bullet_01"
#define OBJECT_MESH_STR_BULLET_02		"Bullet_02"
#define OBJECT_MESH_STR_FLOOR			"Floor"
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
enum class SceneType { eLobby = 0, eMatchingRoom = 1, eGamePlay = 2, eCount = 3 };
enum class CameraType { eThird = 0, eFree = 1, eFix = 2 };
enum class TextAlignType { NONE, Center, Left, Right, Top, Bottom, LT, LB, RT, RB };

// 맵 디파인값이 연속적이여야함
#define GAME_MAP			0
#define TEMP_SECOND_MAP		1 // 두번째 맵

// 인게임 맵
#define MAP_STR_GAME_MAP	"FirstMap"
#define MAP_STR_SECONDMAP	"second_map"

// 스폰 위치
#define SPAWN_RED_TEAM 0
#define SPAWN_BLUE_TEAM 1
#define SPAWN_CENTER 2

// 캐릭터
#define CHARACTER_COWBOY "Cowboy"
#define CHARACTER_GUNMAN "GunMan"

// Animation KeyState
#define KEY_IDLE "Idle"
#define KEY_FORWARD "Running"
#define KEY_BACKWARD "Backward"
#define KEY_LEFT_STRAFE "Left_Strafe"
#define KEY_RIGHT_STRAFE "Right_Strafe"

#define KEY_ATTACK "Attack"
#define KEY_JUMP "Jump"
#define KEY_T_POSE "T_Pose"

#define MAX_ID_LEN 10

// 플레이어 상태
#define MAX_HP 4000
#define MAX_ATTACKGAUGE 300
#define MAX_SKILLGAUGE 120
#define ONE_HIT_CHARGE_SKILLGAUGE 10
#define MAXGAUGE_SIZE_X 245

// 파티클
#define PARTICLE_NAME_SMOKE "particle_smoke"