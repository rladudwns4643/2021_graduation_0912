#pragma once
#include "../../demo_battle/demo_battle/SMathHelper.h"
//------------------------------------------------------------------
#define LOBBY_SERVER_PORT	7237
#define BATTLE_SERVER_PORT	7238

//동석
//#define LOBBY_SERVER_IP_PUBLIC "182.229.10.168"
//#define BATTLE_SERVER_IP_PUBLIC "182.229.10.168"
//영준
#define LOBBY_SERVER_IP_PUBLIC "121.170.59.45"
#define BATTLE_SERVER_IP_PUBLIC "121.170.59.45"
//------------------------------------------------------------------

//------------------------------------------------------------------
#define MAX_ID_LEN		10
#define MAX_STR_LEN		10
//------------------------------------------------------------------


// BYTE : unsigned char : 0 ~ 255

//------------------------------------------------------------------
//#define PACKET_TYPE_NON_BUFFERED	0
//#define PACKET_TYPE_BUFFERED		1
//------------------------------------------------------------------

//------------------------------------------------------------------
#define START_MMR		1000
//------------------------------------------------------------------
//OBJECT TYPE
#define OBJECT_TYPE_TOY				1
#define OBJECT_TYPE_BULLET			2

#define OBJECT_START_INDEX_BULLET	1000
#define MAX_BULLET_COUNT			20
//------------------------------------------------------------------

//------------------------------------------------------------------
//ROOM JOIN FAIL
#define ROOM_FAIL_CODE_ROOM_NOT_FOUND	0
#define ROOM_FAIL_CODE_ROOM_IS_FULL		1
#define ROOM_FAIL_CODE_ROOM_STARTED		2
//------------------------------------------------------------------

//------------------------------------------------------------------
//ANIMATION TYPE
#define ANIM_IDLE		0
#define ANIM_JUMP		1
#define ANIM_MOVE		2
#define ANIM_ATTK		3
#define ANIM_TEST		4
//------------------------------------------------------------------

//------------------------------------------------------------------
#define MAX_ROOM_COUNT 5
//------------------------------------------------------------------


//-------------------------------L->C-------------------------------
//6
#define LC_LOGIN_OK					30
#define LC_LOGIN_FAIL				31
#define LC_SIGNUP_OK				32
#define LC_SIGNUP_FAIL				33
#define LC_USERINFO					34
#define LC_FIND_ROOM				35
#define LC_CANCLE_FIND_ROOM			36

#pragma pack(push, 1)
struct lc_packet_login_ok {
	BYTE size;
	BYTE type;

	int id;
};
struct lc_packet_login_fail {
	BYTE size;
	BYTE type;
};
struct lc_packet_userinfo {
	BYTE size;
	BYTE type;

	char id_str[MAX_ID_LEN];
	int mmr;
};
struct lc_packet_signup_ok {
	BYTE size;
	BYTE type;
};
struct lc_pakcet_signup_fail {
	BYTE size;
	BYTE type;
};
struct lc_packet_cancel_find_room {
	BYTE size;
	BYTE type;
};
struct lc_packet_find_room {
	BYTE size;
	BYTE type;

	short roomNum;
	bool isHost;
};
#pragma pack(pop)
//------------------------------------------------------------------


//-------------------------------C->L-------------------------------
//7
#define CL_LOGIN					20
#define CL_SIGNUP					21
#define CL_DUMMY_LOGIN				99
#define CL_FIND_ROOM				22
#define CL_UPDATE_USER_INFO			23
#define CL_REQUEST_USER_INFO		24
#define CL_CANCEL_AUTOMATCH			25

#pragma pack(push, 1)
struct cl_packet_login {
	BYTE size;
	BYTE type;

	char id[MAX_ID_LEN];
	char pw[MAX_ID_LEN];
};
struct cl_pakcet_signup {
	BYTE size;
	BYTE type;

	char id[MAX_ID_LEN];
	char pw[MAX_ID_LEN];
};
struct cl_packet_dummy_login {
	BYTE size;
	BYTE type;

	char id[MAX_ID_LEN];
};
struct cl_packet_find_room {
	BYTE size;
	BYTE type;
};
struct cl_packet_update_user_info {
	BYTE size;
	BYTE type;

	int mmr;
};
struct cl_packet_request_userinfo {
	BYTE size;
	BYTE type;
};
struct cl_packet_cancel_automatch {
	BYTE size;
	BYTE type;
};
#pragma pack(pop)
//------------------------------------------------------------------

//-------------------------------B->C-------------------------------
//22 + (bb, START_INFO, VECTOR, ROOM)
#define BC_BATTLE_LOGIN_OK		1
#define BC_BATTLE_LOGIN_FAIL	2
#define BC_JOIN_OK				3
#define BC_JOIN_FAIL			4

#define BC_ROOM_ENTERED			5
#define BC_ROOM_LEAVED			6
#define BC_NEW_ROOM_HOST		7

#define BC_READY				8
#define BC_ROOM_START			9
#define BC_ROOM_START_AVAILABLE	10
#define BC_GAME_OVER			11

#define BC_LEFT_TIME			12
#define BC_ROUND_START			13

#define BC_PLAYER_POS			14
#define BC_PLAYER_ROT			15
#define BC_OBJECT_POS			16
#define BC_OBJECT_ROT			17

#define BC_SHOOT_BULLET			18
#define BC_REMOVE_BULLET		19

