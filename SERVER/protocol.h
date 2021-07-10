#pragma once
#include <DirectXMath.h>

#pragma region PORT SETTING
constexpr short LOBBY_SERVER_PORT = 7237;
constexpr short BATTLE_SERVER_PORT = 7238;

#define LOBBY_SERVER_IP			"121.170.59.45"
#define BATTLE_SERVER_IP		"121.170.59.45"
#pragma endregion

#pragma region LENGTH
constexpr short MAX_ID_LEN = 10;
#pragma endregion

#pragma region FAIL CONNECT ROOM CODE
#define ROOM_FAIL_CODE_ROOM_NOT_FOUND	0
#define ROOM_FAIL_CODE_ROOM_IS_FULL		1
#define ROOM_FAIL_CODE_ROOM_STARTED		2
#pragma endregion

constexpr short MAX_ROOM_COUNT = 5;
constexpr short ADD_COIN_TIME = 15;

constexpr short OBJECT_START_INDEX_BULLET = 1000;
constexpr short MAX_BULLET_COUNT = 20;

constexpr short OBJECT_TYPE_TOY			=	1;
constexpr short OBJECT_TYPE_BULLET		=	2;

#define ANIM_IDLE		0
#define ANIM_JUMP		1
#define ANIM_MOVE		2
#define ANIM_ATTK		3
#define ANIM_TEST		4

using PacketSize = unsigned char;
enum class PacketType : unsigned short {
	NO_MSG = 0xFA11,

#pragma region LC (+LB, +BL)
	LC_LOGIN_OK = 0xA000,
	LC_LOGIN_FAIL = 0xA001,
	LC_SIGNUP_OK = 0xA002,
	LC_SIGNUP_FAIL = 0xA003,
	LC_USERINFO = 0xA004,
	LC_FIND_ROOM = 0xA005,
	LC_CANCLE_FIND_ROOM = 0xA006,
	LB_REQUEST_ROOM = 0xA007,
	BL_ROOMREADY = 0xA008,
#pragma endregion
#pragma region CL
	CL_LOGIN = 0xB000,
	CL_SIGNUP = 0xB001,
	CL_DUMMY_LOGIN = 0xB002,
	CL_FIND_ROOM = 0xB003,
	CL_UPDATE_USER_INFO = 0xB004,
	CL_REQUEST_USER_INFO = 0xB005,
	CL_CANCEL_AUTOMATCH = 0xB006,
#pragma endregion
#pragma region BC
	BC_BATTLE_LOGIN_OK = 0xC000,
	BC_BATTLE_LOGIN_FAIL = 0xC001,
	BC_JOIN_OK = 0xC002,
	BC_JOIN_FAIL = 0xC003,
	BC_ROOM_ENTERED = 0xC004,
	BC_ROOM_LEAVED = 0xC005,
	BC_NEW_ROOM_HOST = 0xC006,
	BC_READY = 0xC007,
	BC_ROOM_START = 0xC008,
	BC_ROOM_START_AVAILABLE = 0xC009,
	BC_GAME_OVER = 0xC010,
	BC_LEFT_TIME = 0xC011,
	BC_ROUND_START = 0xC012,
	BC_ADD_COIN = 0xC013,
	BC_UPDATE_COIN = 0xC014,
	BC_PLAYER_POS = 0xC015,
	BC_PLAYER_ROT = 0xC016,
	BC_OBJECT_POS = 0xC017,
	BC_OBJECT_ROT = 0xC018,
	BC_ANIM = 0xC019,
	BC_SHOOT_BULLET = 0xC020,
	BC_REMOVE_BULLET = 0xC021,
	BC_HIT = 0xC022,
	BC_DIE = 0xC023,
	BC_UPDATED_USER_INFO = 0xC024,
#pragma endregion
#pragma region CB
	CB_LOGIN = 0xD000,
	CB_JOIN = 0xD001,
	CB_ROOM_LEAVE = 0xD002,
	CB_READY = 0xD003,
	CB_START = 0xD004,
	CB_GET_COIN = 0xD005,
	CB_BULLET = 0xD006,
	CB_POSITION_VECTOR = 0xD007,
	CB_LOOK_VECTOR = 0xD008,
	CB_MAKE_ANIM = 0xD009,
	CB_TEST_TIME_PLUS = 0xD010,
	CB_TEST_TIME_MINUS = 0xD011,
#pragma endregion
};

struct packet_basic {
	PacketSize size;
	PacketType type;
};

#pragma region L->C (+L->B, B->L)
#pragma pack(push, 1)
struct lc_packet_login_ok {
	PacketSize size;
	PacketType type;

	int id;
};
struct lc_packet_login_fail {
	PacketSize size;
	PacketType type;
};
struct lc_packet_signup_ok {
	PacketSize size;
	PacketType type;
};
struct lc_pakcet_signup_fail {
	PacketSize size;
	PacketType type;
};
struct lc_packet_userinfo {
	PacketSize size;
	PacketType type;

	char id_str[MAX_ID_LEN];
	int mmr;
};
struct lc_packet_find_room {
	PacketSize size;
	PacketType type;

	short roomNum;
	bool isHost;
};
struct lc_packet_cancel_find_room {
	PacketSize size;
	PacketType type;
};
struct lb_packet_request_room {
	PacketSize size;
	PacketType type;
};
struct bl_packet_room_ready {
	PacketSize size;
	PacketType type;

	short id;
	int room_no;
	bool is_host;
};
#pragma pack(pop)
#pragma endregion

#pragma region C->L
#pragma pack(push, 1)
struct cl_packet_login {
	PacketSize size;
	PacketType type;

