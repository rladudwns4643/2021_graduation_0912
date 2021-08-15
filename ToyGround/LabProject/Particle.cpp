#include "pch.h"
#include "Particle.h"
#include "GeometryMesh.h"

Particle::Particle(std::string type, std::string id) :
	GameObject(type, id),
	m_ParticleName(""),
	m_ParticleCount(0),
	m_ParticleTotalTime(0.f),
	m_ParticlePlaySpeed(1.f),
	m_Owner(nullptr),
	m_ParticleOffset(0.f, 0.f, 0.f)
{
}

Particle::~Particle()
{
}

void Particle::Update(const float deltaT)
{
	if (isPlaying) {
		m_ParticleTotalTime += deltaT * m_ParticlePlaySpeed;

		if (isCapture) {
			SetPosition(m_CapturePosition);
		}
		else {
			if (m_Owner) SetPosition(m_Owner->GetPosition());
		}
	}
}

bool Particle::SetMesh(std::string meshName, std::string submeshName)
{
	if (GameObject::SetMesh(meshName, submeshName))
	{
		m_ParticleName = meshName;
		m_ParticleCount = m_Geo->IndexBufferByteSize / sizeof(std::uint16_t);

		return true;
	}

	return false;
}

void Particle::SetPosition(DirectX::XMFLOAT3 xmPos)
{
	GameObject::SetPosition(MathHelper::Add(xmPos, m_ParticleOffset));
}

void Particle::CapturePosition()
{
	isCapture = true;
	m_CapturePosition = m_Owner->GetPosition();
}

void Particle::PlayParticle()
{
	isPlaying = true;
	m_ParticleTotalTime = 0;
}

void Particle::StopParticle()
{
	isPlaying = false;
	isCapture = false;
	m_ParticleTotalTime = 0;
}

void Particle::SetIsLoop(bool isLoop)
{
	m_IsLoop = isLoop;
}

void Particle::SetOwner(GameObject* obj)
{
	if (!obj) return;

	m_Owner = obj;
}

void Particle::SetParticlePlaySpeed(float pTime)
{
	m_ParticlePlaySpeed = pTime;
}

void Particle::SetParticleOffset(XMFLOAT3 offset)
{
	m_ParticleOffset = offset;
}

bool Particle::GetIsLoop() const
{
	return m_IsLoop;
}

float Particle::GetParticleTotalTime() const
{
	return m_ParticleTotalTime;
}
