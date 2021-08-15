#include "pch.h"
#include "AnimationController.h"
#include "SkinnedModelInstance.h"
#include "Character.h"

AnimationController::AnimationController(Character* owner, SkinnedModelInstance* skinnedModelInst) :
	m_PlayerState(PlayerState::STATE_IDLE),
	m_KeyState(PlayerState::STATE_IDLE)
{
	m_Owner = owner;
	m_CopySkinnedModelInst = skinnedModelInst;

	// Animation Data
	m_MapAnimData[KEY_IDLE] = std::make_unique<AnimData>("Idle", 0.f);
	m_MapAnimData[KEY_FORWARD] = std::make_unique<AnimData>("Running", 0.f);
	m_MapAnimData[KEY_BACKWARD] = std::make_unique<AnimData>("Backward", 0.f);
	m_MapAnimData[KEY_LEFT_STRAFE] = std::make_unique<AnimData>("Left_Strafe", 0.f);
	m_MapAnimData[KEY_RIGHT_STRAFE] = std::make_unique<AnimData>("Right_Strafe", 0.f);
	m_MapAnimData[KEY_ATTACK] = std::make_unique<AnimData>("Attack", 0.f);
	m_MapAnimData[KEY_JUMP] = std::make_unique<AnimData>("Jump", 0.f);
	m_MapAnimData[KEY_T_POSE] = std::make_unique<AnimData>("T_Pose", 0.f);
	m_MapAnimData[KEY_DEATH] = std::make_unique<AnimData>("Death", 0.f);

	m_MaxBlendFrames = 10.f;
}

AnimationController::~AnimationController()
{
	m_CopySkinnedModelInst = nullptr;
	m_MapAnimData.clear();
}

void AnimationController::Update(const float deltaT)
{
	SetState(deltaT);
	float speed = deltaT;
	if (m_PlayerState == PlayerState::STATE_ATTACK || m_PlayerState == PlayerState::STATE_JUMP)
		speed *= 2;

	for (auto& p : m_MapAnimData)
	{
		p.second->m_Time = p.second->m_Time + speed;
	}
	UpdateBoneTransforms();

	// 캐릭터 움직임 상태 세팅
	if (m_PlayerState == PlayerState::STATE_IDLE)
		m_Owner->m_IsMoving = false;
	else
		m_Owner->m_IsMoving = true;
}

