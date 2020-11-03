#pragma once
#include "MISC.h"

class ResorceTable {
private:
	ResorceTable();
	~ResorceTable();
	bool loadCheck();
	void loadImg();
	static ResorceTable* inst;
public:
	static ResorceTable* getInstance() {
		if (inst == nullptr) inst = new ResorceTable();
		return inst;
	}
	//add instance

	CImage		img_block_stack;
	CImage		img_block_A;
	CImage		img_block_B;
	CImage		img_block_C;
	CImage		img_block_D;
	CImage		img_block_E;

	CImage		img_Hero1_L1;
	CImage		img_Hero1_R1;
	CImage		img_Hero1_L2;
	CImage		img_Hero1_R2;
	CImage		img_Hero1_S1;
	CImage		img_Hero1_S2;
	CImage		img_Debuff_H1;

	CImage		img_Hero2_L1;
	CImage		img_Hero2_L2;
	CImage		img_Hero2_R1;
	CImage		img_Hero2_R2;
	CImage		img_Hero2_S1;
	CImage		img_Hero2_S2;
	CImage		img_Debuff_H2;

	CImage		img_shadow_block;

	CImage		img_SkillBar;
	CImage		img_SkillGauge;

	/////////////////////////////
	CImage		img_ingame_bg;
	CImage		img_ingame_border;
	CImage		img_ingame_nextblock_border;
	CImage		img_logo_bg;

	/////////////////////////////
	CImage		img_main_btn_start;
	CImage		img_main_btn_rank;
	CImage		img_main_btn_exit;
	/////////////////////////////


	SoundManager m_Sound;

};