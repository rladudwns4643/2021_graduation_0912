#pragma once
#include "GameObject.h"

class Particle : public GameObject
{
	bool isPlaying = false;
	bool isCapture = false;

public:
	explicit Particle(std::string type, std::string id);
	virtual ~Particle();

	virtual void Update(const float deltaT) override;
	virtual bool SetMesh(std::string meshName, std::string submeshName) override;
	virtual void SetPosition(DirectX::XMFLOAT3 xmPos) override;

public:
	void	CapturePosition();

	void	PlayParticle();
	void	StopParticle();

	void	SetIsLoop(bool isLoop);
	void	SetOwner(GameObject* obj);
	void	SetParticlePlaySpeed(float pTime);
	void	SetParticleOffset(XMFLOAT3 offset);

	bool	GetIsLoop() const;
	float	GetParticleTotalTime() const;

private:
	GameObject* m_Owner;
	bool m_IsLoop = false;

	std::string m_ParticleName;
	int			m_ParticleCount;

	float		m_ParticleTotalTime;
	float		m_ParticlePlaySpeed;
	XMFLOAT3	m_ParticleOffset;

	XMFLOAT3	m_CapturePosition;
};