void AnimationController::SetState(const float deltaT)
{
	switch (m_PlayerState)
	{
	case STATE_IDLE:
		switch (m_KeyState)
		{
		case STATE_FORWARD:
			m_PlayerState = STATE_IDLE_TO_FORWARD;
			m_MapAnimData[KEY_FORWARD]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_BACKWARD:
			m_PlayerState = STATE_IDLE_TO_BACKWARD;
			m_MapAnimData[KEY_BACKWARD]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_LEFT_STRAFE:
			m_PlayerState = STATE_IDLE_TO_LEFT_STRAFE;
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_RIGHT_STRAFE:
			m_PlayerState = STATE_IDLE_TO_RIGHT_STRAFE;
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_ATTACK:
			m_PlayerState = STATE_IDLE_TO_ATTACK;
			m_MapAnimData[KEY_ATTACK]->m_Time = 0.0f;
			m_BlendFrame = 1.f;
			break;
		case STATE_JUMP:
			m_PlayerState = STATE_IDLE_TO_JUMP;
			m_MapAnimData[KEY_JUMP]->m_Time = 0.0f;
			m_BlendFrame = 1.f;
			break;
		case STATE_T_POSE:
			m_PlayerState = STATE_IDLE_TO_T_POSE;
			m_MapAnimData[KEY_T_POSE]->m_Time = 0.0f;
			m_BlendFrame = 1.f;
			break;
		case STATE_DEATH:
			m_PlayerState = STATE_IDLE_TO_DEATH;
			m_MapAnimData[KEY_DEATH]->m_Time = 0.0f;
			m_BlendFrame = 1.f;
			break;
		}
		break; 
	case STATE_FORWARD:
		switch (m_KeyState)
		{
		case STATE_BACKWARD:
			m_PlayerState = STATE_FORWARD_TO_BACKWARD;
			m_MapAnimData[KEY_BACKWARD]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_IDLE:
			m_PlayerState = STATE_FORWARD_TO_IDLE;
			m_MapAnimData[KEY_IDLE]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_LEFT_STRAFE:
			m_PlayerState = STATE_FORWARD_TO_LEFT_STRAFE;
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_RIGHT_STRAFE:
			m_PlayerState = STATE_FORWARD_TO_RIGHT_STRAFE;
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_ATTACK:
			m_PlayerState = STATE_FORWARD_TO_ATTACK;
			m_MapAnimData[KEY_ATTACK]->m_Time = 0.f;
			m_BlendFrame = 0.f;
			break;
		case STATE_JUMP:
			m_PlayerState = STATE_FORWARD_TO_JUMP;
			m_MapAnimData[KEY_JUMP]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_DEATH:
			m_PlayerState = STATE_FORWARD_TO_DEATH;
			m_MapAnimData[KEY_DEATH]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		}
		break;
	case STATE_BACKWARD:
		switch (m_KeyState)
		{
		case STATE_FORWARD:
			m_PlayerState = STATE_BACKWARD_TO_FORWARD;
			m_MapAnimData[KEY_FORWARD]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_IDLE:
			m_PlayerState = STATE_BACKWARD_TO_IDLE;
			m_MapAnimData[KEY_IDLE]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_LEFT_STRAFE:
			m_PlayerState = STATE_BACKWARD_TO_LEFT_STRAFE;
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_RIGHT_STRAFE:
			m_PlayerState = STATE_BACKWARD_TO_RIGHT_STRAFE;
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_ATTACK:
			m_PlayerState = STATE_BACKWARD_TO_ATTACK;
			m_MapAnimData[KEY_ATTACK]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_JUMP:
			m_PlayerState = STATE_BACKWARD_TO_JUMP;
			m_MapAnimData[KEY_JUMP]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_DEATH:
			m_PlayerState = STATE_BACKWARD_TO_DEATH;
			m_MapAnimData[KEY_DEATH]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		}
		break;
	case STATE_LEFT_STRAFE:
		switch (m_KeyState)
		{
		case STATE_RIGHT_STRAFE:
			m_PlayerState = STATE_LEFT_STRAFE_TO_RIGHT_STRAFE;
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_IDLE:
			m_PlayerState = STATE_LEFT_STRAFE_TO_IDLE;
			m_MapAnimData[KEY_IDLE]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_FORWARD:
			m_PlayerState = STATE_LEFT_STRAFE_TO_FORWARD;
			m_MapAnimData[KEY_FORWARD]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_BACKWARD:
			m_PlayerState = STATE_LEFT_STRAFE_TO_BACKWARD;
			m_MapAnimData[KEY_BACKWARD]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_ATTACK:
			m_PlayerState = STATE_LEFT_STRAFE_TO_ATTACK;
			m_MapAnimData[KEY_ATTACK]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_JUMP:
			m_PlayerState = STATE_LEFT_STRAFE_TO_JUMP;
			m_MapAnimData[KEY_JUMP]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_DEATH:
			m_PlayerState = STATE_LEFT_STRAFE_TO_DEATH;
			m_MapAnimData[KEY_DEATH]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		}
		break;
	case STATE_RIGHT_STRAFE:
		switch (m_KeyState)
		{
		case STATE_LEFT_STRAFE:
			m_PlayerState = STATE_RIGHT_STRAFE_TO_LEFT_STRAFE;
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_IDLE:
			m_PlayerState = STATE_RIGHT_STRAFE_TO_IDLE;
			m_MapAnimData[KEY_IDLE]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_FORWARD:
			m_PlayerState = STATE_RIGHT_STRAFE_TO_FORWARD;
			m_MapAnimData[KEY_FORWARD]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_BACKWARD:
			m_PlayerState = STATE_RIGHT_STRAFE_TO_BACKWARD;
			m_MapAnimData[KEY_BACKWARD]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_ATTACK:
			m_PlayerState = STATE_RIGHT_STRAFE_TO_ATTACK;
			m_MapAnimData[KEY_ATTACK]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_JUMP:
			m_PlayerState = STATE_RIGHT_STRAFE_TO_JUMP;
			m_MapAnimData[KEY_JUMP]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_DEATH:
			m_PlayerState = STATE_RIGHT_STRAFE_TO_DEATH;
			m_MapAnimData[KEY_DEATH]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		}
		break;

	case STATE_ATTACK:
		switch (m_KeyState)
		{
		case STATE_IDLE:
			m_PlayerState = STATE_ATTACK_TO_IDLE;
			m_MapAnimData[KEY_IDLE]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_FORWARD:
			m_PlayerState = STATE_ATTACK_TO_FORWARD;
			m_MapAnimData[KEY_FORWARD]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_BACKWARD:
			m_PlayerState = STATE_ATTACK_TO_BACKWARD;
			m_MapAnimData[KEY_BACKWARD]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_LEFT_STRAFE:
			m_PlayerState = STATE_ATTACK_TO_LEFT_STRAFE;
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_RIGHT_STRAFE:
			m_PlayerState = STATE_ATTACK_TO_RIGHT_STRAFE;
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_JUMP:
			m_PlayerState = STATE_ATTACK_TO_JUMP;
			m_MapAnimData[KEY_JUMP]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_DEATH:
			m_PlayerState = STATE_ATTACK_TO_DEATH;
			m_MapAnimData[KEY_DEATH]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		}
		break;
	case STATE_JUMP:
		switch (m_KeyState)
		{
		case STATE_IDLE:
			m_PlayerState = STATE_JUMP_TO_IDLE;
			m_MapAnimData[KEY_IDLE]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_FORWARD:
			m_PlayerState = STATE_JUMP_TO_FORWARD;
			m_MapAnimData[KEY_FORWARD]->m_Time = 0.0f;
			m_BlendFrame = 1.f;
			break;
		case STATE_BACKWARD:
			m_PlayerState = STATE_JUMP_TO_BACKWARD;
			m_MapAnimData[KEY_BACKWARD]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_LEFT_STRAFE:
			m_PlayerState = STATE_JUMP_TO_LEFT_STRAFE;
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_RIGHT_STRAFE:
			m_PlayerState = STATE_JUMP_TO_RIGHT_STRAFE;
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_ATTACK:
			m_PlayerState = STATE_JUMP_TO_ATTACK;
			m_MapAnimData[KEY_ATTACK]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		case STATE_DEATH:
			m_PlayerState = STATE_JUMP_TO_DEATH;
			m_MapAnimData[KEY_DEATH]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		}
		break;
	case STATE_T_POSE:
		switch (m_KeyState)
		{
		case STATE_IDLE:
			m_PlayerState = STATE_T_POSE_TO_IDLE;
			m_MapAnimData[KEY_IDLE]->m_Time = 0.0f;
			m_BlendFrame = 1.0f;
			break;
		}
		break;
	case STATE_DEATH:
		switch (m_KeyState)
		{
		case STATE_IDLE:
			m_PlayerState = STATE_DEATH_TO_IDLE;
			m_MapAnimData[KEY_IDLE]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_FORWARD:
			m_PlayerState = STATE_DEATH_TO_FORWARD;
			m_MapAnimData[KEY_FORWARD]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_BACKWARD:
			m_PlayerState = STATE_DEATH_TO_BACKWARD;
			m_MapAnimData[KEY_BACKWARD]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_LEFT_STRAFE:
			m_PlayerState = STATE_DEATH_TO_LEFT_STRAFE;
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;
		case STATE_RIGHT_STRAFE:
			m_PlayerState = STATE_DEATH_TO_RIGHT_STRAFE;
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time = 0.f;
			m_BlendFrame = 1.f;
			break;

		case STATE_ATTACK:
			m_PlayerState = STATE_DEATH_TO_ATTACK;
			m_MapAnimData[KEY_ATTACK]->m_Time = 0.0f;
			m_BlendFrame = 1.f;
			break;
		case STATE_JUMP:
			m_PlayerState = STATE_DEATH_TO_JUMP;
			m_MapAnimData[KEY_JUMP]->m_Time = 0.0f;
			m_BlendFrame = 1.f;
			break;
		}
		break;


		/************************ Blending ****************************/
			// IDLE
	case STATE_FORWARD_TO_IDLE:
	case STATE_BACKWARD_TO_IDLE:
	case STATE_LEFT_STRAFE_TO_IDLE:
	case STATE_RIGHT_STRAFE_TO_IDLE:
	case STATE_ATTACK_TO_IDLE:
	case STATE_JUMP_TO_IDLE:
	case STATE_T_POSE_TO_IDLE:
	case STATE_DEATH_TO_IDLE:
		if (m_BlendFrame > m_MaxBlendFrames)
			m_PlayerState = STATE_IDLE;
		m_BlendFrame += 60.f / (1.f / deltaT);
		break;

		// FORWARD
	case STATE_IDLE_TO_FORWARD:
	case STATE_BACKWARD_TO_FORWARD:
	case STATE_LEFT_STRAFE_TO_FORWARD:
	case STATE_RIGHT_STRAFE_TO_FORWARD:
	case STATE_ATTACK_TO_FORWARD:
	case STATE_JUMP_TO_FORWARD:
	case STATE_DEATH_TO_FORWARD:
		if (m_BlendFrame > m_MaxBlendFrames)
			m_PlayerState = STATE_FORWARD;
		m_BlendFrame += 60.f / (1.f / deltaT);
		break;

		// BACKWARD
	case STATE_IDLE_TO_BACKWARD:
	case STATE_FORWARD_TO_BACKWARD:
	case STATE_LEFT_STRAFE_TO_BACKWARD:
	case STATE_RIGHT_STRAFE_TO_BACKWARD:
	case STATE_ATTACK_TO_BACKWARD:
	case STATE_JUMP_TO_BACKWARD:
	case STATE_DEATH_TO_BACKWARD:
		if (m_BlendFrame > m_MaxBlendFrames)
			m_PlayerState = STATE_BACKWARD;
		m_BlendFrame += 60.f / (1.f / deltaT);
		break;

		// LEFT_STARFE
	case STATE_IDLE_TO_LEFT_STRAFE:
	case STATE_RIGHT_STRAFE_TO_LEFT_STRAFE:
	case STATE_FORWARD_TO_LEFT_STRAFE:
	case STATE_BACKWARD_TO_LEFT_STRAFE:
	case STATE_ATTACK_TO_LEFT_STRAFE:
	case STATE_JUMP_TO_LEFT_STRAFE:
	case STATE_DEATH_TO_LEFT_STRAFE:
		if (m_BlendFrame > m_MaxBlendFrames)
			m_PlayerState = STATE_LEFT_STRAFE;
		m_BlendFrame += 60.f / (1.f / deltaT);
		break;

		// RIGHT_STARFE
	case STATE_IDLE_TO_RIGHT_STRAFE:
	case STATE_LEFT_STRAFE_TO_RIGHT_STRAFE:
	case STATE_FORWARD_TO_RIGHT_STRAFE:
	case STATE_BACKWARD_TO_RIGHT_STRAFE:
	case STATE_ATTACK_TO_RIGHT_STRAFE:
	case STATE_JUMP_TO_RIGHT_STRAFE:
	case STATE_DEATH_TO_RIGHT_STRAFE:
		if (m_BlendFrame > m_MaxBlendFrames)
			m_PlayerState = STATE_RIGHT_STRAFE;
		m_BlendFrame += 60.f / (1.f / deltaT);
		break;

		// ATTACK
	case STATE_FORWARD_TO_ATTACK:
	case STATE_BACKWARD_TO_ATTACK:
	case STATE_LEFT_STRAFE_TO_ATTACK:
	case STATE_RIGHT_STRAFE_TO_ATTACK:
	case STATE_JUMP_TO_ATTACK:
	case STATE_IDLE_TO_ATTACK:
	case STATE_DEATH_TO_ATTACK:
		if (m_BlendFrame > m_MaxBlendFrames)
			m_PlayerState = STATE_ATTACK;
		m_BlendFrame += 60.f / (1.f / deltaT);
		break;

		// JUMP
	case STATE_FORWARD_TO_JUMP:
	case STATE_BACKWARD_TO_JUMP:
	case STATE_LEFT_STRAFE_TO_JUMP:
	case STATE_RIGHT_STRAFE_TO_JUMP:
	case STATE_ATTACK_TO_JUMP:
	case STATE_IDLE_TO_JUMP:
	case STATE_DEATH_TO_JUMP:
		if (m_BlendFrame > m_MaxBlendFrames)
			m_PlayerState = STATE_JUMP;
		m_BlendFrame += 60.f / (1.f / deltaT);
		break;

		// T_POSE
	case STATE_IDLE_TO_T_POSE:
		if (m_BlendFrame > m_MaxBlendFrames)
			m_PlayerState = STATE_T_POSE;
		m_BlendFrame += 60.f / (1.f / deltaT);
		break;

		// DEATH
	case STATE_IDLE_TO_DEATH:
	case STATE_FORWARD_TO_DEATH:
	case STATE_BACKWARD_TO_DEATH:
	case STATE_LEFT_STRAFE_TO_DEATH:
	case STATE_RIGHT_STRAFE_TO_DEATH:
	case STATE_ATTACK_TO_DEATH:
	case STATE_JUMP_TO_DEATH:
		if (m_BlendFrame > m_MaxBlendFrames)
			m_PlayerState = STATE_DEATH;
		m_BlendFrame += 60.f / (1.f / deltaT);
		break;
	}
}