#define BC_HIT					20
#define BC_DIE					21

#define BC_UPDATED_USER_INFO	22
#define BC_ANIM					23

#pragma pack(push, 1)
struct PTC_START_INFO {
	int id;
	XMFLOAT4 spawn_pos;
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
	BYTE size;
	BYTE type;

	XMFLOAT4X4 mats;
	XMFLOAT3 sizes;
};
struct bc_packet_battle_login_ok {
	BYTE size;
	BYTE type;

	int id;
};
struct bc_packet_battle_login_fail {
	BYTE size;
	BYTE type;
};
struct bc_packet_join_ok {
	BYTE size;
	BYTE type;

	char player_no;
};
struct bc_packet_join_fail {
	BYTE size;
	BYTE type;

	char code; //fail code
};
struct bc_packet_room_entered {
	BYTE size;
	BYTE type;

	int id;
	bool ready;
	char player_no;

	char name[10]; //not used
	int mmr;
	char isManager;
};
struct bc_packet_room_leaved {
	BYTE size;
	BYTE type;

	int id;
};
struct bc_packet_new_room_host {
	BYTE size;
	BYTE type;

	int id;
};
struct bc_packet_ready {
	BYTE size;
	BYTE type;

	int id;
	bool ready;
};
struct bc_packet_room_start {
	BYTE size;
	BYTE type;

	PTC_START_INFO start_info[2];
};
struct bc_packet_room_start_available {
	BYTE size;
	BYTE type;

	bool available;
};
struct bc_packet_game_over {
	BYTE size;
	BYTE type;

	char win_team;
};
struct bc_packet_left_time {
	BYTE size;
	BYTE type;

	unsigned char left_time;
};
struct bc_packet_round_start {		//game start
	BYTE size;
	BYTE type;
};
struct bc_packet_player_pos {
	BYTE size;
	BYTE type;

	int id;
	PTC_VECTOR pos;
};
struct bc_packet_player_rot {
	BYTE size;
	BYTE type;

	int id;
	PTC_VECTOR look;
};
struct bc_packet_object_pos {
	BYTE size;
	BYTE type;

	short type_id;
	int obj_id;
	PTC_VECTOR pos;
};
struct bc_packet_object_rot {
	BYTE size;
	BYTE type;

	short type_id;
	float rot_y;
};
struct bc_packet_shoot_bullet {
	BYTE size;
	BYTE type;

	char bullet_id;
	PTC_VECTOR pos;
	PTC_VECTOR look;
};
struct bc_packet_remove_bullet {
	BYTE size;
	BYTE type;

	char bullet_id;
	PTC_VECTOR pos; //사라지는 위치 이팩트 발생 위치
};
struct bc_packet_hit {
	BYTE size;
	BYTE type;

	int id;
	float hp;
};
struct bc_packet_die {
	BYTE size;
	BYTE type;

	int id;
};
struct bc_packet_anim_type {
	BYTE size;
	BYTE type;

	int id;
	char anim_type;
};
struct bc_packet_updated_user_info {
	BYTE size;
	BYTE type;

	int mmr;
};
#pragma pack(pop)
//------------------------------------------------------------------

//-------------------------------C->B-------------------------------
//20
#define CB_LOGIN				1
#define CB_JOIN					2
#define CB_ROOM_LEAVE			3
#define CB_READY				4
#define CB_START				5

#define CB_BULLET				7
#define CB_POSITION_VECTOR		8
#define CB_LOOK_VECTOR			9
#define CB_TEST_TIME_PLUS		10
#define CB_TEST_TIME_MINUS		11

#pragma pack(push, 1)
struct cb_packet_login {
	BYTE size;
	BYTE type;

	char name[10];
	int mmr;
};
struct cb_packet_join {
	BYTE size;
	BYTE type;

	int room_no;
	char is_roomMnr;
};
struct cb_packet_room_leave {
	BYTE size;
	BYTE type;
};
struct cb_packet_ready {
	BYTE size;
	BYTE type;
};
struct cb_packet_start {
	BYTE size;
	BYTE type;
};
//key
struct cb_packet_position {
	BYTE size;
	BYTE type;

	int id;
	PTC_VECTOR pos;
};
struct cb_packet_bullet {
	BYTE size;
	BYTE type;

	PTC_VECTOR dir; //look angle
};
struct cb_packet_look_vector {
	BYTE size;
	BYTE type;

	int id;
	PTC_VECTOR look;
};
struct cb_test_packet_time_plus {
	BYTE size;
	BYTE type;
};
struct cb_test_packet_time_mimus {
	BYTE size;
	BYTE type;
};
#pragma pack (pop)
//------------------------------------------------------------------

//-------------------------------L->B------------------------------- 
//1

#define LB_REQUEST_ROOM			31
#define LB_CHECK				32
#pragma pack (push, 1)
struct lb_packet_check_connect {
	BYTE size;
	BYTE type;
};
struct lb_packet_request_room {
	BYTE size;
	BYTE type;
};
#pragma pack (pop)
//------------------------------------------------------------------

//-------------------------------B->L-------------------------------
//1

#define BL_ROOMREADY			41
#define BL_CHECK				42
#pragma pack (push, 1)
struct bl_packet_room_ready {
	BYTE size;
	BYTE type;

	short id;
	int room_no;
	bool is_host;
};

struct bl_pakcet_check_connect {
	BYTE size;
	BYTE type;
};
//------------------------------------------------------------------
#pragma pack (pop)