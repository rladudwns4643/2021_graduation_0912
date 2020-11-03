#include "MISC.h"
#include "SoundManager.h"

#pragma comment(lib,"FMOD/lib/fmod64_vc.lib")

SoundManager::SoundManager()
{
	FMOD::System_Create(&m_System);

	m_System->init(
		FMOD_MAX_CHANNEL_WIDTH
		, FMOD_INIT_NORMAL
		, nullptr
	);

	for (auto& p : m_Sound) p = nullptr;
}

SoundManager::~SoundManager()
{
	for (auto& p : m_Sound)
	{
		if (p) p->release();
		p = nullptr;
	}

	m_System->close();
	m_System->release();
}

bool SoundManager::AddSound(size_t idx, LPCSTR szPath, bool bBGM)
{
	if (idx < 0) return false;
	if (idx >= 50) return false;

	FMOD_RESULT res;
	if (bBGM)
	{
		res = m_System->createSound(
			szPath
			, FMOD_LOOP_NORMAL | FMOD_2D
			, nullptr
			, &m_Sound[idx]
		);
	}
	else
	{
		res = m_System->createSound(
			szPath
			, FMOD_DEFAULT
			, nullptr
			, &m_Sound[idx]
		);
	}
	return (res == FMOD_OK);
}

void SoundManager::Stop(bool bMasterStop)
{
	if (nullptr == m_Channel) return;
	bool hNowPlaying;
	m_Channel->isPlaying(&hNowPlaying);
	if (hNowPlaying)
		m_Channel->stop();

	if (!bMasterStop) return;

	FMOD::ChannelGroup *cg_master{ nullptr };
	m_System->getMasterChannelGroup(&cg_master);
	cg_master->stop();
}

void SoundManager::PlayEffect(size_t idx)
{
	if (idx < 0) return;
	if (idx >= 50) return;

	m_System->playSound(m_Sound[idx]
		, nullptr, false, nullptr);
}

void SoundManager::PlayBGM(size_t idx)
{
	if (idx < 0) return;
	if (idx >= 50) return;

	m_System->playSound(m_Sound[idx]
		, nullptr, false, &m_Channel);
}