#include "Table.h"
#include "Global.h"

CTable::CTable(int Player) {
	PlayerNum = Player;
	ShakeDegree = 1000;
	ptstart.x = PTStartX;
	ptstart.y = PTStartY;
	for (int j = 0; j < table_HEIGHT; ++j) {
		for (int i = 0; i < table_WIDTH; ++i) {
			Val[i][j] = BLOCK_TYPE::NONE;
			rcVal[i][j] = RECT{
				 (i + 0) * BLOCK_SIZE+ ptstart.x
				,(j + 0) * BLOCK_SIZE + ptstart.y
				,(i + 1) * BLOCK_SIZE + ptstart.x
				,(j + 1) * BLOCK_SIZE + ptstart.y };
		}
	}
	init();
	spawn(getRandomXPos(), static_cast<BLOCK_TYPE>(getRandom(Block_Type_Count)));
}

CTable::~CTable() {}

void CTable::drawBg(HDC hDC) {
	ResorceTable::getInstance()->img_ingame_bg.Draw(hDC, 0, 0);
}

void CTable::drawBorder(HDC hDC) {
	ResorceTable::getInstance()->img_ingame_border.Draw(hDC, ptstart.x, ptstart.y);
}

void CTable::drawBlock(HDC hDC)
{
	for (int j = 0; j < table_HEIGHT; ++j) {
		for (int i = 0; i < table_WIDTH; ++i) {
			switch (Val[i][j]) {
			case BLOCK_TYPE::A:
				ResorceTable::getInstance()->img_block_A.Draw(hDC, rcVal[i][j]);
				break;
			case BLOCK_TYPE::B:
				ResorceTable::getInstance()->img_block_B.Draw(hDC, rcVal[i][j]);
				break;
			case BLOCK_TYPE::C:
				ResorceTable::getInstance()->img_block_C.Draw(hDC, rcVal[i][j]);
				break;
			case BLOCK_TYPE::D:
				ResorceTable::getInstance()->img_block_D.Draw(hDC, rcVal[i][j]);
				break;
			case BLOCK_TYPE::E:
				ResorceTable::getInstance()->img_block_E.Draw(hDC, rcVal[i][j]);
				break;
			case BLOCK_TYPE::STACK:
				ResorceTable::getInstance()->img_block_stack.Draw(hDC, rcVal[i][j]);
				break;
			case BLOCK_TYPE::SHADOW:
				ResorceTable::getInstance()->img_shadow_block.Draw(hDC, rcVal[i][j]);
				break;
			case BLOCK_TYPE::NONE:
				FillRect(hDC, &rcVal[i][j], static_cast<HBRUSH>(GetStockObject(NULL_BRUSH)));
				break;
			}
		}
	}
}

void CTable::drawNextBlock(HDC hDC)
{
	ResorceTable::getInstance()->img_ingame_nextblock_border.Draw(hDC, ptnextBlock.x, ptnextBlock.y);

	switch (nextBlock) {
	case BLOCK_TYPE::A:
		ResorceTable::getInstance()->img_block_A.Draw(hDC, rcnextBlock);
		break;
	case BLOCK_TYPE::B:
		ResorceTable::getInstance()->img_block_B.Draw(hDC, rcnextBlock);
		break;
	case BLOCK_TYPE::C:
		ResorceTable::getInstance()->img_block_C.Draw(hDC, rcnextBlock);
		break;
	case BLOCK_TYPE::D:
		ResorceTable::getInstance()->img_block_D.Draw(hDC, rcnextBlock);
		break;
	case BLOCK_TYPE::E:
		ResorceTable::getInstance()->img_block_E.Draw(hDC, rcnextBlock);
		break;
	}
}

void CTable::draw(HDC hDC) {
	//DrawBg
	drawBg(hDC);

	//DrawBorder
	drawBorder(hDC);

	//DrawBlock
	drawBlock(hDC);

	//Draw nextBlock & Border
	drawNextBlock(hDC);
}

