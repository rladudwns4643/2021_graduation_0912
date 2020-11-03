#pragma once
#include <Windows.h>
#include <atlimage.h>
#include <time.h>
#include <iostream>
#include <assert.h>
#include <vector>
#include <fstream>
#include "SoundManager.h"

using namespace std;

#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")

using std::cout;
using std::endl;

//client
constexpr auto CLIENT_WIDTH = 1200;	// 너비;
constexpr auto CLIENT_HEIGHT = 900;		// 높이

//child
constexpr auto STAGE_WIDTH = 600;
constexpr auto STAGE_HEIGHT = 900;

// 표 시작 포인트
constexpr auto  PTStartX = 100;
constexpr auto  PTStartY = 50;

//timer
constexpr auto timer_Main = 1;
constexpr auto tick_Main = 10;		//(ms)
constexpr auto tick_block = 2;
constexpr auto tick_skill = 5;
constexpr auto tick_targeting = 10;

//Table Size setting
constexpr auto table_WIDTH = 10;
constexpr auto table_HEIGHT = 20;

//Block Size setting (정사각형이며 이미지 파일과 크기를 맞춰야함)
constexpr auto BLOCK_SIZE = 40;
constexpr auto HERO_SIZE = 40;

// 라디안 값
#define Radian (double)0.017453292519943295769236907684886

// shadow
#define shadowDelay (double)2

// 히어로 시작위치
const int HeroStartPosX = 300;
const int HeroStartPosY = 630;

// 히어로 떨어지는 속도
#define Gravity 9.8

const auto UNSETVAL = -1234;

// 스킬관련
#define MaxSkillGauge 10
#define NormalSkillGaugeIncrement 1

class JSize {
public:
	int width;
	int height;
	void set(int w, int h) {
		width = w;
		height = h;
	}
	void set(int input) {
		width = input;
		height = input;
	}
};

class JPoint {
public:
	int x;
	int y;
	JPoint() {
		x = UNSETVAL;
		y = UNSETVAL;
	}
	JPoint(int _x, int _y) : x(_x), y(_y) {};
	~JPoint() {};
};

//떨어지는 블록종류
#define Block_Type_Count 5
enum class BLOCK_TYPE {
	 A
	,B
	,C
	,D
	,E
	,NONE
	,STACK		//쌓여진 블록
	,SHADOW		//그림자
};

// 상태
enum class STATE {
	 NORMAL
	,LEFT
	,RIGHT
};

// 히어로 종류
enum class HERO {
	Hero1,
	Hero2,
};

///0~MAX-1
static int getRandom(int MAX) {
	return rand() % MAX;
}

void GetKeyDown(WPARAM w);

void GetKeyUp(WPARAM w);

void OnDraw_1(HDC hDC);

void OnDraw_2(HDC hDC);