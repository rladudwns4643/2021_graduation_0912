#include "pch.h"
#include "SoundManager.h"

void SoundManager::DestroyAll()
{
	for (auto& p : m_MapSound)
	{
		FMOD_Sound_Release(p.second);
	}

	FMOD_System_Close(m_pSystem);
	FMOD_System_Release(m_pSystem);
}

void SoundManager::ReadySoundManager(const char* pParentFolderPath)
{
	FMOD_System_Create(&m_pSystem);
	FMOD_System_Init(m_pSystem, MAX_CHANNEL, FMOD_INIT_NORMAL, nullptr);

	LoadSoundFile(pParentFolderPath);
}

void SoundManager::UpdateSoundManager()
{
	FMOD_System_Update(m_pSystem);
}

void SoundManager::PlaySoundOnce(const TCHAR* pSoundKey, CHANNEL_ID eID, float volume)
{
	if (volume < 0.f) volume = 0.f;

	if (m_MapSound.empty())
		return;

	auto& iter = find_if(m_MapSound.begin(), m_MapSound.end(),
		[&pSoundKey](auto& MyPair)
		{
			return !lstrcmp(pSoundKey, MyPair.first);
		});

	if (m_MapSound.end() == iter)
	{
		MessageBox(0, TEXT("Not Find SoundKey!"), TEXT("System Message"), MB_OK);
		return;
	}

	FMOD_System_PlaySound(m_pSystem, iter->second, FALSE, 0, &m_pChannelArr[eID]);
	FMOD_Channel_SetVolume(m_pChannelArr[eID], volume);
	// deltaT= 0;
}

void SoundManager::PlaySoundLoop(const TCHAR* pSoundKey, CHANNEL_ID eID)
{
	if (m_MapSound.empty()) {
		cout << "mapSound empty" << endl;
		return;
	}

	auto& iter = find_if(m_MapSound.begin(), m_MapSound.end(),
		[&pSoundKey](auto& MyPair)
		{
			return !lstrcmp(pSoundKey, MyPair.first);
		});

	if (m_MapSound.end() == iter)
	{
		_MSG_BOX("Not Find SoundKey!");
		return;
	}

	FMOD_System_PlaySound(m_pSystem, iter->second, NULL, 0, &m_pChannelArr[eID]);
	FMOD_Channel_SetMode(m_pChannelArr[eID], FMOD_LOOP_NORMAL);
}

void SoundManager::PlayBGM(const TCHAR* pSoundKey, float volume)
{
	if (m_MapSound.empty())
		return;

	auto& iter = find_if(m_MapSound.begin(), m_MapSound.end(),
		[&pSoundKey](auto& MyPair)
		{
			return !lstrcmp(pSoundKey, MyPair.first);
		});

	if (m_MapSound.end() == iter)
	{
		MessageBox(0, TEXT("Not Find SoundKey!"), TEXT("System Message"), MB_OK);
		return;
	}

	FMOD_System_PlaySound(m_pSystem, iter->second, NULL, 0, &m_pChannelArr[BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[BGM], volume);
}

void SoundManager::StopSound(CHANNEL_ID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void SoundManager::StopAll()
{
	for (int i = 0; i < MAX_CHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void SoundManager::LoadSoundFile(const char* pParentFolderPath)
{
	struct _finddata_t fd;

	char szFileName[128] = "";
	strcpy_s(szFileName, pParentFolderPath);
	strcat_s(szFileName, "*.mp3");

	intptr_t handle = _findfirst(szFileName, &fd);

	if (0 == handle)
	{
		_MSG_BOX("Not Found Sound Folder!");
		return;
	}

	if (handle == -1)
		goto FIND_WAVE;

	int iResult = 0;
	char szFullPath[128] = "";

	while (-1 != iResult)
	{
		strcpy_s(szFullPath, pParentFolderPath);
		strcat_s(szFullPath, fd.name);

		FMOD_SOUND* pSound = nullptr;
		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, NULL, &pSound);

		if (FMOD_OK == eRes)
		{
			int iLength = static_cast<int>(strlen(fd.name)) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_MapSound.insert({ pSoundKey, pSound });
		}

		iResult = _findnext(handle, &fd);
	}

FIND_WAVE:
	strcpy_s(szFileName, pParentFolderPath);
	strcat_s(szFileName, "*.wav");

	handle = _findfirst(szFileName, &fd);

	if (0 == handle)
	{
		_MSG_BOX("Not Found Sound Folder!");
		return;
	}

	iResult = handle;
	while (-1 != iResult)
	{
		strcpy_s(szFullPath, pParentFolderPath);
		strcat_s(szFullPath, fd.name);

		FMOD_SOUND* pSound = nullptr;
		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, NULL, &pSound);

		if (FMOD_OK == eRes)
		{
			int iLength = static_cast<int>(strlen(fd.name)) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_MapSound.insert({ pSoundKey, pSound });
		}

		iResult = _findnext(handle, &fd);
	}

	_findclose(handle);

	FMOD_System_Update(m_pSystem);
}