void CTable::init() {
	Val[0][table_HEIGHT - 1] = BLOCK_TYPE::STACK;
	Val[1][table_HEIGHT - 1] = BLOCK_TYPE::STACK;
	Val[2][table_HEIGHT - 1] = BLOCK_TYPE::STACK;
	Val[3][table_HEIGHT - 1] = BLOCK_TYPE::STACK;
	Val[4][table_HEIGHT - 1] = BLOCK_TYPE::STACK;
	Val[5][table_HEIGHT - 1] = BLOCK_TYPE::STACK;
	Val[6][table_HEIGHT - 1] = BLOCK_TYPE::STACK;
	Val[7][table_HEIGHT - 1] = BLOCK_TYPE::STACK;
	Val[8][table_HEIGHT - 1] = BLOCK_TYPE::STACK;
	Val[9][table_HEIGHT - 1] = BLOCK_TYPE::STACK;

	Val[0][table_HEIGHT - 2] = BLOCK_TYPE::STACK;
	Val[1][table_HEIGHT - 2] = BLOCK_TYPE::STACK;
	Val[2][table_HEIGHT - 2] = BLOCK_TYPE::STACK;
	Val[3][table_HEIGHT - 2] = BLOCK_TYPE::STACK;
	Val[4][table_HEIGHT - 2] = BLOCK_TYPE::STACK;
	Val[5][table_HEIGHT - 2] = BLOCK_TYPE::STACK;
	Val[6][table_HEIGHT - 2] = BLOCK_TYPE::STACK;
	Val[7][table_HEIGHT - 2] = BLOCK_TYPE::STACK;
	Val[8][table_HEIGHT - 2] = BLOCK_TYPE::STACK;
	Val[9][table_HEIGHT - 2] = BLOCK_TYPE::STACK;

	Val[0][table_HEIGHT - 3] = BLOCK_TYPE::STACK;
	Val[1][table_HEIGHT - 3] = BLOCK_TYPE::STACK;
	Val[2][table_HEIGHT - 3] = BLOCK_TYPE::STACK;
	Val[3][table_HEIGHT - 3] = BLOCK_TYPE::STACK;
	Val[4][table_HEIGHT - 3] = BLOCK_TYPE::STACK;
	Val[5][table_HEIGHT - 3] = BLOCK_TYPE::STACK;
	Val[6][table_HEIGHT - 3] = BLOCK_TYPE::STACK;
	Val[7][table_HEIGHT - 3] = BLOCK_TYPE::STACK;
	Val[8][table_HEIGHT - 3] = BLOCK_TYPE::STACK;
	Val[9][table_HEIGHT - 3] = BLOCK_TYPE::STACK;
}

void CTable::drop() {
	for (int j = table_HEIGHT - 1; j >= 0; --j) {
		for (int i = 0; i < table_WIDTH; ++i) {
			//A~E블럭과 STACK만 떨어짐
			if (Val[i][j] != BLOCK_TYPE::NONE
				&& Val[i][j] != BLOCK_TYPE::SHADOW)
			{
				//쌓인것 위 이거나 바닥일때
				if (Val[i][j + 1] == BLOCK_TYPE::STACK || j == table_HEIGHT - 1)
				{
					effect(POINT{ i,j }, Val[i][j]);
				}
				//비어있는곳 위, 그림자 위일때
				if (Val[i][j + 1] == BLOCK_TYPE::NONE
					|| Val[i][j + 1] == BLOCK_TYPE::SHADOW)
				{
					// 바닥위가 아니라면
					if (j != table_HEIGHT - 1)
					{
						nowDrop = TRUE;
						Val[i][j + 1] = Val[i][j];
						Val[i][j] = BLOCK_TYPE::NONE;
					}
				}
			}
		}
	}
//	if (!nowDrop) {
		if (!check(standard_line, delete_line)) {
			//spawn(getRandomXPos(), static_cast<BLOCK_TYPE>(getRandom(Block_Type_Count)));
		}
//	}
	nowDrop = FALSE;
}

