#pragma once
#include "Singleton.h"

class Particle;
class ParticleSystem : public TemplateSingleton<ParticleSystem>
{
public:
	void Update(const float deltaT);

	void SetParticle(std::string particleName, std::string particleInstID);
	void SetCharacterParticle(std::string characterName, std::string characterInstID, std::string particleName, std::string particleInstID);
	
private:
	std::vector<Particle*> m_Particles;
};