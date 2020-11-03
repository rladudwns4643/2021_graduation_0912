#include "Scene.h"

CScene::CScene() {}
CScene::~CScene() { ReleaseObjects(); }

void CScene::update() {}
void CScene::draw(HDC hDC) {}
bool CScene::init(CFramework* pFramework, HWND hWnd) {
	m_Framework = pFramework;
	m_hWnd = hWnd;
	return true;
}

bool CScene::Keyboard(UINT msg, WPARAM w, LPARAM l) { return false; }
bool CScene::Mouse(UINT msg, WPARAM w, LPARAM l) { return false; }
void CScene::ReleaseObjects() {}