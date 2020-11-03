#pragma once
#include "MISC.h"
#include "Global.h"
#include "ResorceTable.h"
#include "SoundManager.h"

//check �Լ� ��� ����.
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

	//boder�� �׷����� �����ϴ� pt
	POINT	ptstart;

	// ��鸮�� ����
	int ShakeDegree;

	CTable(int);
	~CTable();

	void log();

	///<summary>
	/// �׸��� �Լ� (OnDraw���� ȣ��)
	/// ���ȭ��/ ����/ ��/ ���� �� �� ���� �� ����
	///</summary>
	void draw(HDC hDC);

	void drawBg(HDC hDC);

	void drawBorder(HDC hDC);

	void drawBlock(HDC hDC);

	void drawNextBlock(HDC hDC);

	void init();

	///<summary>
	/// ���� ��ĭ�� ����(Timer���� ȣ��)
	///</summary>
	void drop();

	///<summary>
	/// ���� ��/�׿��� ���� ���� �� ȣ��
	///</summary>
	///<param name = "_pt"> ȿ���� �Ͼ ��ġ </param>
	///<param name = "type"> �� ��ġ�� �ִ� �� ���� </param>
	void effect(POINT _pt, BLOCK_TYPE type);

	void effect_shadow();

	///<summary>
	/// ���� ��ȯ
	///</summary>
	///<param name = "_pt"> �ٲٰ� ���� ��ġ </param>
	///<param name = "type"> �ٲٰ� ���� �� ���� </param>
	bool safeChange(POINT _pt, BLOCK_TYPE type);

	///<summary>
	/// �� ����
	///</summary>
	///<param name = "_spawnXpos"> �����Ǵ� X��ġ (Y�� 0���� ����) </param>
	///<param name = "_spawnType"> �����ϰ� ���� �� ����</param>
	void spawn(int _spawnXpos, BLOCK_TYPE _spawnType);
	void spawn1x1block(int);

	///<summary>
	/// ä���� ���� ���� �ٺ��� �� ��� _delete��ŭ ����
	///</summary>
	///<param name = "_standard"> ���� �� </param>
	///<param name = "_delete"> ������ �� ����</param>
	bool check(int _standard, int _delete);

	///<summary>
	/// �ϰ��� ���� ����
	///</summary>
	int getRandomXPos();

	/// ���ӿ��� üũ
	void GameOverCheck(int);
};