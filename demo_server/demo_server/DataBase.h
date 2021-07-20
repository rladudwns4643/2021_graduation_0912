#pragma once
#include <iostream>
#include <string>
#include <windows.h>
#include <sqlext.h>
#include <codecvt>
#include <assert.h>
#include "Singleton.h"

constexpr auto NAME_LEN = 11;

class DataBase : public Singleton<DataBase> {
private:
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt{ 0 };
	SQLRETURN retcode;

private:
	SQLINTEGER nID;
	SQLLEN cbID;

	SQLINTEGER nMMR;
	SQLLEN cbMMR;

	SQLINTEGER nCount; //for id check
	SQLLEN cbCount;
public:
	void Init();
	DataBase();
	~DataBase();
	void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
	void show_error();

	bool LoginPlayer(std::string id, std::string pw, int* mmr);
	bool SignUpPlayer(std::string id, std::string pw);
	bool CheckId(std::string id, int* is_exist);

	bool GetUserInfo(std::string& id, int& mmr);
	void SetUserInfo(std::string& id, int& mmr);

	std::wstring StringToWstring(const std::string& str);

	bool retSuccess(SQLRETURN& ret);
};