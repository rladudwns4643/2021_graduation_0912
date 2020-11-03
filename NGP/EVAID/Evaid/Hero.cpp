#include "Hero.h"
#include "Global.h"

CHero::CHero(HERO KindOfHero, int Player) {

	HeroType = KindOfHero;
	pPosition = JPoint(HeroStartPosX, HeroStartPosY);
	SetHeroRect();
	state = STATE::NORMAL;
	NowSkillGauge = 0;
	jumpCnt = 0;
	skillCnt = 0;
	IsSkillOn = false;
	Debuff = 0;
	PlayerNum = Player;
	switch (KindOfHero)
	{
	case HERO::Hero1:
		speed = 9;
		jumpHeight = 20;		// jumpHeighy / 5 가 점프하는 칸수
		SkillCostGauge = 4;		// SkillCostGauge / 20 이 스킬 사용하는데 필요한 칸수
		break;
	case HERO::Hero2:
		speed = 9;
		jumpHeight = 15;		// 세칸 점프
		SkillCostGauge = 5;
		break;
	}
}

CHero::~CHero(){}

void CHero::SetHeroRect() {
	rect.left = pPosition.x - (HERO_SIZE / 2);
	rect.right = pPosition.x + (HERO_SIZE / 2);
	rect.top = pPosition.y - (HERO_SIZE / 2);
	rect.bottom = pPosition.y + (HERO_SIZE / 2);
}

