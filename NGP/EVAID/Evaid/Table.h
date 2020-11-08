#pragma once
#include "MISC.h"
#include "Global.h"
#include "ResorceTable.h"
#include "SoundManager.h"

//check 함수 사용 인자.
#define standard_line 3
#define delete_line 1


class CTable {
	RECT	rcVal[table_WIDTH][table_HEIGHT];

	int PlayerNum;

	BOOL	nowDrop = FALSE;

	BLOCK_TYPE	nextBlock;
	POINT	ptnextBlock{ 510, 50 };

	RECT	rcnextBlock{
		  ptnextBlock.x + BLOCK_SIZE / 2
		, ptnextBlock.y + BLOCK_SIZE / 2
		, ptnextBlock.x + BLOCK_SIZE / 2 + BLOCK_SIZE
		, ptnextBlock.y + BLOCK_SIZE / 2 + BLOCK_SIZE };


public:
	bool isShadowOn = false;
	BLOCK_TYPE	Val[table_WIDTH][table_HEIGHT] = { BLOCK_TYPE::NONE };

	//boder이 그려지기 시작하는 pt
	POINT	ptstart;

	// 흔들리는 정도
	int ShakeDegree;

	CTable(int);
	~CTable();

	void log();

	///<summary>
	/// 그리기 함수 (OnDraw에서 호출)
	/// 배경화면/ 보더/ 블럭/ 다음 블럭 및 다음 블럭 보더
	///</summary>
	void draw(HDC hDC);

	void drawBg(HDC hDC);

	void drawBorder(HDC hDC);

	void drawBlock(HDC hDC);

	void drawNextBlock(HDC hDC);

	void init();

	///<summary>
	/// 블럭을 한칸씩 내림(Timer에서 호출)
	///</summary>
	void drop();

	///<summary>
	/// 블럭이 땅/쌓여진 블럭에 닿을 때 호출
	///</summary>
	///<param name = "_pt"> 효과가 일어날 위치 </param>
	///<param name = "type"> 그 위치에 있는 블럭 종류 </param>
	void effect(POINT _pt, BLOCK_TYPE type);

	void effect_shadow();

	///<summary>
	/// 안전 변환
	///</summary>
	///<param name = "_pt"> 바꾸고 싶은 위치 </param>
	///<param name = "type"> 바꾸고 싶은 블럭 종류 </param>
	bool safeChange(POINT _pt, BLOCK_TYPE type);

	///<summary>
	/// 블럭 생성
	///</summary>
	///<param name = "_spawnXpos"> 생성되는 X위치 (Y는 0으로 고정) </param>
	///<param name = "_spawnType"> 생성하고 싶은 블럭 종류</param>
	void spawn(int _spawnXpos, BLOCK_TYPE _spawnType);
	void spawn1x1block(int);

	///<summary>
	/// 채워진 줄이 기준 줄보다 길 경우 _delete만큼 지움
	///</summary>
	///<param name = "_standard"> 기준 줄 </param>
	///<param name = "_delete"> 삭제할 줄 갯수</param>
	bool check(int _standard, int _delete);

	///<summary>
	/// 일관된 랜덤 생성
	///</summary>
	int getRandomXPos();

	/// 게임오버 체크
	void GameOverCheck(int);
};