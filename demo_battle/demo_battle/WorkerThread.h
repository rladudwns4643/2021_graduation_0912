#pragma once
#include "pch.h"
#include "MyThread.h"
#include "Global.h"
#include "battleServer.h"
#include "Room.h"

class WorkerThread : public MyThread {
public:
	void InitThread() override;
	void ProcThread() override;
	void JoinThread() override;
	message ProcPacket(int id, void* buf);

public:
	void DisconnectClient(int clientId, SOCKET client);
	void EraseRoom(int& roomId);

private:
	SOCKADDR_IN m_ServerAddr;
	int m_AddrLen;
};