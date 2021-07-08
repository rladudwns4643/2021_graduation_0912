#pragma once
#include "Scene.h"

#define CLIENT_EVENT_TITLE_UI_PRESSED 0xFFFF10
#define CLIENT_EVENT_TITLE_UI_INPUT_PRESSED 0xFFFF11

namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_UIPSO;
}

enum CHATTYPE
{
	NONE, ID, PASSWORD
};

enum MBType
{
	BLANK, OK, DENY
};

class TitleScene : public Scene
{
	friend class TitleController;

private:
	virtual void ProcessEvent(int sEvent, int argsCount = 0, ...) override;
	virtual void UIEvent(int sEvent, int argsCount = 0, ...) override;

public:
	virtual void Initialize() override;
	virtual void OnResize() override;

public:
	virtual bool Enter() override;
	virtual void Exit() override;

	virtual void Update(const float& fDeltaTime) override;
	virtual void Render() override;
	virtual void RenderUI() override;

private:
	std::string _id;
	std::string _password;

public:
	std::wstring m_ID;
	std::wstring m_Password;

	int m_ChatType;

	bool m_IsSignUp = false;
	bool m_IsMB = false;
	wstring m_MBMainMessage;
	wstring m_MBSubMessage;
	MBType m_MBType = MBType::BLANK;
};

