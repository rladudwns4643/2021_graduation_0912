#pragma once
#include "Global.h"


//물리 일단 킾, 당장 못하겠음 -> 중간발표 이전에 해결할 수 있을까
class Object;
class Boundary;

enum eAXIS {AXIS_X, AXIS_Y, AXIS_Z, AXIS_NUM};

struct BBPoints {
	XMFLOAT3 pt[8];
};

struct BBTwoPoint {
	XMFLOAT3 pt[2];
};

struct ProcCollision {
	bool is_collision;
	XMFLOAT3 collision_normal_vector;
	XMFLOAT3 relative_velocity;
	float overlap_depth;
	bool is_sequential;
};

class Physics {
public:
	Physics() {};

public:
	ProcCollision CheckCollision(const Object* lhs, const Object* rhs) const;
	bool CheckBulletObjCollisionPoint(const XMFLOAT3 bullet_pos, const Object* obj) const;
	bool CheckBulletObjCollisionLine(const XMFLOAT3 bullet_pos_s, const XMFLOAT3 bullet_pos_e, const Object* obj) const;
	bool CheckBulletCharacterCollision(const XMFLOAT3 bullet_pos, const Object* obj) const;

	void ProcBulletImpulse(Object* rhs, XMFLOAT3 bullet_pos_s, XMFLOAT3 bullet_pos_e);
};