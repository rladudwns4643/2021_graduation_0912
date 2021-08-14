#pragma once
#include "Scene.h"

#define CLIENT_EVENT_MATCHINGROOM_UI_PRESSED 0xFFFF10
#define CLIENT_EVENT_MATCHINGROOM_UI_INPUT_PRESSED 0xFFFF11

namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_UIPSO;
}

enum MBType
{
	BLANK, OK, DENY
};

class MatchingRoomScene : public Scene
{
	friend class MatchingRoomController;

public:
	virtual void Initialize() override;
	virtual void OnResize() override;

public:
	virtual bool Enter() override;
	virtual void Exit() override;

	virtual void Update(const float& fDeltaTime) override;
	virtual void Render() override;
	virtual void RenderText() override;

public:

	bool m_IsMB = false;
	wstring m_MBMainMessage;
	wstring m_MBSubMessage;
	MBType m_MBType = MBType::BLANK;
};