void CTable::effect(POINT _pt, BLOCK_TYPE type)
{
	switch (type) {
	case BLOCK_TYPE::A:
		ShakeDegree = 0;
		if (Global::getInstance()->Gameover_1 == true && PlayerNum == 1)
			ResorceTable::getInstance()->m_Sound.PlayEffect(5);
		else if (Global::getInstance()->Gameover_2 == true && PlayerNum == 2)
			ResorceTable::getInstance()->m_Sound.PlayEffect(5);
		else
			ResorceTable::getInstance()->m_Sound.PlayEffect(3);
		safeChange(_pt, BLOCK_TYPE::STACK);
		break;
	case BLOCK_TYPE::B:
		ShakeDegree = 0;
		if (Global::getInstance()->Gameover_1 == true && PlayerNum == 1)
			ResorceTable::getInstance()->m_Sound.PlayEffect(5);
		else if (Global::getInstance()->Gameover_2 == true && PlayerNum == 2)
			ResorceTable::getInstance()->m_Sound.PlayEffect(5);
		else
			ResorceTable::getInstance()->m_Sound.PlayEffect(3);
		safeChange(POINT{ _pt.x - 3,_pt.y }, BLOCK_TYPE::SHADOW);
		safeChange(POINT{ _pt.x - 2,_pt.y }, BLOCK_TYPE::SHADOW);
		safeChange(POINT{ _pt.x - 1,_pt.y }, BLOCK_TYPE::SHADOW);
		safeChange(POINT{ _pt.x + 1,_pt.y }, BLOCK_TYPE::SHADOW);
		safeChange(POINT{ _pt.x + 2,_pt.y }, BLOCK_TYPE::SHADOW);
		safeChange(POINT{ _pt.x + 3,_pt.y }, BLOCK_TYPE::SHADOW);

		safeChange(POINT{ _pt.x + 0,_pt.y }, BLOCK_TYPE::STACK);
		break;
	case BLOCK_TYPE::C:
		ShakeDegree = 0;
		if (Global::getInstance()->Gameover_1 == true && PlayerNum == 1)
			ResorceTable::getInstance()->m_Sound.PlayEffect(5);
		else if (Global::getInstance()->Gameover_2 == true && PlayerNum == 2)
			ResorceTable::getInstance()->m_Sound.PlayEffect(5);
		else
			ResorceTable::getInstance()->m_Sound.PlayEffect(3);
		safeChange(POINT{ _pt.x , _pt.y - 1 }, BLOCK_TYPE::SHADOW);

		safeChange(POINT{ _pt.x , _pt.y - 0 }, BLOCK_TYPE::STACK);
		break;
	case BLOCK_TYPE::D:
		ShakeDegree = 0;
		if (Global::getInstance()->Gameover_1 == true && PlayerNum == 1)
			ResorceTable::getInstance()->m_Sound.PlayEffect(5);
		else if (Global::getInstance()->Gameover_2 == true && PlayerNum == 2)
			ResorceTable::getInstance()->m_Sound.PlayEffect(5);
		else
			ResorceTable::getInstance()->m_Sound.PlayEffect(3);
		safeChange(POINT{ _pt.x - 1 , _pt.y - 1 }, BLOCK_TYPE::SHADOW);
		safeChange(POINT{ _pt.x + 0 , _pt.y - 1 }, BLOCK_TYPE::SHADOW);
		safeChange(POINT{ _pt.x + 1 , _pt.y - 1 }, BLOCK_TYPE::SHADOW);
		safeChange(POINT{ _pt.x + 1 , _pt.y + 0 }, BLOCK_TYPE::SHADOW);
		safeChange(POINT{ _pt.x - 1 , _pt.y + 0 }, BLOCK_TYPE::SHADOW);

		safeChange(POINT{ _pt.x + 0 , _pt.y + 0 }, BLOCK_TYPE::STACK);
		break;
	case BLOCK_TYPE::E:
		ShakeDegree = 0;
		if (Global::getInstance()->Gameover_1 == true && PlayerNum == 1)
			ResorceTable::getInstance()->m_Sound.PlayEffect(5);
		else if (Global::getInstance()->Gameover_2 == true && PlayerNum == 2)
			ResorceTable::getInstance()->m_Sound.PlayEffect(5);
		else
			ResorceTable::getInstance()->m_Sound.PlayEffect(3);
		safeChange(POINT{ _pt.x - 1, _pt.y - 3 }, BLOCK_TYPE::SHADOW);
		safeChange(POINT{ _pt.x + 0, _pt.y - 3 }, BLOCK_TYPE::SHADOW);
		safeChange(POINT{ _pt.x + 1, _pt.y - 3 }, BLOCK_TYPE::SHADOW);

		safeChange(POINT{ _pt.x + 0, _pt.y + 0 }, BLOCK_TYPE::STACK);
		break;
	}
	check(standard_line, delete_line);
	isShadowOn = true;
	GameOverCheck(PlayerNum);
}

void CTable::effect_shadow() {
	for (int j = 0; j < table_HEIGHT; ++j) {
		for (int i = 0; i < table_WIDTH; ++i) {
			if (Val[i][j] == BLOCK_TYPE::SHADOW) {
				Val[i][j] = BLOCK_TYPE::STACK;
			}
		}
	}
	GameOverCheck(PlayerNum);
}

