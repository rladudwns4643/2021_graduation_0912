#pragma once
#include "MISC.h"
#include "Table.h"
#include "ResorceTable.h"
#include "Global.h"

// ����� �������� �ӵ�
#define Gravity 8

// ��ų����
#define MaxSkillGauge 10
#define NormalSkillGaugeIncrement 1

class CHero
{
private:
	HERO HeroType;
	JPoint pPosition;
	RECT rect;			// Rect
	int speed;			// �̵��ӵ� �ɷ�ġ

	int jumpHeight;		// ���� �ɷ�ġ
	int jumpCnt;		// ���� ī��Ʈ
	bool Isjump;		// ���� üũ

	int skillCnt;		// ��ų ī��Ʈ
	bool IsSkillOn;		// ��ų üũ
	int SkillCostGauge;	// ��ų�� ����ϱ� ���� ������
	int NowSkillGauge;	// ���� ��ų ������

	int PlayerNum;		// �÷��̾� ����
	int Debuff;			// ���������
public:
	STATE state;		// ����

	CHero(HERO, int);		// ���� = ����� ����, �÷��̾��
	~CHero();
	void SetHeroRect();
	void move(CTable);
	void draw(HDC);
	void herojump();
	void skillGaugeUp(CHero&);
	void SkillOn(CHero&);
	void SkillOff(CHero&);
};

