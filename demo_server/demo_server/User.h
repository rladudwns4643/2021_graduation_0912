#pragma once
#include "pch.h"
#include "Global.h"

constexpr int UNSETMMR = -1;
constexpr int UNSETID = -1;

class User {
public:
	User() {
		name = "null";
		mmr = UNSETMMR;
		lobbyID = LOBBY_ID;
		isMatching = false;
		isAllow = false;
	}
	~User() {};

	void SetPlayerLoginOK(std::string _name) { name = _name; isAllow = false; };
	std::string GetPlayerID() { return name; };
	
	void SetPlayerMMR(int _mmr) { mmr = _mmr; };
	int GetPlayerMMR() { return mmr; };

	void SetPlayerMatch(bool _m) { isMatching = _m; };
	bool GetPlayerMatch() { return isMatching; };

private:
	std::string name;
	int mmr;
	short lobbyID;
	bool isMatching;
	bool isAllow;
};