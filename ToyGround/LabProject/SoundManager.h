#pragma once
#include "Singleton.h"
#include <io.h>

class SoundManager : public TemplateSingleton<SoundManager>
{
public:
	enum CHANNEL_ID { BGM, PLAYER_RUNNING, PLAYER_JUMP, PLAYER_ATTACK, PLAYER_TRANSFORM, PLAYER_BEAT, PLAYER_PENALTY, PLAYER_PENALTY_WARNING, UI_BUTTON, MAX_CHANNEL /*�ؽ�Ʈ �Է��Ҷ�, �÷��̾� ��ŷ, �÷��̾� ���� ���,, ���α����س��°�����.*/ };
	void DestroyAll();

public:
	void ReadySoundManager(const char* pParentFolderPath);
	void UpdateSoundManager();

public:
	void PlaySoundOnce(const TCHAR* pSoundKey, CHANNEL_ID eID, float volume = 1.f);
	void PlaySoundLoop(const TCHAR* pSoundKey, CHANNEL_ID eID);
	void PlayBGM(const TCHAR* pSoundKey, float volume = 1.f); // PlayBGM �뷡 ���ѹݺ�
	void StopSound(CHANNEL_ID eID);
	void StopAll();

private:
	void LoadSoundFile(const char* pParentFolderPath);

private:
	std::map<const TCHAR*, FMOD_SOUND*>		m_MapSound;
	FMOD_CHANNEL* m_pChannelArr[MAX_CHANNEL];
	FMOD_SYSTEM* m_pSystem;

};

