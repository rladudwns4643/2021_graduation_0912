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


//------------------------------------------------------------------
#define PACKET_TYPE_NON_BUFFERED	0
#define PACKET_TYPE_BUFFERED		1

#define LC_LOGIN_OK			10
#define LC_LOGIN_FAIL		11
#define LC_USERINFO			12

#define CL_LOGIN			20
#define CL_DUMMY_LOGIN		29
#define CL_UPDATE_USER_INFO	21
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
#pragma pack(push ,1)

struct sc_packet_login_ok {
	char size;
	char type;
	int id;
	short x, y;
	short hp;
	short level;
	int	exp;
};

struct sc_packet_move {
	char size;
	char type;
	int id;
	short x, y;
};

constexpr unsigned char O_PLAYER = 0;
constexpr unsigned char O_NPC = 1;

struct sc_packet_enter {
	char size;
	char type;
	int id;
	char name[MAX_ID_LEN];
	char o_type;
	short x, y;
};

struct sc_packet_leave {
	char size;
	char type;
	int id;
};

struct cs_packet_login {
	char	size;
	char	type;
	char	name[MAX_ID_LEN];
};

constexpr unsigned char D_UP = 0;
constexpr unsigned char D_DOWN = 1;
constexpr unsigned char D_LEFT = 2;
constexpr unsigned char D_RIGHT = 3;

struct cs_packet_move {
	char	size;
	char	type;
	char	direction;
};

#pragma pack (pop)