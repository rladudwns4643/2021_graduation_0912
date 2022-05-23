#include "pch.h"
#include "InputHandler.h"

namespace InputHandler
{
	static bool	s_CursorState = true;
	static bool s_InputStringState = false;
	bool g_CursorSwitch = true;

	static bool	s_KeyFlag[256];
	static wstring s_WGlobalString;

	// ���� �Ⱦ��̴��� ������ �κ���̳�, �ٸ������� ���ϼ�������
	POINT	g_StartMousePos = { 0.f,0.f };
	POINT	g_LastMousePos = { 0.f,0.f };
	float	g_MouseChangebleX = 0.f;
	float	g_MouseChangebleY = 0.f;

	bool	g_MoveMouseCallback = false;

	bool	g_LeftMouseCallback = false;
	bool	g_LeftMouseOverlap = false;
	bool	g_RightMouseCallback = false;
	bool	g_RightMouseOverlap = false;

	bool	g_PreviousStateLeftMouse = false;
	bool	g_PreviousStateRightMouse = false;
	bool	g_LeftMouseClick = false;
	bool	g_RightMouseClick = false;

	float	g_MouseClickTime = 0.f;

	bool IsKeyDown(int key)
	{
		if (GetAsyncKeyState(key) & 0x8000) {
			if (!s_KeyFlag[key])
				return s_KeyFlag[key] = true;
		}
		return false;
	}

	bool IsKeyUp(int key)
	{
		if (GetAsyncKeyState(key) & 0x8000)
			s_KeyFlag[key] = true;
		else
			if (s_KeyFlag[key])
				return !(s_KeyFlag[key] = false);
		return false;
	}

	bool IsOverlap(int key)
	{
		return (GetAsyncKeyState(key) & 0x8000);
	}

	void OnMouseDown(WPARAM btnState, int x, int y)
	{
		//ClipCursorToScreen();

		g_StartMousePos.x = x;
		g_StartMousePos.y = y;

		SetCapture(Core::g_hMainWnd);

		if ((btnState & MK_LBUTTON) != 0)
		{
			g_LeftMouseCallback = true;
			g_LeftMouseOverlap = true;
			g_PreviousStateLeftMouse = true;
		}
		if ((btnState & MK_RBUTTON) != 0)
		{
			g_RightMouseCallback = true;
			g_RightMouseOverlap = true;
			g_PreviousStateRightMouse = true;
		}
	}

	void OnMouseUp(WPARAM btnState, int x, int y)
	{
		ReleaseCapture();

		g_LastMousePos.x = x;
		g_LastMousePos.y = y;

		if ((btnState & MK_LBUTTON) == 0)
		{
			g_LeftMouseCallback = false;
			g_LeftMouseOverlap = false;
			if (g_LeftMouseCallback != g_PreviousStateLeftMouse)
			{
				g_PreviousStateLeftMouse = g_LeftMouseCallback;
				g_LeftMouseClick = true;
			}
		}
		if ((btnState & MK_RBUTTON) == 0)
		{
			g_RightMouseCallback = false;
			g_RightMouseOverlap = false;
			if (g_RightMouseCallback != g_PreviousStateRightMouse)
			{
				g_PreviousStateRightMouse = g_RightMouseCallback;
				g_RightMouseClick = true;
			}
		}

		g_MouseClickTime = 0.f;
	}

	void OnMouseMove(WPARAM btnState, int x, int y)
	{
		// Player Controller
		if (x != Core::g_DisplayWidth / 2 &&
			y != Core::g_DisplayHeight / 2)
		{
			g_MoveMouseCallback = true;
		}


		// ���콺 ��ǥ�̵��� ���� ȸ������ ���ϱ�
		if (g_MoveMouseCallback)
		{
			float dx = XMConvertToRadians(MOUSE_DPI_X * static_cast<float>(x - Core::g_DisplayWidth / 2));
			float dy = XMConvertToRadians(MOUSE_DPI_Y * static_cast<float>(y - Core::g_DisplayHeight / 2));
			g_MouseChangebleX = dx;
			g_MouseChangebleY = dy;
		}
	}

	void OnOffInputStringState(bool onoff)
	{
		s_InputStringState = onoff;
	}

	bool GetInputStringState()
	{
		return s_InputStringState;
	}

	void ResetWString()
	{
		s_WGlobalString.clear();
	}

	void SetWString(WPARAM wParam, bool isUnicode)
	{
		if (!s_InputStringState) return;

		if (!isUnicode)
		{
			if (wParam == VK_BACK && !s_WGlobalString.empty())
			{
				s_WGlobalString.pop_back();
			}
			else
			{
				s_WGlobalString.push_back(wParam);
			}
		}
	}

	wstring GetWString()
	{
		return s_WGlobalString;
	}

	void ShowMouseCursor()
	{
		if (s_CursorState == g_CursorSwitch)
			return;

		s_CursorState = g_CursorSwitch;
		ShowCursor(s_CursorState);
	}

	void ClipCursorToScreen()
	{
		int sideInteger = 2;
		RECT winRect;
		GetClientRect(Core::g_hMainWnd, &winRect);
		ClientToScreen(Core::g_hMainWnd, (LPPOINT)&winRect.left);
		ClientToScreen(Core::g_hMainWnd, (LPPOINT)&winRect.right);
		winRect.left += sideInteger;
		winRect.right -= sideInteger;
		winRect.top += sideInteger;
		winRect.bottom -= sideInteger;
		ClipCursor(&winRect);
	}

	// Ŭ�� �� ������ ��������� ��
	void ResetClickState()
	{
		g_LeftMouseClick = false;
		g_RightMouseClick = false;
	}
}