bool CTable::safeChange(POINT _pt, BLOCK_TYPE type)
{
	if (_pt.x < 0 || _pt.x >= table_WIDTH) return false;
	if (_pt.y < 0 || _pt.y >= table_HEIGHT) return false;
	if (Val[_pt.x][_pt.y] == BLOCK_TYPE::STACK && type == BLOCK_TYPE::SHADOW) return false;
	Val[_pt.x][_pt.y] = type;
	return true;
	
}

void CTable::spawn(int _spawnXpos, BLOCK_TYPE _spawnType)
{
	safeChange(POINT{ _spawnXpos, 0 }, nextBlock);

	nextBlock = _spawnType;
}

void CTable::spawn1x1block(int TargetPos)
{
	if (TargetPos != -1)
		Val[TargetPos][0] = BLOCK_TYPE::A;
}

bool CTable::check(int _standard, int _delete)
{
	for (int i = 0; i < table_WIDTH; ++i) {
		for (int j = 0; j < standard_line; ++j) {
			if (Val[i][(table_HEIGHT - 1) - j] != BLOCK_TYPE::STACK) return FALSE;
		}
	}
	if ((Global::getInstance()->Gameover_1 == false && PlayerNum==1) ||
		 (Global::getInstance()->Gameover_2 == false && PlayerNum==2))
	{
		for (int i = 0; i < table_WIDTH; ++i) {
			for (int j = 0; j < delete_line; ++j) {
				Val[i][(table_HEIGHT - 1) - j] = BLOCK_TYPE::NONE;
			}
		}
	}
	return TRUE;
}

int CTable::getRandomXPos()
{
	bool realRandom = true;
	for (int i = 0; i < table_WIDTH; ++i)
	{
		if (Val[i][table_HEIGHT - standard_line - 2] == BLOCK_TYPE::STACK)
		{
			realRandom = false;
			break;
		}
	}
	if (realRandom)
		return rand() % table_WIDTH;

	int targetPos[table_WIDTH];
	int targetPosCnt = 0;
	for (int i = 0; i < table_WIDTH; ++i)
	{
		if (Val[i][table_HEIGHT - standard_line] == BLOCK_TYPE::NONE || Val[i][table_HEIGHT - standard_line] == BLOCK_TYPE::SHADOW)
			targetPos[targetPosCnt++] = i;
	}
	if (targetPosCnt > 0)
		return targetPos[rand() % targetPosCnt];
	return rand() % table_WIDTH;
}

void CTable::GameOverCheck(int Player)
{
	if (Player == 1)
	{
		if ((Val[GetPosInValX(Global::getInstance()->Player1Center.x - 17)][GetPosInValY(Global::getInstance()->Player1Center.y)] != BLOCK_TYPE::NONE &&
			Val[GetPosInValX(Global::getInstance()->Player1Center.x - 17)][GetPosInValY(Global::getInstance()->Player1Center.y)] != BLOCK_TYPE::SHADOW)
			|| (Val[GetPosInValX(Global::getInstance()->Player1Center.x + 17)][GetPosInValY(Global::getInstance()->Player1Center.y)] != BLOCK_TYPE::NONE &&
				Val[GetPosInValX(Global::getInstance()->Player1Center.x + 17)][GetPosInValY(Global::getInstance()->Player1Center.y)] != BLOCK_TYPE::SHADOW))
		{
			Global::getInstance()->Gameover_1 = true;
		}
	}
	else if (Player == 2)
	{
		if ((Val[GetPosInValX(Global::getInstance()->Player2Center.x - 17)][GetPosInValY(Global::getInstance()->Player2Center.y)] != BLOCK_TYPE::NONE &&
			Val[GetPosInValX(Global::getInstance()->Player2Center.x - 17)][GetPosInValY(Global::getInstance()->Player2Center.y)] != BLOCK_TYPE::SHADOW)
			|| (Val[GetPosInValX(Global::getInstance()->Player2Center.x + 17)][GetPosInValY(Global::getInstance()->Player2Center.y)] != BLOCK_TYPE::NONE &&
				Val[GetPosInValX(Global::getInstance()->Player2Center.x + 17)][GetPosInValY(Global::getInstance()->Player2Center.y)] != BLOCK_TYPE::SHADOW))
		{
			Global::getInstance()->Gameover_2 = true;
		}
	}
}
