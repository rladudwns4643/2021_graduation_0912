#pragma once
#include "MISC.h"

constexpr int SERVER_PORT = 3500;

constexpr int SC_LOGIN_OK = 1;
constexpr int SC_READY = 2;
constexpr int SC_GAMESTART = 3;
constexpr int SC_POS = 4;
constexpr int SC_SKILL = 5;
constexpr int SC_GAMEOVER = 6;
//******************************
constexpr int CS_READY = 1;
constexpr int CS_MOVE = 2;
constexpr int CS_SKILL = 3;
constexpr int CS_JUMP = 4;

#pragma pack(push,1)
class Message {
public:
	char type;
	char id;
	char data;
};

class Protocol_Player {
public:
	char id;
	float posX;
	float posY;
};

class sc_packet_login_ok {
public:
	short size;
	char type;
	char id;
};

class sc_packet_player_in {
public:
	short size;
	char type;
	Protocol_Player players[2];
};

class sc_packet_player_out {
public:
	short size;
	char type;
	char id;
};

class sc_packet_ready {
public:
	short size;
	char type;
	char id;
	bool ready;
};

class sc_packet_game_start {
public:
	short size;
	char type;
	Protocol_Player players[2];
};

class sc_packet_gameover {
public:
	short size;
	char type;
	char id;	//win
};

class cs_packet_move {
public:
	short size;
	char type;
	bool isL;
};

class cs_packet_jump {
public:
	short size;
	char type;
};

class cs_packet_skill {
public:
	short size;
	char type;
	char id;	//target
};

class cs_packet_ready {
public:
	short size;
	char type;
};
#pragma pack(pop)