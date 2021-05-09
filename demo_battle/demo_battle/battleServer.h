#pragma once
#include "pch.h"
#include "Global.h"
#include "Singleton.h"
#include "ThreadHandler.h"

class ThreadHandler;

class BattleServer : public Singleton<BattleServer> {
public:
	BattleServer();
	~BattleServer();

	void Initialize();
	void ConncetLobbyServer();
	void AcceptLobbyServer();
	void Run();

	void error_display(const char*, int);
	void AddTimer(EVENT& ev);

public:
	SOCKADDR_IN GetServerAdder();

public:
	void SendPacket(int id, void* buff);
	void SendCheckConnect();
	void SendAutoAccessOKPacket(int id);
	void SendAutoAccessFailPacket(int id);
	void SendAutoRoomReadyPacket(int id, int room_no);
	void SendRoomJoinSuccess(int id, bool isRoomMnr);
	void SendRoomJoinFail(int id, int code);
	void SendRoomEnterPacket(int to, int enterer, bool ready, char playerNo, char* name, int mmr, bool isManager);
	void SendRoomLeavePacket(int to, int leave);
	void SendGameStartPacket(int id, PTC_START_INFO* player_info);
	
	//¿Œ∞‘¿”
	void SendRoundStartPacket(int id);
	void SendPlayerRotation(int to, int from, PTC_VECTOR look);
	void SendLeftTimePacket(int id, char left_time);
	void SendShootPacket(int id, int bullet_id, PTC_VECTOR look);
	void SendGameOverPacket(int id, int winner_id);

	void SendUpdateUserInfoPacket(const int& id, const int& mmr);

private:
	ThreadHandler* m_ThreadHandler;
	SocketUtil m_sockUtil;
	TCPSocketPtr m_listen;

	SOCKADDR_IN m_ServerAddr{};
};