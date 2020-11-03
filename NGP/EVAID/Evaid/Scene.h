#pragma once
#include "MISC.h"
class CFramework;

class CScene {
public:
	CScene();
	virtual ~CScene();
	virtual void update();
	virtual void draw(HDC hDC);
	virtual bool init(CFramework* pFramework, HWND hWnd);
	virtual bool Keyboard(UINT msg, WPARAM w, LPARAM l);
	virtual bool Mouse(UINT msg, WPARAM w, LPARAM l);
	virtual void ReleaseObjects();

protected:
	CFramework* m_Framework{ nullptr };
	HWND m_hWnd{ nullptr };
};