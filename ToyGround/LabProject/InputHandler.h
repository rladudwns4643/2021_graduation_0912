#pragma once

namespace Core
{
	extern HWND g_hMainWnd;

	extern int g_iDisplayWidth;
	extern int g_iDisplayHeight;
}

namespace InputHandler
{
	bool IsKeyDown(int key);
	bool IsKeyUp(int key);
	bool IsOverlap(int key);

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	void OnOffInputStringState(bool onoff);
	bool GetInputStringState();

	void ResetWString();
	void SetWString(WPARAM wParam, bool isUnicode = false);
	wstring GetWString();

	void ShowMouseCursor();
	void ClipCursorToScreen();
}