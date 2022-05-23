#pragma once
#include "Singleton.h"
#include <io.h>

class SoundManager : public TemplateSingleton<SoundManager>
{
public:
	enum CHANNEL_ID { BGM = 0, GEM, UI_BUTTON, Bullet, BulletCol, MAX_CHANNEL };
	void DestroyAll();

public:
	void ReadySoundManager(const char* pParentFolderPath);
	void UpdateSoundManager();

public:
	void PlaySoundOnce(const TCHAR* pSoundKey, CHANNEL_ID eID, float volume = 1.f);
	void PlaySoundLoop(const TCHAR* pSoundKey, CHANNEL_ID eID, float volume = 1.f);
	void PlayBGM(const TCHAR* pSoundKey, float volume = 1.f); // PlayBGM 노래 무한반복
	void StopBGM();
	void StopSound(CHANNEL_ID eID);
	void StopAll();

private:
	void LoadSoundFile(const char* pParentFolderPath);

private:
	std::map<const TCHAR*, FMOD_SOUND*>		m_MapSound;
	FMOD_CHANNEL* m_pChannelArr[MAX_CHANNEL];
	FMOD_SYSTEM* m_pSystem;

};

