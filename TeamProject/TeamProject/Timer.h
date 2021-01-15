#pragma once
#include "stdafx.h"

const ULONG MAX_SAMPLE_COUNT = 50; // 50ȸ�� ������ ó���ð��� �����Ͽ� �����

class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();

	void Tick(float fLockFPS = 0.0f);											// Ÿ�̸��� �ð��� ����
	void Start();																// ����
	void Stop();																// ����
	void Reset();																// �ð� ����

	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);	// ������ ����Ʈ ��ȯ
	float GetTimeElapsed();														// �������� ��� ��� �ð� ��ȯ
	float GetTotalTime();


private:
	float							m_fTimeScale;					// Scale Couter�� ��
	float							m_fTimeElapsed;					// ������ ������ ���� ������ �ð�

	__int64							m_nBasePerformanceCounter;		//
	__int64							m_nPausedPerformanceCounter;	//
	__int64							m_nStopPerformanceCounter;		//
	__int64							m_nCurrentPerformanceCounter;	// ������ �ð�
	__int64							m_nLastPerformanceCounter;		// ������ �������� �ð�

	__int64							m_nPerformanceFrequencyPerSec;	//

	float							m_fFrameTime[MAX_SAMPLE_COUNT];	// ������ �ð��� �����ϱ� ���� �迭
	ULONG							m_nSampleCount;					// ������ ������ Ƚ��

	unsigned long					m_nCurrentFrameRate;			// ������ ������ ����Ʈ
	unsigned long					m_nFramesPerSecond;				// �ʴ� ������ ��
	float							m_fFPSTimeElapsed;				// ������ ����Ʈ ��� �ҿ�ð�

	bool							m_bStopped;						// ���� ����
};