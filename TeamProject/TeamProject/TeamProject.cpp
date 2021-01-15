#include "stdafx.h"
#include "TeamProject.h"
#include "GameFramework.h"

// 게임프레임워크 생성
CGameFramework gGameFramework;

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TEAMPROJECT, szWindowClass, MAX_LOADSTRING);

    // 윈도우 클래스 등록
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEAMPROJECT));

    MSG msg;
    
    // 메시지 루프
    while (1)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);        // 이 함수 호출 시 WndProc 함수 호출됨
            }
        }
        else
        {
            gGameFramework.FrameAdvance();      // 렌더링
        }
    }
    gGameFramework.OnDestroy();

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;          // 윈도우 프로시져 설정
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEAMPROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    //wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TEAMPROJECT);  // 메뉴 있는 버전
    wcex.lpszMenuName   = NULL;                                 // 메뉴 없는 버전
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
   
   // 윈도우 스타일 설정
   DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;

   // 윈도우 크기 설정
   RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
   AdjustWindowRect(&rc, dwStyle, FALSE);

   // 주 윈도우 생성(가운데에 정해진 크기의 윈도우 생성)
   HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, WindowCenterPosX, WindowCenterPosY,
       rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

   if (!hMainWnd) return FALSE;

   // 주윈도우 핸들값을 프레임워크에 보내줌
   gGameFramework.OnCreate(hInstance, hMainWnd);

   ShowWindow(hMainWnd, nCmdShow);
   UpdateWindow(hMainWnd);

   // _WITH_SWAPCHAIN_FULLSCREEN_STATE 가 정의 되어 있다면 스왑체인 실행
#ifdef _WITH_SWAPCHAIN_FULLSCREEN_STATE
   gGameFramework.ChangeSwapChainState();
#endif

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;
    switch (message)
    {
    case WM_COMMAND:
        wmId     = LOWORD(wParam);
        wmEvent  = HIWORD(wParam);

        // 메뉴의 선택영역을 구문분석합니다.
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 그리기 코드를 추가합니다.
        EndPaint(hWnd, &ps);
        break;

    // 프레임 워크에 처리할 메시지를 보내준다.(추가할 것들 있으면 추가)
    case WM_SIZE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_KEYDOWN:
    case WM_KEYUP:
        gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
        break;

    case WM_DESTROY:
        PostQuitMessage(0); // WM_QUIT 메시지를 생성하여 메시지 큐에 삽입한다.
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