void AnimationController::UpdateBoneTransforms()
{
	std::string strState = "";

	switch (m_PlayerState)
	{
	case STATE_IDLE:
		strState = KEY_IDLE;
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time);
		break;
	case STATE_FORWARD:
		strState = KEY_FORWARD;
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time);
		break;
	case STATE_BACKWARD:
		strState = KEY_BACKWARD;
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time);
		break;
	case STATE_LEFT_STRAFE:
		strState = KEY_LEFT_STRAFE;
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time);
		break;
	case STATE_RIGHT_STRAFE:
		strState = KEY_RIGHT_STRAFE;
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time);
		break;
	case STATE_ATTACK:
		strState = KEY_ATTACK;
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time);
		break;
	case STATE_JUMP:
		strState = KEY_JUMP;
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time);
		break;
	case STATE_T_POSE:
		strState = KEY_T_POSE;
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(m_MapAnimData[KEY_T_POSE]->m_Name, m_MapAnimData[KEY_T_POSE]->m_Time);
		break;
	case STATE_DEATH:
		strState = KEY_DEATH;
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(m_MapAnimData[KEY_DEATH]->m_Name, m_MapAnimData[KEY_DEATH]->m_Time);
		break;

		/************************ Blending ****************************/
			// IDLE
	case STATE_IDLE_TO_FORWARD:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_IDLE_TO_BACKWARD:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_IDLE_TO_LEFT_STRAFE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_IDLE_TO_RIGHT_STRAFE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_IDLE_TO_ATTACK:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_IDLE_TO_JUMP:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_IDLE_TO_T_POSE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			m_MapAnimData[KEY_T_POSE]->m_Name, m_MapAnimData[KEY_T_POSE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_IDLE_TO_DEATH:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			m_MapAnimData[KEY_DEATH]->m_Name, m_MapAnimData[KEY_DEATH]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
		// FORWARD
	case STATE_FORWARD_TO_BACKWARD:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time,
			m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_FORWARD_TO_IDLE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time,
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_FORWARD_TO_LEFT_STRAFE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time,
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_FORWARD_TO_RIGHT_STRAFE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time,
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_FORWARD_TO_ATTACK:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time,
			m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_FORWARD_TO_JUMP:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time,
			m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_FORWARD_TO_DEATH:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time,
			m_MapAnimData[KEY_DEATH]->m_Name, m_MapAnimData[KEY_DEATH]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
		// BACKWARD
	case STATE_BACKWARD_TO_FORWARD:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time,
			m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_BACKWARD_TO_IDLE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time,
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_BACKWARD_TO_LEFT_STRAFE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time,
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_BACKWARD_TO_RIGHT_STRAFE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time,
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_BACKWARD_TO_ATTACK:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time,
			m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_BACKWARD_TO_JUMP:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time,
			m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_BACKWARD_TO_DEATH:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time,
			m_MapAnimData[KEY_DEATH]->m_Name, m_MapAnimData[KEY_DEATH]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
		// LEFT_STRAFE
	case STATE_LEFT_STRAFE_TO_RIGHT_STRAFE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time,
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_LEFT_STRAFE_TO_IDLE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time,
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_LEFT_STRAFE_TO_FORWARD:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time,
			m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_LEFT_STRAFE_TO_BACKWARD:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time,
			m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_LEFT_STRAFE_TO_ATTACK:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time,
			m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_LEFT_STRAFE_TO_JUMP:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time,
			m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_LEFT_STRAFE_TO_DEATH:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time,
			m_MapAnimData[KEY_DEATH]->m_Name, m_MapAnimData[KEY_DEATH]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
		// RIGHT_STRAFE
	case STATE_RIGHT_STRAFE_TO_LEFT_STRAFE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time,
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_RIGHT_STRAFE_TO_IDLE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time,
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_RIGHT_STRAFE_TO_FORWARD:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time,
			m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_RIGHT_STRAFE_TO_BACKWARD:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time,
			m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_RIGHT_STRAFE_TO_ATTACK:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time,
			m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_RIGHT_STRAFE_TO_JUMP:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time,
			m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_RIGHT_STRAFE_TO_DEATH:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time,
			m_MapAnimData[KEY_DEATH]->m_Name, m_MapAnimData[KEY_DEATH]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
		// ATTACK
	case STATE_ATTACK_TO_IDLE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time,
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_ATTACK_TO_FORWARD:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time,
			m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_ATTACK_TO_BACKWARD:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time,
			m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_ATTACK_TO_LEFT_STRAFE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time,
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_ATTACK_TO_RIGHT_STRAFE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time,
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_ATTACK_TO_JUMP:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time,
			m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_ATTACK_TO_DEATH:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time,
			m_MapAnimData[KEY_DEATH]->m_Name, m_MapAnimData[KEY_DEATH]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
		// JUMP
	case STATE_JUMP_TO_IDLE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time,
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_JUMP_TO_FORWARD:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time,
			m_MapAnimData[KEY_FORWARD]->m_Name, m_MapAnimData[KEY_FORWARD]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_JUMP_TO_BACKWARD:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time,
			m_MapAnimData[KEY_BACKWARD]->m_Name, m_MapAnimData[KEY_BACKWARD]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_JUMP_TO_LEFT_STRAFE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time,
			m_MapAnimData[KEY_LEFT_STRAFE]->m_Name, m_MapAnimData[KEY_LEFT_STRAFE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_JUMP_TO_RIGHT_STRAFE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time,
			m_MapAnimData[KEY_RIGHT_STRAFE]->m_Name, m_MapAnimData[KEY_RIGHT_STRAFE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_JUMP_TO_ATTACK:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time,
			m_MapAnimData[KEY_ATTACK]->m_Name, m_MapAnimData[KEY_ATTACK]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	case STATE_JUMP_TO_DEATH:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_JUMP]->m_Name, m_MapAnimData[KEY_JUMP]->m_Time,
			m_MapAnimData[KEY_DEATH]->m_Name, m_MapAnimData[KEY_DEATH]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
		// T_POSE
	case STATE_T_POSE_TO_IDLE:
		m_CopySkinnedModelInst->ChangeSkinnedAnimation(
			m_MapAnimData[KEY_T_POSE]->m_Name, m_MapAnimData[KEY_T_POSE]->m_Time,
			m_MapAnimData[KEY_IDLE]->m_Name, m_MapAnimData[KEY_IDLE]->m_Time,
			(m_BlendFrame / m_MaxBlendFrames));
		break;
	}
	if (strState == "")
		return;
	if (m_MapAnimData[strState]->m_Time > m_CopySkinnedModelInst->SkinnedInfo->GetClipEndTime(strState))
	{
		m_MapAnimData[strState]->m_Time = 0.f;
	}
}
