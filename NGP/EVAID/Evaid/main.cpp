#include "MISC.h"
#include "ResorceTable.h"
#include "Global.h"
#include "Table.h"
#include "Hero.h"
#include "resource.h"

static HINSTANCE g_hInst;						// ���� �ν��Ͻ�
static LPCTSTR lpszTitle = TEXT("EVAID TITLE");
static LPCTSTR lpszClass = TEXT("EVAID CLASS");

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void CALLBACK TimerProc(HWND hWnd, UINT iMessage, UINT_PTR idEvent, DWORD dwTime);

// ��Ʈ�Լ�
vector<BYTE> ReadFontOutputFile(LPCTSTR path)
{
	ifstream fFont(path, ios::in | ios::binary);

	if (!fFont.is_open()) {
		MessageBox(NULL, TEXT("Font Load Fail!"), TEXT("���α׷� ���� ����"), MB_OK);
		return vector<BYTE>();
	}

	// size check;
	fFont.seekg(0, fstream::end);
	size_t szData = fFont.tellg();
	fFont.seekg(0);

	vector<BYTE> vbFont;
	vbFont.resize(szData);

	fFont.read(reinterpret_cast<char*>(&(vbFont[0])), szData);
	fFont.close();

	return vbFont;
}

CTable board_1(1);
CTable board_2(2);

CHero Hero1P(Global::getInstance()->Player1HeroKind, 1);
CHero Hero2P(Global::getInstance()->Player2HeroKind, 2);

int Hero1Score = 0;
int Hero2Score = 0;

CImage doublebuffer;
CImage doublebuffer_Stage_1;
CImage doublebuffer_Stage_2;

bool ShakeWindow = true;

HWND Stage_1_hWnd;
HWND Stage_2_hWnd;

LRESULT CALLBACK Stage_1_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Stage_2_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	DWORD dwStyle =
		WS_OVERLAPPED 		// ����Ʈ ������. Ÿ��Ʋ �ٿ� ũ�� ������ �ȵǴ� ��輱�� ������. �ƹ��� ��Ÿ�ϵ� ���� ������ �� ��Ÿ���� ����ȴ�.
		| WS_CAPTION 		// Ÿ��Ʋ �ٸ� ���� �����츦 ����� WS_BORDER ��Ÿ���� �����Ѵ�.
		| WS_SYSMENU		// �ý��� �޴��� ���� �����츦 �����.
		| WS_MINIMIZEBOX	// �ּ�ȭ ��ư�� �����.
		| WS_BORDER			// �ܼ����� �� ��輱�� ����� ũ�� ������ �� �� ����.
//		| WS_THICKFRAME		// ũ�� ������ ������ �β��� ��輱�� ������. WS_BORDER�� ���� ����� �� ����.
		;					// �߰��� �ʿ��� ������ ��Ÿ���� http://www.soen.kr/lecture/win32api/reference/Function/CreateWindow.htm ����.

	RECT getWinSize;
	GetWindowRect(GetDesktopWindow(), &getWinSize);

	RECT clientSize;
	clientSize.left = clientSize.top = 0;
	clientSize.right = CLIENT_WIDTH;
	clientSize.bottom = CLIENT_HEIGHT;
	AdjustWindowRect(&clientSize, dwStyle, FALSE);

	//Ŭ���� ������ǥ (������� �߾ӿ� ��ġ)
	POINT ptClientWorld;
	ptClientWorld.x = (getWinSize.right - clientSize.right) / 2;
	ptClientWorld.y = (getWinSize.bottom - clientSize.bottom) / 2;

	HWND hWnd = CreateWindow(
		lpszClass							// ������ Ŭ���� ��
		, lpszTitle							// ĸ�� ǥ�� ���ڿ�
		, dwStyle							// ������ ��Ÿ��
		, ptClientWorld.x					// �θ� ������ ��� ������ ������ǥ x
		, ptClientWorld.y					// �θ� ������ ��� ������ ������ǥ y
		, clientSize.right - clientSize.left//	������ ������ : width
		, clientSize.bottom - clientSize.top//	������ ������ : height
		, NULL								// �θ� ������
		, (HMENU)NULL						// �޴� �ڵ�
		, hInstance							// �ν��Ͻ� �ڵ�
		, NULL								// �߰� �Ķ����
	);
	if (!hWnd) return false;

	//������ ǥ��
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return true;
}

void MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX WndClass;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style =
		CS_HREDRAW		// Ŭ���̾�Ʈ�� �ʺ� ����Ǹ�, ��ü �����츦 �ٽ� �׸�
		| CS_VREDRAW	// Ŭ���̾�Ʈ�� ���̰� ����Ǹ�, ��ü �����츦 �ٽ� �׸�
//		| CS_DBLCLKS	// ����Ŭ���� ����ϱ� ���ؼ� �߰���
;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;

	WndClass.hIcon = LoadIcon(g_hInst, (LPCTSTR)IDI_ICON1);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(g_hInst, (LPCTSTR)IDI_ICON1);
	RegisterClassEx(&WndClass);

	//----------------------------P1
	WndClass.hCursor = LoadCursor(NULL, IDI_APPLICATION);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	WndClass.lpszClassName = TEXT("Stage_1");
	WndClass.lpfnWndProc = Stage_1_Proc;
	RegisterClassEx(&WndClass);

	//----------------------------P2
	WndClass.hCursor = LoadCursor(NULL, IDI_APPLICATION);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	WndClass.lpszClassName = TEXT("Stage_2");
	WndClass.lpfnWndProc = Stage_2_Proc;
	RegisterClassEx(&WndClass);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	g_hInst = hInstance;
	srand(static_cast<unsigned>(time(NULL)));
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow)) return false;
	HACCEL hAccelTable = LoadAccelerators(hInstance, lpszTitle);
	
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC;

	switch (iMessage) {
#pragma region �ʱ�ȭ
	case WM_CREATE:
		Global::getInstance()->rankMng.load();
		Stage_1_hWnd = CreateWindowEx(
			WS_EX_CLIENTEDGE
			, TEXT("Stage_1")
			, NULL
			, WS_CHILD | WS_VISIBLE | WS_BORDER
			, 0, 0, STAGE_WIDTH, STAGE_HEIGHT
			, hWnd
			, NULL
			, g_hInst
			, NULL);

		Stage_2_hWnd = CreateWindowEx(
			WS_EX_CLIENTEDGE
			, TEXT("Stage_2")
			, NULL
			, WS_CHILD | WS_VISIBLE | WS_BORDER
			, STAGE_WIDTH, 0, CLIENT_WIDTH, STAGE_HEIGHT
			, hWnd
			, NULL
			, g_hInst
			, NULL);

		doublebuffer.Create(CLIENT_WIDTH, CLIENT_HEIGHT, 24);
		ShakeWindow = true;
		// ��Ʈ ����
		{
			static auto vFont = ReadFontOutputFile(L"Font/ARCADECLASSIC.TTF");
			static DWORD fi;
			AddFontMemResourceEx(&(vFont[0]), vFont.size(), 0, &fi);
		}

		// �����
		ResorceTable::getInstance()->m_Sound.PlayBGM(0);

		break;
#pragma endregion

	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
		case ID_PLAYER01_1:
			Global::getInstance()->Player1HeroKind = HERO::Hero1;
			break;
		case ID_PLAYER01_2:
			Global::getInstance()->Player1HeroKind = HERO::Hero2;
			break;

		case ID_PLAYER02_1:
			Global::getInstance()->Player2HeroKind = HERO::Hero1;
			break;
		case ID_PLAYER02_2:
			Global::getInstance()->Player2HeroKind = HERO::Hero2;
			break;
		}
	}

#pragma region �׸���
	case WM_PAINT:
	{
		HDC hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;
#pragma endregion

#pragma region Ű���� I/0 ���ڴ� wParam
	case WM_KEYDOWN:
		GetKeyDown(wParam);
		break;
	case WM_KEYUP:
		GetKeyUp(wParam);
		break;
	case WM_CHAR:		break;
#pragma endregion

#pragma region ���� �� ȣ��.		�����츦 �Ҹ��Ű�� ������ DestoryWindow(hWnd)
	case WM_DESTROY:
		Global::getInstance()->rankMng.save(Hero1Score);
		Global::getInstance()->rankMng.save(Hero2Score);
		PostQuitMessage(0);
		return 0;
#pragma endregion

	default:
		return DefWindowProc(hWnd, iMessage, wParam, lParam);
	}

	return 0;
}

