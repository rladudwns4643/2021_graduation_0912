#include "Global.h"


Global::Global() {
	TimerTick = 0;
	ShadowTick_1 = 0;
	ShadowTick_2 = 0;
	tick_spawn = 80;
	Gameover_1 = false;
	Gameover_2 = false;
	isStart = false;
	Player1HeroKind = HERO::Hero1;
	Player2HeroKind = HERO::Hero1;
}

Global* Global::inst = nullptr;