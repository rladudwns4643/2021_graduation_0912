#pragma once
#include "MISC.h"
#include "Table.h"
#include "ResorceTable.h"
#include "Global.h"

// 히어로 떨어지는 속도
#define Gravity 8

// 스킬관련
#define MaxSkillGauge 10
#define NormalSkillGaugeIncrement 1

class CHero
{
private:
	HERO HeroType;
	JPoint pPosition;
	RECT rect;			// Rect
	int speed;			// 이동속도 능력치

	int jumpHeight;		// 점프 능력치
	int jumpCnt;		// 점프 카운트
	bool Isjump;		// 점프 체크

	int skillCnt;		// 스킬 카운트
	bool IsSkillOn;		// 스킬 체크
	int SkillCostGauge;	// 스킬을 사용하기 위한 게이지
	int NowSkillGauge;	// 현재 스킬 게이지

	int PlayerNum;		// 플레이어 구분
	int Debuff;			// 디버프상태
public:
	STATE state;		// 상태

	CHero(HERO, int);		// 인자 = 히어로 종류, 플레이어구분
	~CHero();
	void SetHeroRect();
	void move(CTable);
	void draw(HDC);
	void herojump();
	void skillGaugeUp(CHero&);
	void SkillOn(CHero&);
	void SkillOff(CHero&);
};

