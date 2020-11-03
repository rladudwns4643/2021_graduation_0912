#include "MISC.h"
#include "ResorceTable.h"
#include "Framework.h"
#include "logoScene.h"

CLogoScene::CLogoScene() {}
CLogoScene::~CLogoScene() {}
void CLogoScene::update() {

	//todo: 다음 화면으로 넘기는 트리거
	m_Framework->PopScene();
}

void CLogoScene::draw(HDC hDC) {
	RECT rc = m_Framework->getClientRect();

	ResorceTable::getInstance()->img_logo_bg.Draw(hDC, 0, 0);
}

bool CLogoScene::init(CFramework* pFramework, HWND hWnd) {
	if (!CScene::init(pFramework, hWnd)) return false;
	return true;
}

bool CLogoScene::Keyboard(UINT msg, WPARAM w, LPARAM l) {
	return true;
}

bool CLogoScene::Mouse(UINT msg, WPARAM w, LPARAM l) {
	POINT M;
	M.x = LOWORD(l);
	M.y = HIWORD(l);

	if (!(M.x > 700 && M.x < 1100)) return false;

	if (M.y > 126 && M.y < 234) {
		//start btn
		return true;
	}
	if (M.y > 306 && M.y < 414) {
		//rank btn
		return true;
	}
	if (M.y > 486 && M.y < 594) {
		//help btn
		return true;
	}
	if (M.y > 666 && M.y < 774) {
		PostQuitMessage(0);
		return true; // 영원히 호출 안됨
	}
}

void CLogoScene::ReleaseObjects() {}