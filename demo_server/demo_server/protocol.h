#pragma once
#include "../../demo_battle/demo_battle/MathHelper.h"
//------------------------------------------------------------------
#define LOBBY_SERVER_PORT	7237
#define BATTLE_SERVER_PORT	7238

#define LOBBY_SERVER_IP_PUBLIC "121.170.59.45"
//------------------------------------------------------------------

//------------------------------------------------------------------
#define MAX_ID_LEN		50
#define MAX_STR_LEN		50
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

//-------------------------------L->C-------------------------------
//6
#define LC_LOGIN_OK					10
#define LC_LOGIN_FAIL				11
#define LC_SIGNUP_OK				12
#define LC_SIGNUP_FAIL				13
#define LC_USERINFO					14
#define LC_CANCEL_AUTOMATCH_SUCCESS 15
#define LC_MATCHSTART				16

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

struct lc_packet_cancel_automatch_success {
	BYTE size;
	BYTE type;
};

struct lc_packet_startMatch {
	BYTE size;
	BYTE type;
};
#pragma pack(pop)
//------------------------------------------------------------------


//-------------------------------C->L-------------------------------
//7
#define CL_LOGIN					20
#define CL_SIGNUP					21
#define CL_DUMMY_LOGIN				99
#define CL_AUTOMATCH				22
#define CL_UPDATE_USER_INFO			23
#define CL_REQUEST_USER_INFO		24
#define CL_CANCEL_AUTOMATCH			25

#pragma pack(push, 1)
struct cl_pakcet_login {
	BYTE size;
	BYTE type;

	char id[MAX_ID_LEN];
};

struct cl_pakcet_signup {
	BYTE size;
	BYTE type;

	char id[10];
	char password[10];
};

struct cl_packet_dummy_login {
	BYTE size;
	BYTE type;

	char id[MAX_ID_LEN];
};

struct cl_packet_automatch {
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
#define BC_AUTO_ACCEPT_OK		1
#define BC_AUTO_ACCEPT_FAIL		2
#define BC_JOIN_OK				3
#define BC_JOIN_FAIL			4

#define BC_ROOM_ENTERED			5
#define BC_ROOM_LEAVED			6
#define BC_NEW_ROOM_HOST		7

#define BC_READY				8
#define BC_GAME_START			9
#define BC_GAME_START_AVAILABLE	10
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

#pragma pack(push, 1)
struct PTC_START_INFO {
	int id;
	char rule;
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

struct bc_packet_accept_ok {
	BYTE size;
	BYTE type;

	int id;
};

struct bc_packet_accept_fail {
	BYTE size;
	BYTE type;
};

struct bc_packet_join_ok {
	BYTE size;
	BYTE type;

	bool isRoomMnr;
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

struct bc_pakcet_ready {
	BYTE size;
	BYTE type;

	int id;
	bool ready;
};

struct bc_packet_game_start {
	BYTE size;
	BYTE type;
};

struct bc_packet_gamestart_available {
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

struct bc_pakcet_player_pos {
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

struct bc_pakcet_shoot_bullet {
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

struct bc_pakcet_hit {
	BYTE size;
	BYTE type;

	int id;
	float hp;
};

struct bc_pakcet_die {
	BYTE size;
	BYTE type;

	int id;
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

//#define CB_MOVE_KEY_STATUS		6
#define CB_KEY_W_UP				60
#define CB_KEY_W_DOWN			61
#define CB_KEY_A_UP				62
#define CB_KEY_A_DOWN			63
#define CB_KEY_S_UP				64
#define CB_KEY_S_DOWN			65
#define CB_KEY_D_UP				66
#define CB_KEY_D_DOWN			67
#define CB_KEY_JUMP				68

#define CB_BULLET				7
#define CB_LOOK_VECTOR			9
#define CB_TEST_TIME_PLUS		10
#define CB_TEST_TIME_MINUS		11

#pragma pack(push, 1)
struct cb_packet_login {
	BYTE size;
	BYTE type;

	char name[10];
	int mmr;
	int totalGameCnt;
	int winningGameCnt;
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
struct cb_packet_move_key_status {
	BYTE size;
	BYTE type;

	BYTE key;	//CB_KEY_@_DOWN, UP
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

//-------------------------------M->L------------------------------- 
//x
struct ml_packet_match {
	BYTE size;
	BYTE type;

	short id;
	short room_no;
	char is_host;
};

struct ml_packet_room_ready {
	BYTE size;
	BYTE type;

	short id;
	short room_no;
};
//------------------------------------------------------------------

//-------------------------------L->B------------------------------- 
#define LB_REQUEST_ROOM 0

struct lb_packet_request_room {
	BYTE size;
	BYTE type;
};
//------------------------------------------------------------------

//-------------------------------B->L-------------------------------
//1

#define BL_ROOMREADY			1
#pragma pack (push, 1)
struct bl_pakcet_room_ready {
	BYTE size;
	BYTE type;

	int room_no;
};
//------------------------------------------------------------------
#pragma pack (pop)