#include "pch.h"
#include "SpriteAnimation.h"
#include "UserInterface.h"

SpriteAnimation::SpriteAnimation(UserInterface* owner, float durationTime) :
	m_SpriteTotalTime(0.f),
	m_SpriteAnimationSpeed(1.f),
	m_FollowPointer(0),
	m_SpriteDurationTime(durationTime)
{
	if (!owner) cout << "SpriteAnimation owner is nullptr" << endl;
	m_Owner = owner;
}

SpriteAnimation::SpriteAnimation(UserInterface* owner, float durationTime, float animationSpeed) :
	m_SpriteTotalTime(0.f),
	m_FollowPointer(0),
	m_SpriteAnimationSpeed(animationSpeed),
	m_SpriteDurationTime(durationTime)
{
	if (!owner) cout << "SpriteAnimation owner is nullptr" << endl;
	m_Owner = owner;
}

void SpriteAnimation::Initialize()
{
	m_SpriteTotalTime = 0.f;
	m_FollowPointer = 0;
}

void SpriteAnimation::PlayAnimation(const float deltaT)
{
	if (m_SpriteTotalTime >= m_SpriteFrames.size()) {
		m_Owner->m_MaterialIndex = m_Owner->GetOriginMaterial();
		return;
	}

	m_SpriteTotalTime += deltaT * (m_SpriteFrames.size() / m_SpriteDurationTime);
	m_FollowPointer = std::floor(m_SpriteTotalTime);

	if (m_FollowPointer >= m_SpriteFrames.size()) {
		m_FollowPointer = 0;
	}

	int frame = m_SpriteFrames[m_FollowPointer];
	m_Owner->m_MaterialIndex = frame;
}

void SpriteAnimation::PlayAnimationLoop(const float deltaT)
{
	m_SpriteTotalTime += deltaT * (m_SpriteFrames.size() / m_SpriteDurationTime);
	m_FollowPointer = std::floor(m_SpriteTotalTime);

	// 벡터 널포인터 방지 예외처리
	if (m_FollowPointer >= m_SpriteFrames.size()) {
		m_SpriteTotalTime = 0;
		m_FollowPointer = 0;
	}

	int frame = m_SpriteFrames[m_FollowPointer];
	m_Owner->m_MaterialIndex = frame;
}

void SpriteAnimation::AddFrame(int matIndex)
{
	m_SpriteFrames.emplace_back(matIndex);
}

void SpriteAnimation::SetDurationTime(float durationTime)
{
	m_SpriteDurationTime = durationTime;
}
