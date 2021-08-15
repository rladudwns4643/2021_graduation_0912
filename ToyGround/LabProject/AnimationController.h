#pragma once

class Character;
class SkinnedModelInstance;
class AnimationController
{
public:
	explicit AnimationController(Character* owner, SkinnedModelInstance* skinnedModelInst);
	virtual ~AnimationController();

	void Update(const float deltaT);

	void SetState(const float deltaT);
	void UpdateBoneTransforms();

private:
	Character* m_Owner;
public:
	SkinnedModelInstance* m_CopySkinnedModelInst;

public:
	enum PlayerState
	{
		STATE_IDLE,
		STATE_IDLE_TO_FORWARD,
		STATE_IDLE_TO_BACKWARD,
		STATE_IDLE_TO_LEFT_STRAFE,
		STATE_IDLE_TO_RIGHT_STRAFE,
		STATE_IDLE_TO_ATTACK,
		STATE_IDLE_TO_JUMP,
		STATE_IDLE_TO_T_POSE,

		STATE_FORWARD,
		STATE_FORWARD_TO_IDLE,
		STATE_FORWARD_TO_BACKWARD,
		STATE_FORWARD_TO_LEFT_STRAFE,
		STATE_FORWARD_TO_RIGHT_STRAFE,
		STATE_FORWARD_TO_ATTACK,
		STATE_FORWARD_TO_JUMP,

		STATE_BACKWARD,
		STATE_BACKWARD_TO_IDLE,
		STATE_BACKWARD_TO_FORWARD,
		STATE_BACKWARD_TO_LEFT_STRAFE,
		STATE_BACKWARD_TO_RIGHT_STRAFE,
		STATE_BACKWARD_TO_ATTACK,
		STATE_BACKWARD_TO_JUMP,

		STATE_LEFT_STRAFE,
		STATE_LEFT_STRAFE_TO_IDLE,
		STATE_LEFT_STRAFE_TO_FORWARD,
		STATE_LEFT_STRAFE_TO_BACKWARD,
		STATE_LEFT_STRAFE_TO_RIGHT_STRAFE,
		STATE_LEFT_STRAFE_TO_ATTACK,
		STATE_LEFT_STRAFE_TO_JUMP,

		STATE_RIGHT_STRAFE,
		STATE_RIGHT_STRAFE_TO_IDLE,
		STATE_RIGHT_STRAFE_TO_FORWARD,
		STATE_RIGHT_STRAFE_TO_BACKWARD,
		STATE_RIGHT_STRAFE_TO_LEFT_STRAFE,
		STATE_RIGHT_STRAFE_TO_ATTACK,
		STATE_RIGHT_STRAFE_TO_JUMP,

		STATE_ATTACK,
		STATE_ATTACK_TO_IDLE,
		STATE_ATTACK_TO_FORWARD,
		STATE_ATTACK_TO_BACKWARD,
		STATE_ATTACK_TO_LEFT_STRAFE,
		STATE_ATTACK_TO_RIGHT_STRAFE,
		STATE_ATTACK_TO_JUMP,

		STATE_JUMP,
		STATE_JUMP_TO_IDLE,
		STATE_JUMP_TO_FORWARD,
		STATE_JUMP_TO_BACKWARD,
		STATE_JUMP_TO_LEFT_STRAFE,
		STATE_JUMP_TO_RIGHT_STRAFE,
		STATE_JUMP_TO_ATTACK,

		STATE_T_POSE,
		STATE_T_POSE_TO_IDLE,
	};

	PlayerState m_PlayerState;
	PlayerState m_KeyState;

public:
	class AnimData
	{
	public:
		explicit AnimData(std::string name, float time) : m_Name(name), m_Time(time) {};
		virtual ~AnimData() = default;
		std::string m_Name;
		float m_Time;
	};

	std::map<std::string, std::unique_ptr<AnimData>> m_MapAnimData;

	float m_BlendFrame;
	float m_MaxBlendFrames;
};

