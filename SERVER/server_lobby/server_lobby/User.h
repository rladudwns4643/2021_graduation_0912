#pragma once
#include "pch.h"
#include "Global.h"

constexpr int UNSET_MMR = -1;
constexpr int UNSET_ID = -1;

class User {
public:
	User() {
		name = "NULL";
		mmr = UNSET_MMR;
		lobbyID = LOBBY_ID;
		is_Match = false;
	}
	~User() {};

	void SetPlayerLoginOk(std::string _name) { name = _name; };
	std::string GetPlayerID() { return name; };
	void SetPlayerMatch(bool _match) { is_Match = _match; }
	bool GetPlayerMatch() { return is_Match; };
	void SetPlayerMMR(int _mmr) { mmr = _mmr; };
	int GetPlayerMMR() { return mmr; };

private:
	std::string name;
	int mmr;
	short lobbyID;
	bool is_Match;
};