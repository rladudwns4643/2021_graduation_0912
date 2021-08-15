#include "pch.h"
#include "ParticleSystem.h"
#include "ApplicationContext.h"
#include "Particle.h"
#include "Character.h"

void ParticleSystem::Update(const float deltaT)
{
	// Particle Update
	for (auto& p : m_Particles)
	{
		p->Update(deltaT);
	}
}

void ParticleSystem::SetParticle(std::string particleName, std::string particleInstID)
{
	m_Particles.push_back(AppContext->FindObject<Particle>(particleName, particleInstID));
}

void ParticleSystem::SetCharacterParticle(std::string characterName, std::string characterInstID, std::string particleName, std::string particleInstID)
{
	m_Particles.push_back(AppContext->FindObject<Particle>(particleName, particleInstID));
	AppContext->FindObject<Character>(characterName, characterInstID)->SetParticle(particleName, particleInstID);
}