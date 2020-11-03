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

#pragma region 기본설정
static HINSTANCE g_hInst;						// 현재 인스턴스
static LPCTSTR lpszTitle = TEXT("EVAID TITLE");
static LPCTSTR lpszClass = TEXT("EVAID CLASS");
#pragma endregion

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void CALLBACK TimerProc(HWND hWnd, UINT iMessage, UINT_PTR idEvent, DWORD dwTime);

#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")

using std::cout;
using std::endl;


//client
#define CLIENT_WIDTH	1200	// 너비
#define CLIENT_HEIGHT	900		// 높이

//child
#define STAGE_WIDTH		600
#define STAGE_HEIGHT	900

// 표 시작 포인트
#define PTStartX 100
#define PTStartY 50

//timer
#define timer_Main 1			
#define tick_Main 10		//(ms)
#define tick_block 2
#define tick_skill 5
#define tick_targeting 10

//Table Size setting
#define table_WIDTH 10
#define table_HEIGHT 20

//Block Size setting (정사각형이며 이미지 파일과 크기를 맞춰야함)
const int BLOCK_SIZE = 40;
const int HERO_SIZE = 40;

// 라디안 값
#define Radian (double)0.017453292519943295769236907684886

// shadow
#define shadowDelay (double)2

// 히어로 시작위치
const int HeroStartPosX = 300;
const int HeroStartPosY = 630;

// 히어로 떨어지는 속도
#define Gravity 8

const int UNSETVAL = -1234;

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