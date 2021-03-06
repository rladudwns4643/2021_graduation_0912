#pragma once
const ULONG MAX_SAMPLE_COUNT = 50; // 50ȸ�� ������ ó���ð��� �����Ͽ� ����Ѵ�.
class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();

	// Ÿ�̸��� �ð� ����
	void Tick(float fLockFPS = 0.0f);
	void Start();
	void Stop();
	void Reset();

	// ������ ����Ʈ ��ȯ
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);

	// ������ ��� ��� �ð� ��ȯ
	float GetTimeElapsed();

	// �� ���� �ð�
	float GetTotalTime();

private:
	double							m_fTimeScale;					// Scale Counter�� ��
	float							m_fTimeElapsed;					// ������ ������ ���� ������ �ð�

	__int64							m_nBasePerformanceCounter;
	__int64							m_nPausedPerformanceCounter;
	__int64							m_nStopPerformanceCounter;
	__int64							m_nCurrentPerformanceCounter;	// ������ �ð�
	__int64							m_nLastPerformanceCounter;		// ������ �������� �ð�

	__int64							m_nPerformanceFrequencyPerSec;	// ��ǻ���� Performance Frequency

	float							m_fFrameTime[MAX_SAMPLE_COUNT];	// ������ �ð��� �����ϱ� ���� �迭
	ULONG							m_nSampleCount;					// ������ ������ Ƚ��

	unsigned long					m_nCurrentFrameRate;			// ������ ������ ����Ʈ
	unsigned long					m_nFramesPerSecond;				// �ʴ� ������ ��
	float							m_fFPSTimeElapsed;				// ������ ����Ʈ ��� �ҿ�ð�

	bool							m_bStopped;
};