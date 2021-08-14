#pragma once
#include "Scene.h"

#define CLIENT_EVENT_MATCHINGROOM_UI_PRESSED 0xFFFF10
#define CLIENT_EVENT_MATCHINGROOM_UI_INPUT_PRESSED 0xFFFF11

namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_UIPSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_SkinnedPSO;
}
namespace InputHandler
{
	extern bool g_CursorSwitch;
}
class Character;

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

	bool m_isReady1 = false;
	bool m_isReady2 = false;

	XMFLOAT2 m_ScaleConvert{ 1.f, 1.f };

	std::map<int, Character*> m_Toys;
};

