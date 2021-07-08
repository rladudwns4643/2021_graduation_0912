#pragma once

class UserInterface;
class SpriteAnimation
{
public:
	explicit SpriteAnimation(UserInterface* owner, float durationTime);
	explicit SpriteAnimation(UserInterface* owner, float durationTime, float animationSpeed);
	~SpriteAnimation() = default;

public:
	void Initialize();

	void PlayAnimation(const float deltaT);
	void PlayAnimationLoop(const float deltaT);

public:
	void AddFrame(int matIndex);
	void SetDurationTime(float durationTime);

private:
	UserInterface* m_Owner;

	std::vector<int> m_SpriteFrames;
	int m_FollowPointer;

	float m_SpriteTotalTime;
	float m_SpriteDurationTime;
	float m_SpriteAnimationSpeed;
};

