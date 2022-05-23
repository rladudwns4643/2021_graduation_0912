#pragma once
#include "ThreadInterface.h"
#include "extern.h"

class TimerThread final : public ThreadInterface {
public:
	void InitThread() override;
	void ProcThread() override;
	void JoinThread() override;

private:
	bool MatchingUser();
	void SendRequestRoomPacket(int id_1, int id_2);
	void SendPacket(void* buf);
};