void OnDraw_1(HDC hDC)
{
	if (!Global::getInstance()->isStart)
	{
		ResorceTable::getInstance()->img_main_bg_1.Draw(hDC, 0, 0);
	}
	if (Global::getInstance()->isStart)
	{
		RECT temp{ 0,0,STAGE_WIDTH,CLIENT_HEIGHT };
		FillRect(hDC, &temp, (HBRUSH)GetStockObject(WHITE_BRUSH));

		board_1.draw(hDC);
		if (!Global::getInstance()->Gameover_1)
		{
			Hero1P.draw(hDC);
		}

		// ���ھ�
		{
			HFONT hFont = CreateFont(40, 0, 0, 0, 0, 0, 0, 0
				, DEFAULT_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN
				, TEXT("ArcadeClassic"));

			HFONT oldFont = (HFONT)SelectObject(hDC, hFont);

			SetBkMode(hDC, TRANSPARENT);
			SetTextColor(hDC, RGB(255, 255, 255));

			TCHAR ScorePrint[100];
			RECT textRect;
			textRect.left = PTStartX;
			textRect.right = CLIENT_WIDTH / 2 - PTStartX;
			textRect.top = 0;
			textRect.bottom = PTStartY;

			if (!Global::getInstance()->Gameover_1)
			{
				Hero1Score = Global::getInstance()->TimerTick;
			}

			// ����� �ؽ�Ʈ �Է�
			wsprintf(ScorePrint, L"Score: %d", Hero1Score);
			DrawText(hDC, ScorePrint, lstrlen(ScorePrint), &textRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			SelectObject(hDC, oldFont);
			DeleteObject(hFont);
		}

		if (Global::getInstance()->Gameover_1)
		{
			HFONT hFont = CreateFont(75, 0, 0, 0, 0, 0, 0, 0
				, DEFAULT_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN
				, TEXT("ArcadeClassic"));

			HFONT oldFont = (HFONT)SelectObject(hDC, hFont);

			SetBkMode(hDC, TRANSPARENT);
			SetTextColor(hDC, RGB(255, 255, 255));

			RECT textRect;
			textRect.left = 20;
			textRect.right = CLIENT_WIDTH / 2;
			textRect.top = 0;
			textRect.bottom = CLIENT_HEIGHT;

			// ����� �ؽ�Ʈ �Է�
			DrawText(hDC, L"Game Over!", lstrlen(L"Game Over!"), &textRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			SelectObject(hDC, oldFont);
			DeleteObject(hFont);
		}
	}
}

void OnDraw_2(HDC hDC)
{
	if (!Global::getInstance()->isStart)
	{
		ResorceTable::getInstance()->img_main_bg_2.Draw(hDC, 0, 0);
	}
	if (Global::getInstance()->isStart)
	{
		RECT temp{ 0, 0, STAGE_WIDTH,CLIENT_HEIGHT };
		FillRect(hDC, &temp, (HBRUSH)GetStockObject(WHITE_BRUSH));

		board_2.draw(hDC);
		if (!Global::getInstance()->Gameover_2)
		{
			Hero2P.draw(hDC);
		}

		// ���ھ�
		{
			HFONT hFont = CreateFont(40, 0, 0, 0, 0, 0, 0, 0
				, DEFAULT_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN
				, TEXT("ArcadeClassic"));

			HFONT oldFont = (HFONT)SelectObject(hDC, hFont);

			SetBkMode(hDC, TRANSPARENT);
			SetTextColor(hDC, RGB(255, 255, 255));

			TCHAR ScorePrint[100];
			RECT textRect;
			textRect.left = PTStartX;
			textRect.right = CLIENT_WIDTH / 2 - PTStartX;
			textRect.top = 0;
			textRect.bottom = PTStartY;

			if (!Global::getInstance()->Gameover_2)
			{
				Hero2Score = Global::getInstance()->TimerTick;
			}

			// ����� �ؽ�Ʈ �Է�
			wsprintf(ScorePrint, L"Score: %d", Hero2Score);
			DrawText(hDC, ScorePrint, lstrlen(ScorePrint), &textRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			SelectObject(hDC, oldFont);
			DeleteObject(hFont);
		}

		if (Global::getInstance()->Gameover_2)
		{
			HFONT hFont = CreateFont(75, 0, 0, 0, 0, 0, 0, 0
				, DEFAULT_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN
				, TEXT("ArcadeClassic"));

			HFONT oldFont = (HFONT)SelectObject(hDC, hFont);

			SetBkMode(hDC, TRANSPARENT);
			SetTextColor(hDC, RGB(255, 255, 255));

			RECT textRect;
			textRect.left = 20;
			textRect.right = CLIENT_WIDTH / 2;
			textRect.top = 0;
			textRect.bottom = CLIENT_HEIGHT;

			// ����� �ؽ�Ʈ �Է�
			DrawText(hDC, L"Game Over!", lstrlen(L"Game Over!"), &textRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			SelectObject(hDC, oldFont);
			DeleteObject(hFont);
		}
	}
}

void GetKeyDown(WPARAM w) {
	switch (w) {
		// Player1
	case 'Z':
		Hero1P.state = STATE::LEFT;
		break;
	case 'X':
		Hero1P.state = STATE::RIGHT;
		break;
	case 'C':
		Hero1P.herojump();
		break;
	case 'V':
		Hero1P.SkillOn(Hero2P);
		break;

		// Player2
	case 'H':
		Hero2P.state = STATE::LEFT;
		break;
	case 'J':
		Hero2P.state = STATE::RIGHT;
		break;
	case 'K':
		Hero2P.herojump();
		break;
	case 'L':
		Hero2P.SkillOn(Hero1P);
		break;
	}
}

void GetKeyUp(WPARAM w) {
	switch (w) {
		// P1ayer1
	case 'Z':
		if(Hero1P.state == STATE::LEFT) Hero1P.state = STATE::NORMAL;
		break;
	case 'X':
		if (Hero1P.state == STATE::RIGHT) Hero1P.state = STATE::NORMAL;
		break;

		// P1ayer2
	case 'H':
		if(Hero2P.state == STATE::LEFT) Hero2P.state = STATE::NORMAL;
		break;
	case 'J':
		if(Hero2P.state == STATE::RIGHT) Hero2P.state = STATE::NORMAL;
		break;
	}
}

void CALLBACK TimerProc(HWND hWnd, UINT iMessage, UINT_PTR idEvent, DWORD dwTime) {

	Global::getInstance()->TimerTick += 1;

	if (board_1.ShakeDegree < 480) board_1.ShakeDegree += 60;

	if (board_2.ShakeDegree < 480) board_2.ShakeDegree += 60;

	if (board_1.isShadowOn) {
		Global::getInstance()->ShadowTick_1 += 0.1f;
		if (Global::getInstance()->ShadowTick_1 > shadowDelay) {
			board_1.effect_shadow();
			Global::getInstance()->ShadowTick_1 = 0;
			board_1.isShadowOn = false;
		}
	}

	if (board_2.isShadowOn) {
		Global::getInstance()->ShadowTick_2 += 0.1f;
		if (Global::getInstance()->ShadowTick_2 > shadowDelay) {
			board_2.effect_shadow();
			Global::getInstance()->ShadowTick_2 = 0;
			board_2.isShadowOn = false;
		}
	}

	if (Global::getInstance()->TimerTick % Global::getInstance()->tick_spawn == 0
		|| Global::getInstance()->Gameover_1) {
		bool FullBoard_1 = true;
		for (int i = 0; i < table_WIDTH; ++i) {
			if (board_1.Val[i][0] != BLOCK_TYPE::STACK && board_1.Val[i][0] != BLOCK_TYPE::SHADOW) {
				FullBoard_1 = false;
				break;
			}
		}
		if (!FullBoard_1) {
			board_1.spawn(board_1.getRandomXPos(), static_cast<BLOCK_TYPE>(getRandom(Block_Type_Count)));
		}
	}
	if (Global::getInstance()->TimerTick % Global::getInstance()->tick_spawn == 0
		|| Global::getInstance()->Gameover_2) {
		bool FullBoard_2 = true;
		for (int i = 0; i < table_WIDTH; ++i) {
			if (board_2.Val[i][0] != BLOCK_TYPE::STACK && board_2.Val[i][0] != BLOCK_TYPE::SHADOW) {
				FullBoard_2 = false;
				break;
			}
		}
		if (!FullBoard_2) {
			board_2.spawn(board_2.getRandomXPos(), static_cast<BLOCK_TYPE>(getRandom(Block_Type_Count)));
		}
	}


	if (Global::getInstance()->TimerTick % tick_block == 0) {
		board_1.drop();
		board_2.drop();
	}

	if (Global::getInstance()->TimerTick % tick_skill == 0) {
		Hero1P.skillGaugeUp(Hero2P);
		Hero2P.skillGaugeUp(Hero1P);
	}
	Hero1P.move(board_1);
	Hero2P.move(board_2);

	InvalidateRect(Stage_1_hWnd, NULL, FALSE);
	InvalidateRect(Stage_2_hWnd, NULL, FALSE);

}

LRESULT CALLBACK Stage_1_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;

	switch (uMsg) {
#pragma region �ʱ�ȭ
	case WM_CREATE:
		doublebuffer_Stage_1.Create(STAGE_WIDTH, STAGE_HEIGHT, 24);
		break;
#pragma endregion

#pragma region ���콺 I/O ���ڴ� lParam HIWORD/LOWORD
	case WM_LBUTTONDOWN:	break;
	case WM_LBUTTONUP:		break;
	case WM_RBUTTONDOWN:	break;
	case WM_RBUTTONUP:		break;
	case WM_MOUSEMOVE:		break;
#pragma endregion

#pragma region Ű���� I/0 ���ڴ� wParam
	case WM_KEYDOWN:	break;
	case WM_KEYUP:		break;
	case WM_CHAR:		break;
#pragma endregion

#pragma region �׸���
	case WM_PAINT:
	{
		HDC hdc = BeginPaint(hWnd, &ps);
		HDC memDC = doublebuffer_Stage_1.GetDC();

		OnDraw_1(memDC);
		doublebuffer_Stage_1.Draw(hdc, sin(board_1.ShakeDegree*Radian) * 10, cos(board_1.ShakeDegree*Radian) * 10);
		doublebuffer_Stage_1.ReleaseDC();
		EndPaint(hWnd, &ps);
	}
	break;
#pragma endregion

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK Stage_2_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	switch (uMsg) {
#pragma region �ʱ�ȭ
	case WM_CREATE:
		doublebuffer_Stage_2.Create(STAGE_WIDTH, STAGE_HEIGHT, 24);
		break;
#pragma endregion

#pragma region ���콺 I/O ���ڴ� lParam HIWORD/LOWORD
	case WM_LBUTTONDOWN:	break;
	case WM_LBUTTONUP:
	{
		if (!Global::getInstance()->isStart)
		{

			POINT M;
			M.x = LOWORD(lParam);
			M.y = HIWORD(lParam);

			ResorceTable::getInstance()->m_Sound.PlayEffect(2);
			//startbtn
			if (M.x > 100 && M.x < 500 && M.y > 126 && M.y < 234) {
				Hero1P = CHero{ Global::getInstance()->Player1HeroKind, 1 };
				Hero2P = CHero{ Global::getInstance()->Player2HeroKind, 2 };
				Global::getInstance()->isStart = true;
				SetTimer(hWnd, timer_Main, tick_Main, TimerProc);
			}

			//rank
			if (M.x > 100 && M.x < 500 && M.y > 306 && M.y < 414) {
				ShellExecute(NULL, TEXT("open"), TEXT("notepad"), TEXT("RANK.txt"), NULL, SW_SHOW);
			}

			//help
			if (M.x > 100 && M.x < 500 && M.y > 486 && M.y < 594) {
				ShellExecute(NULL, TEXT("open"), TEXT("notepad"), TEXT("HELP.txt"), NULL, SW_SHOW);
			}

			//exit
			if (M.x > 100 && M.x < 500 && M.y > 666 && M.y < 774) {
				PostQuitMessage(0);
			}
		}
	}
	break;
	case WM_RBUTTONDOWN:	break;
	case WM_RBUTTONUP:		break;
	case WM_MOUSEMOVE:		break;
#pragma endregion

#pragma region Ű���� I/0 ���ڴ� wParam
	case WM_KEYDOWN:	break;
	case WM_KEYUP:		break;
	case WM_CHAR:		break;
#pragma endregion

#pragma region �׸���
	case WM_PAINT:
	{
		HDC hdc = BeginPaint(hWnd, &ps);
		HDC memDC = doublebuffer_Stage_2.GetDC();

		OnDraw_2(memDC);
		doublebuffer_Stage_2.Draw(hdc, sin(board_2.ShakeDegree*Radian) * 10, cos(board_2.ShakeDegree*Radian) * 10);
		doublebuffer_Stage_2.ReleaseDC();
		EndPaint(hWnd, &ps);
	}
	break;
#pragma endregion

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
