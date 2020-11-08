#pragma once

#include"FMOD/include/fmod.hpp"

class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	bool AddSound(size_t idx, LPCSTR szPath, bool bBGM = false);

	void Stop(bool bMasterStop = false);

	void PlayEffect(size_t idx);
	void PlayBGM(size_t idx);

private:
	FMOD::System* m_System{ nullptr };
	FMOD::Channel* m_Channel{ nullptr };
	FMOD::Sound* m_Sound[50];
};

