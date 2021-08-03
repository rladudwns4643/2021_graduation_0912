#pragma once

namespace InputHandler
{
	extern POINT	g_StartMousePos;
	extern POINT	g_LastMousePos;

	extern float	g_MouseChangebleX;
	extern float	g_MouseChangebleY;

	extern bool		g_MoveMouseCallback;

	extern bool		g_LeftMouseCallback;
	extern bool		g_LeftMouseOverlap;

	extern bool		g_RightMouseCallback;
	extern bool		g_RightMouseOverlap;

	extern float	g_MouseClickTime;

	extern bool	g_LeftMouseClick;
	extern bool g_RightMouseClick;
}

class Controller abstract
{
public:
	virtual void Update(const float deltaT) = 0;

protected:
	virtual void HandleInput(const float deltaT) = 0;
	virtual void MouseCallback() = 0;

	virtual void OnKeyPressed() {};
	virtual void OnKeyReleased() {};

public:
	void SetMapName(std::string mapName) { m_MapName = mapName; }

protected:
	std::string m_MapName;
};