#pragma once
#include "pch.h"

namespace SR {
	extern std::map<ID, MatchInfo> g_match_list;
	extern SOCKET g_battleSocket;
}

namespace ATOMIC {
	extern std::mutex g_match_list_lock;
}