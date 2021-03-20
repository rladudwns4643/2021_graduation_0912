#pragma once

#include "../../demo_server/demo_server/protocol.h"
#include "Global.h"
#include "Singleton.h"

class ThreadHandler;

class BattleServer : public Singleton<BattleServer> {
public:
	BattleServer();
	~BattleServer();

	void Initialize();
	void Run();

	void error_display(const char*, int);
	void AddTimer(EVENT& ev);

public:
};