void CHero::move(CTable Target) {
	// 점프 관련
	if (jumpCnt == 0) {
		int bottomY = GetPosInValY(rect.bottom + Gravity);
		int leftX = GetPosInValX(rect.left + 7);
		int rightX = GetPosInValX(rect.right - 7);
		if (((Target.Val[leftX][bottomY] == BLOCK_TYPE::NONE && Target.Val[rightX][bottomY] == BLOCK_TYPE::NONE)
			|| (Target.Val[leftX][bottomY] == BLOCK_TYPE::SHADOW && Target.Val[rightX][bottomY] == BLOCK_TYPE::SHADOW))
			|| (rect.bottom - PTStartY) % BLOCK_SIZE > 0) pPosition.y += Gravity;
		else Isjump = false;
	} else {
		pPosition.y -= Gravity;
		jumpCnt--;
	}


	int PosL = GetPosInValX(rect.left + 1);
	int PosR = GetPosInValX(rect.right - 1);
	int PosT = GetPosInValY(rect.top);
	int PosB = GetPosInValY(rect.bottom);
	if ((Target.Val[PosL][PosT] != BLOCK_TYPE::NONE && Target.Val[PosL][PosT] != BLOCK_TYPE::SHADOW)
		|| (Target.Val[PosR][PosT] != BLOCK_TYPE::NONE && Target.Val[PosR][PosT] != BLOCK_TYPE::SHADOW)) {
		jumpCnt = 0;
		if (!((Target.Val[PosL][PosB + 1] != BLOCK_TYPE::NONE && Target.Val[PosL][PosB + 1] != BLOCK_TYPE::SHADOW)
			|| (Target.Val[PosR][PosB + 1] != BLOCK_TYPE::NONE && Target.Val[PosR][PosB + 1] != BLOCK_TYPE::SHADOW)))
			pPosition.y = PTStartY + (PosT + 1) * BLOCK_SIZE + BLOCK_SIZE / 2;
	}

	// 이동 관련
	if (state == STATE::LEFT) {
		if (speed >= 0) {
			int PosT = GetPosInValY(rect.top);
			int PosB = GetPosInValY(rect.bottom - 1);
			int PosL = GetPosInValX(rect.left - speed);
			if (((Target.Val[PosL][PosT] == BLOCK_TYPE::NONE && Target.Val[PosL][PosB] == BLOCK_TYPE::NONE)
				|| (Target.Val[PosL][PosT] == BLOCK_TYPE::SHADOW && Target.Val[PosL][PosB] == BLOCK_TYPE::SHADOW))
				&& rect.left - speed >= PTStartX) pPosition.x -= speed;
			else pPosition.x = (rect.left / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
		} else {
			int reverseSpeed = speed * -1;
			int topY = GetPosInValY(rect.top);
			int bottomY = GetPosInValY(rect.bottom - 1);
			int rightX = GetPosInValX(rect.right + reverseSpeed);
			if (rightX == table_WIDTH)
				rightX -= 1;
			if (((Target.Val[rightX][topY] == BLOCK_TYPE::NONE && Target.Val[rightX][bottomY] == BLOCK_TYPE::NONE)
				|| (Target.Val[rightX][topY] == BLOCK_TYPE::SHADOW && Target.Val[rightX][bottomY] == BLOCK_TYPE::SHADOW))
				&& rect.right + reverseSpeed <= PTStartX + table_WIDTH * BLOCK_SIZE) pPosition.x += reverseSpeed;
			else pPosition.x = (rect.right / BLOCK_SIZE) * BLOCK_SIZE;
		}
	}
	else if (state == STATE::RIGHT) {
		if (speed >= 0) {
			int topY = GetPosInValY(rect.top);
			int bottomY = GetPosInValY(rect.bottom - 1);
			int rightX = GetPosInValX(rect.right + speed);
			if (rightX == table_WIDTH)
				rightX -= 1;
			if (((Target.Val[rightX][topY] == BLOCK_TYPE::NONE && Target.Val[rightX][bottomY] == BLOCK_TYPE::NONE)
				|| (Target.Val[rightX][topY] == BLOCK_TYPE::SHADOW && Target.Val[rightX][bottomY] == BLOCK_TYPE::SHADOW))
				&& rect.right + speed <= PTStartX + table_WIDTH * BLOCK_SIZE) pPosition.x += speed;
			else pPosition.x = (rect.right / BLOCK_SIZE) * BLOCK_SIZE;
		} else {
			int reverseSpeed = speed * -1;
			int topY = GetPosInValY(rect.top);
			int bottomY = GetPosInValY(rect.bottom - 1);
			int leftX = GetPosInValX(rect.left - reverseSpeed);
			if (((Target.Val[leftX][topY] == BLOCK_TYPE::NONE && Target.Val[leftX][bottomY] == BLOCK_TYPE::NONE)
				|| (Target.Val[leftX][topY] == BLOCK_TYPE::SHADOW && Target.Val[leftX][bottomY] == BLOCK_TYPE::SHADOW))
				&& rect.left - reverseSpeed >= PTStartX) pPosition.x -= reverseSpeed;
			else pPosition.x = (rect.left / BLOCK_SIZE) * BLOCK_SIZE + BLOCK_SIZE;
		}
	}

	if (PlayerNum == 1) Global::getInstance()->Player1Center = pPosition;
	else if (PlayerNum == 2) Global::getInstance()->Player2Center = pPosition;

	SetHeroRect();
}

void CHero::draw(HDC hDC) {
	int motionTick = (Global::getInstance()->TimerTick / 10) % 2;

	// 히어로
	if (IsSkillOn) {
		switch (HeroType) {
		case HERO::Hero1:
			if (motionTick) ResorceTable::getInstance()->img_Hero1_S1.Draw(hDC, rect);
			else ResorceTable::getInstance()->img_Hero1_S2.Draw(hDC, rect);
			break;
		case HERO::Hero2:
			if (motionTick) ResorceTable::getInstance()->img_Hero2_S1.Draw(hDC, rect);
			else ResorceTable::getInstance()->img_Hero2_S2.Draw(hDC, rect);
			break;
		}
	} else {
		switch (HeroType) {
		case HERO::Hero1:
			switch (state) {
			case STATE::NORMAL:
				ResorceTable::getInstance()->img_Hero1_L1.Draw(hDC, rect);
				break;
			case STATE::LEFT:
				if(motionTick) ResorceTable::getInstance()->img_Hero1_L1.Draw(hDC, rect);
				else ResorceTable::getInstance()->img_Hero1_L2.Draw(hDC, rect);
				break;
			case STATE::RIGHT:
				if (motionTick) ResorceTable::getInstance()->img_Hero1_R1.Draw(hDC, rect);
				else ResorceTable::getInstance()->img_Hero1_R2.Draw(hDC, rect);
				break;
			}
			break;
		case HERO::Hero2:
			switch (state) {
			case STATE::NORMAL:
				ResorceTable::getInstance()->img_Hero2_L1.Draw(hDC, rect);
				break;
			case STATE::LEFT:
				if (motionTick) ResorceTable::getInstance()->img_Hero2_L1.Draw(hDC, rect);
				else ResorceTable::getInstance()->img_Hero2_L2.Draw(hDC, rect);
				break;
			case STATE::RIGHT:
				if (motionTick) ResorceTable::getInstance()->img_Hero2_R1.Draw(hDC, rect);
				else ResorceTable::getInstance()->img_Hero2_R2.Draw(hDC, rect);
				break;
			}
			break;
		}
	}

	// 디버프 상태
	switch (Debuff) {
	case 1:
		ResorceTable::getInstance()->img_Debuff_H1.Draw(hDC, rect);
		break;
	case 2:
		ResorceTable::getInstance()->img_Debuff_H2.Draw(hDC, rect);
		break;
	}

	// 스킬 게이지
	RECT SkillBarRect{ 530,545,570,755 };
	ResorceTable::getInstance()->img_SkillBar.Draw(hDC, SkillBarRect);
	for (int i = 9; i >= 10 - NowSkillGauge; --i) {
		RECT SkillGaugeRect{ 530,550 + (i * 20), 570, 550 + ((i + 1) * 20) };
		ResorceTable::getInstance()->img_SkillGauge.Draw(hDC, SkillGaugeRect);
	}
}

void CHero::herojump() {
	if (!Isjump) {
		ResorceTable::getInstance()->m_Sound.PlayEffect(1);
		jumpCnt = jumpHeight;
		Isjump = true;
	}
}

void CHero::skillGaugeUp(CHero& enemyHero) {
	// 지속시간 스킬
	if (IsSkillOn) {
		if (skillCnt > 0) skillCnt--;
		else SkillOff(enemyHero);
	}
	// 스킬 게이지
	if (Global::getInstance()->TimerTick % 50 == 0) if (NowSkillGauge + NormalSkillGaugeIncrement < MaxSkillGauge)	NowSkillGauge += NormalSkillGaugeIncrement;
}

void CHero::SkillOn(CHero& enemyHero) {
	if (NowSkillGauge >= SkillCostGauge && !IsSkillOn) {
		ResorceTable::getInstance()->m_Sound.PlayEffect(4);
		IsSkillOn = true;
		NowSkillGauge -= SkillCostGauge;
		switch (HeroType) {
		case HERO::Hero1:
			enemyHero.Debuff = 1;
			skillCnt = 10;
			enemyHero.speed -= 4;
			break;
		case HERO::Hero2:
			enemyHero.Debuff = 2;
			skillCnt = 10;
			enemyHero.jumpHeight = 0;
			break;
		}
	}
}

void CHero::SkillOff(CHero& enemyHero)
{
	enemyHero.Debuff = 0;
	IsSkillOn = false;
	switch (HeroType) {
	case HERO::Hero1:
		enemyHero.speed += 4;
		break;
	case HERO::Hero2:
		if(HeroType==HERO::Hero1) enemyHero.jumpHeight = 20;
		else enemyHero.jumpHeight = 15;
		break;
	}
}