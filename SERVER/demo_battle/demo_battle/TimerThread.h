#pragma once
#include "MyThread.h"

class TimerThread final : public MyThread {
public:
	void InitThread() override;

	void ProcThread() override;
	void JoinThread() override;
};