#pragma once
#include "pch.h"
#include "Global.h"

constexpr int UNSETMMR = -1;
constexpr int UNSETID = -1;

class User {
public:
	User() {
		id = UNSETID;
		mmr = UNSETMMR;
		lobbyID = LOBBY_ID;
		isMatching = false;
	}
	~User() {};

	void SetPlayerLoginOK(int _id) { id = _id; };
	int GetPlayerID() { return id; };
	
	void SetPlayerMMR(int _mmr) { mmr = _mmr; };
	int GetPlayerMMR() { return mmr; };

	void SetPlayerMatch(bool _m) { isMatching = _m; };
	bool GetPlayerMatch() { return isMatching; };

private:
	int id;
	int mmr;
	short lobbyID;
	bool isMatching;
};