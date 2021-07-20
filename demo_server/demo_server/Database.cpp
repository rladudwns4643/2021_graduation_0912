#include "DataBase.h"

void DataBase::Init()
{
	setlocale(LC_ALL, "korean");
	std::cout << "INIT: DB\n";
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (!retSuccess(retcode)) assert(!"db: 1 ");
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
	if (!retSuccess(retcode)) assert(!"db: 2 ");
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	if (!retSuccess(retcode)) assert(!"db: 3 ");
	SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
	//retcode = SQLConnect(hdbc, (SQLWCHAR*)L"ToyGround_DB", SQL_NTS, (SQLWCHAR*)NULL, SQL_NTS, NULL, SQL_NTS);
	retcode = SQLConnect(hdbc, (SQLWCHAR*)L"ToyGround_DB", SQL_NTS, (SQLWCHAR*)L"admin", SQL_NTS, (SQLWCHAR*)L"210817", SQL_NTS);
	if (!retSuccess(retcode)) assert(!"db: 4 ");
	std::cout << "DB CONNECT\n";
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
}

DataBase::DataBase()
{
	Init();
}

DataBase::~DataBase()
{
	SQLCancel(hstmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
}

void DataBase::HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER iError;
	WCHAR wszMessage[1000];
	WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
	if (RetCode == SQL_INVALID_HANDLE) {
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5)) {
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
}

void DataBase::show_error()
{
	std::cout << "error\n";
}

bool DataBase::LoginPlayer(std::string id, std::string pw, int* mmr) {
	std::wstring q{ L"EXEC login_player " };
	q += StringToWstring(id);
	q += L", ";
	q += StringToWstring(pw);

	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)q.c_str(), SQL_NTS);
	if (retSuccess(retcode)) {
		retcode = SQLBindCol(hstmt, 1, SQL_INTEGER, &nMMR, 10, &cbMMR);
		retcode = SQLFetch(hstmt);
		if (retcode == SQL_ERROR) assert(!"LoginPlayer");
		if (retSuccess(retcode)) {
			*mmr = nMMR;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			return true;
		}
		else {
			std::cout << "no player in db\n";
			return false;
		}
	}
	return false;
}

bool DataBase::SignUpPlayer(std::string id, std::string pw)
{
	std::wstring q{ L"EXEC signup_player " };
	q += StringToWstring(id);
	q += L", ";
	q += StringToWstring(pw);

	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)q.c_str(), SQL_NTS);
	return retSuccess(retcode);
}

bool DataBase::CheckId(std::string id, int* is_exist)
{
	std::wstring q{ L"EXEC check_id " };
	q += StringToWstring(id);
	std::cout << "check id: " << id << std::endl;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)q.c_str(), SQL_NTS);
	if (retSuccess(retcode)) {
		retcode = SQLBindCol(hstmt, 1, SQL_INTEGER, &nCount, 10, &cbCount);
		retcode = SQLFetch(hstmt);
		if (retcode == SQL_ERROR) assert(!"checkid");
		if (retSuccess(retcode)) {
			*is_exist = nCount;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			return true;
		}
		else {
			std::cout << "fail to find id\n";
			return false;
		}
	}
	return false;
}

bool DataBase::GetUserInfo(std::string& id, int& mmr)
{
	std::wstring q{ L"EXEC get_user_info " };
	//std::wstring tmp{};
	//tmp.assign(id.begin(), id.end()); //유니코드 -> 멀티바이트(string -> wstring)
	q += StringToWstring(id);
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)q.c_str(), SQL_NTS);
	if (retSuccess(retcode)) {
		retcode = SQLBindCol(hstmt, 1, SQL_INTEGER, &nMMR, 4, &cbMMR);
		if (retcode != SQL_SUCCESS) {
			HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
		}
		retcode = SQLFetch(hstmt); //bind 된 값 받아옴
		if (retcode != SQL_ERROR) assert(!"get_user_info");
		if (retSuccess(retcode)) {
			mmr = nMMR;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			return true;
		}
		else {
			std::cout << "none id\n";
			return false;
		}
	}
	else {
		HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
		return false;
	}
}

void DataBase::SetUserInfo(std::string& id, int& mmr)
{
	std::wstring q{L"EXEC set_user_info "};
	q += 
		L", " + StringToWstring(id) 
		+ L", " + std::to_wstring(mmr);
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	if (retSuccess(retcode)) {
		std::cout << "set_user_info: " << id << std::endl;
		if (retcode != SQL_SUCCESS) {
			HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
		}
		if (retcode == SQL_ERROR) assert(!"set_user_info");
		else SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	}
	else {
		HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
	}
}

std::wstring DataBase::StringToWstring(const std::string& str)
{
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	return converter.from_bytes(str);
}

bool DataBase::retSuccess(SQLRETURN& ret) {
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) return true;
	else return false;
}