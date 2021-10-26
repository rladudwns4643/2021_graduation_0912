#pragma once
#include "pch.h"

constexpr int UNSETMMR = -1;
constexpr int UNSETID = -1;

class User {
public:
	User() {
		name = "null";
		mmr = UNSETMMR;
		lobbyID = UNSETID;
		isMatching = false;
	}
	~User() {};

	void SetPlayerLoginOK(const std::string& _name) { name = _name; };
	std::string GetPlayerID() const { return name; };
	
	void SetPlayerMMR(const int& _mmr) { mmr = _mmr; };
	int GetPlayerMMR() const { return mmr; };

	void SetPlayerMatch(const bool& _m) { isMatching = _m; };
	bool GetPlayerMatch() const { return isMatching; };

private:
	std::string name;
	int mmr;
	short lobbyID;
	bool isMatching;
};