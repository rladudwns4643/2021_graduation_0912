#pragma once
#include "Scene.h"

class CLogoScene : public CScene {
public:
	CLogoScene();
	~CLogoScene() override;

	void update() override;
	void draw(HDC hDC) override;
	bool init(CFramework* pFramework, HWND hWnd) override;
	bool Keyboard(UINT msg, WPARAM w, LPARAM l) override;
	bool Mouse(UINT msg, WPARAM w, LPARAM l) override;
	void ReleaseObjects() override;

private:
	float m_fFade{ 0.f };
	float m_fMaxFade{ 2.f };
};