#pragma once
//------------------------------------------------------------------
#define LOBBY_SERVER_PORT	7237
#define BATTLE_SERVER_PORT	7238

#define LOBBY_SERVER_IP_PUBLIC "121.170.59.45"
//------------------------------------------------------------------

//------------------------------------------------------------------
#define MAX_ID_LEN		50
#define MAX_STR_LEN		255
//------------------------------------------------------------------


// BYTE : unsigned char : 0 ~ 255

//------------------------------------------------------------------
#define PACKET_TYPE_NON_BUFFERED	0
#define PACKET_TYPE_BUFFERED		1
 
#define LC_LOGIN_OK				10
#define LC_LOGIN_FAIL			11
#define LC_USERINFO				12
#define LC_MATCHSTART			13

#define CL_LOGIN				20
#define CL_DUMMY_LOGIN			99
#define CL_REQUEST_USER_INFO	21
#define CL_UPDATE_USER_INFO		22
#define CL_AUTOMATCH			23

//------------------------------------------------------------------

//------------------------------------------------------------------
#define START_MMR		1000
//------------------------------------------------------------------


#pragma pack(push, 1)
//-------------------------------L->C-------------------------------
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

struct lc_packet_startMatch {
	BYTE size;
	BYTE type;

	BYTE roomNo;
};

//------------------------------------------------------------------

//-------------------------------C->L-------------------------------
struct cl_pakcet_login {
	BYTE size;
	BYTE type;

	char id[MAX_ID_LEN];
};

struct cl_packet_dummy_login {
	BYTE size;
	BYTE type;

	char id[MAX_ID_LEN];
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

struct cl_packet_automatch {
	BYTE size;
	BYTE type;
};
//------------------------------------------------------------------
#pragma pack (pop)

/*******************************/

#define WORLD_WIDTH		8
#define WORLD_HEIGHT	8

#define SERVER_PORT		9000

#define C2S_LOGIN	1
#define C2S_MOVE	2

#define S2C_LOGIN_OK		1
#define S2C_MOVE			2
#define S2C_ENTER			3
#define S2C_LEAVE			4
/*******************************/