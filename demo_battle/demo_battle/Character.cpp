#include "pch.h"
#include "extern.h"
#include "Character.h"

Character::Character(const int& spawn_pos_idx) {
	Initialize();

	XMFLOAT4 spawn_pos = SR::g_spawn_pos.m_spawn_pos[spawn_pos_idx];
	SetPosition(spawn_pos.x, spawn_pos.y, spawn_pos.z);
	Rotate(0.f, spawn_pos.w, 0.f);

	m_boundaries = new Boundary(*SR::g_boundaries[OBJECT_TYPE_TOY]);
	m_boundaries->BoundaryMove(spawn_pos.x, spawn_pos.y, spawn_pos.z);
}

Character::Character(Object& cur, const Boundary* new_bb) {
	Initialize();

	m_xmf4x4World = MathHelper::Identity4x4();

	m_xmfAcc = cur.GetAccerleration();
	m_xmfVel = cur.GetVelocity();

	int numofBB = new_bb->GetNumOfBB();

	delete m_boundaries;
	m_boundaries = new Boundary(numofBB);
	m_boundaries->SetObjType(-1);

	//obj의 rot_quaternion
	FXMVECTOR obj_rot_quat{ MathHelper::QuatFromMatrix(GetMatrix()) };
	XMVECTOR result_quat;
	for (int i = 0; i < numofBB; ++i) {
		m_boundaries->SetBBPos(new_bb->GetBBPos(i), i); //setPos bb
		m_boundaries->SetBBRot(new_bb->GetBBRot(i), i); //setRot bb

		XMFLOAT3 bb_rot{ m_boundaries->GetBBRot(i) };
		FXMVECTOR bb_rot_quat{ //bb rot_quaternion
			XMQuaternionRotationRollPitchYaw(
				XMConvertToRadians(bb_rot.x),
				XMConvertToRadians(bb_rot.y),
				XMConvertToRadians(bb_rot.z))
		};

		//make quaternion mat
		//make 
		XMFLOAT4X4 result_quat_to_mat;
		result_quat = XMQuaternionMultiply(bb_rot_quat, obj_rot_quat);
		XMFLOAT4 tmp = MathHelper::XMVector4ToFloat4(result_quat);
		MathHelper::QuatToMatrix(&tmp, &result_quat_to_mat);

		XMFLOAT3 bb_pos{ m_boundaries->GetBBPos(i) };
		result_quat_to_mat._41 += bb_pos.x;
		result_quat_to_mat._42 += bb_pos.y;
		result_quat_to_mat._43 += bb_pos.z;

		m_boundaries->SetBBSize(new_bb->GetBBSize(i), i);
		m_boundaries->SetWorldMatrix(result_quat_to_mat, i);
		m_boundaries->SetBBPos(MathHelper::Add(m_boundaries->GetBBPos(i), GetPosition()), i);
	}
}

Character::~Character() {
	delete m_boundaries;
	m_boundaries = nullptr;
}

void Character::Initialize() {
	m_xmf4x4World = MathHelper::Identity4x4();
	m_xmfAcc = {};
	m_xmfVel = {};
	m_boundaries->SetObjType(OBJECT_TYPE_TOY);

	m_forceAmountXZ = 10.f;
	m_forceAmountY = 5.f;
	m_fricCoef = 0.3f;
	m_maxVel = 0.612f;		//이동속도
	m_mass = 50.f;
}