	char id[MAX_ID_LEN];
	char pw[MAX_ID_LEN];
};
struct cl_pakcet_signup {
	PacketSize size;
	PacketType type;

	char id[MAX_ID_LEN];
	char pw[MAX_ID_LEN];
};
struct cl_packet_dummy_login {
	PacketSize size;
	PacketType type;

	char id[MAX_ID_LEN];
};
struct cl_packet_find_room {
	PacketSize size;
	PacketType type;
};
struct cl_packet_update_user_info {
	PacketSize size;
	PacketType type;

	int mmr;
};
struct cl_packet_request_userinfo {
	PacketSize size;
	PacketType type;
};
struct cl_packet_cancel_automatch {
	PacketSize size;
	PacketType type;
};
#pragma pack(pop)
#pragma endregion

#pragma region B->C
#pragma pack(push, 1)
struct PTC_START_INFO {
	int id;
	DirectX::XMFLOAT4 spawn_pos;
};
struct PTC_VECTOR {
	float x;
	float y;
	float z;
};
struct PTC_ROOM {
	short id;
	char joinable;
};
struct bc_packet_bb {
	PacketSize size;
	PacketType type;

	DirectX::XMFLOAT4X4 mats;
	DirectX::XMFLOAT3 sizes;
};
struct bc_packet_battle_login_ok {
	PacketSize size;
	PacketType type;

	int id;
};
struct bc_packet_battle_login_fail {
	PacketSize size;
	PacketType type;
};
struct bc_packet_join_ok {
	PacketSize size;
	PacketType type;

	char player_no;
};
struct bc_packet_join_fail {
	PacketSize size;
	PacketType type;

	char code; //fail code
};
struct bc_packet_room_entered {
	PacketSize size;
	PacketType type;

	int id;
	bool ready;
	char player_no;

	char name[10]; //not used
	int mmr;
	char isManager;
};
struct bc_packet_room_leaved {
	PacketSize size;
	PacketType type;

	int id;
};
struct bc_packet_new_room_host {
	PacketSize size;
	PacketType type;

	int id;
};
struct bc_packet_ready {
	PacketSize size;
	PacketType type;

	int id;
	bool ready;
};
struct bc_packet_room_start {
	PacketSize size;
	PacketType type;

	PTC_START_INFO start_info[2];
};
struct bc_packet_room_start_available {
	PacketSize size;
	PacketType type;

	bool available;
};
struct bc_packet_game_over {
	PacketSize size;
	PacketType type;

	int win_team;
};
struct bc_packet_add_coin {
	PacketSize size;
	PacketType type;

	PTC_VECTOR pos;
	int coin_id;
};
struct bc_packet_update_coin {
	PacketSize size;
	PacketType type;

	int id;
	int coin_cnt;
	int delete_coin_id;
};
struct bc_packet_left_time {
	PacketSize size;
	PacketType type;

	unsigned char left_time;
};
struct bc_packet_round_start {		//game start
	PacketSize size;
	PacketType type;
};
struct bc_packet_player_pos {
	PacketSize size;
	PacketType type;

	int id;
	PTC_VECTOR pos;
};
struct bc_packet_player_rot {
	PacketSize size;
	PacketType type;

	int id;
	PTC_VECTOR look;
};
struct bc_packet_object_pos {
	PacketSize size;
	PacketType type;

	short type_id;
	int obj_id;
	PTC_VECTOR pos;
};
struct bc_packet_object_rot {
	PacketSize size;
	PacketType type;

	short type_id;
	float rot_y;
};
struct bc_packet_shoot_bullet {
	PacketSize size;
	PacketType type;

	char bullet_id;
	PTC_VECTOR pos;
	PTC_VECTOR look;
};
struct bc_packet_remove_bullet {
	PacketSize size;
	PacketType type;

	char bullet_id;
	PTC_VECTOR pos; //사라지는 위치 이팩트 발생 위치
};
struct bc_packet_hit {
	PacketSize size;
	PacketType type;

	int id;
	float hp;
};
struct bc_packet_die {
	PacketSize size;
	PacketType type;

	int id;
};
struct bc_packet_anim_type {
	PacketSize size;
	PacketType type;

	int id;
	int anim_type;
};
struct bc_packet_updated_user_info {
	PacketSize size;
	PacketType type;

	int mmr;
};
#pragma pack(pop)
#pragma endregion

#pragma region C->B
#pragma pack(push,1)
struct cb_packet_login {
	PacketSize size;
	PacketType type;

	char name[10];
	int mmr;
};
struct cb_packet_join {
	PacketSize size;
	PacketType type;

	int room_no;
	char is_roomMnr;
};
struct cb_packet_room_leave {
	PacketSize size;
	PacketType type;
};
struct cb_packet_ready {
	PacketSize size;
	PacketType type;
};
struct cb_packet_start {
	PacketSize size;
	PacketType type;
};
struct cb_packet_position {
	PacketSize size;
	PacketType type;

	int id;
	PTC_VECTOR pos;
	int anim_type;
};
struct cb_packet_get_coin {
	PacketSize size;
	PacketType type;

	int id;
	int coin_id;
};
struct cb_packet_anim {
	PacketSize size;
	PacketType type;

	int id;
	int anim_type;
};
struct cb_packet_bullet {
	PacketSize size;
	PacketType type;

	PTC_VECTOR dir; //look angle
};
struct cb_packet_look_vector {
	PacketSize size;
	PacketType type;

	int id;
	PTC_VECTOR look;
};
struct cb_test_packet_time_plus {
	PacketSize size;
	PacketType type;
};
struct cb_test_packet_time_mimus {
	PacketSize size;
	PacketType type;
};
#pragma pack(pop)
#pragma endregion