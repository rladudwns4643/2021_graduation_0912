#pragma once
#include "pch.h"
#include "Global.h"
#include "Singleton.h"
#include "../../demo_server/demo_server/protocol.h"

class ThreadMnr;

class BattleServer : public Singleton<BattleServer> {
public:
	BattleServer();
	~BattleServer();

	void Initialize();
	void AcceptLobbyServer();
	void Run();

	void error_display(const char*, int);
	void AddTimer(EVENT& ev);

public:
	SOCKADDR_IN GetServerAdder();

public:
	void SendPacket(int id, void* buff);
	void SendAccessOKPacket(int id, char match_type);
	void SendAccessFailPacket(int id);
	void SendAutoRoomReadyPacket(int id, int room_no);
	void SendRoomJoinSuccess(int id, int slot);
	void SendRoomJoinFail(int id, int code);
	void SendMapPacket(int id);
	void SendRoomEnterPacket(int to, int enterer, bool ready, char playerNo, const char* char_type, char* name,
		int mmr, int catchCnt, float w_rate, int totalCnt, int winCnt, bool isManager);
	void SendRoomLeavePacket(int to, int from);
	void SendGameStartPacket(int id, PTC_START_INFO* player_info);
	
	//¿Œ∞‘¿”
	void SendRoundStartPacket(int id);
	void SendPlayerRotation(int to, int from, PTC_VECTOR look);
	void SendLeftTimePacket(int id, char left_time);
	void SendShootPacket(int id, PTC_VECTOR look);
	void SendGameOverPacket(int id, int winner_id);

	void SendUpdateUserInfoPacket(const int& id, const int& mmr);

private:
	ThreadMnr* m_ThreadHandler;
	SocketUtil m_sockUtil;
	TCPSocketPtr m_listen;

	SOCKADDR_IN m_ServerAddr{};

	ofstream m_logfile{ "LOG.txt", ios::app };
};