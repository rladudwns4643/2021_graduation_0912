#pragma once
#include "Object.h"

class Character : public Object {
public: 
	Character() = default;
	Character(const int& spawn_pos_idx);
	Character(Object& cur, const Boundary* new_bb);
	~Character();

public:
	void Initialize();
};