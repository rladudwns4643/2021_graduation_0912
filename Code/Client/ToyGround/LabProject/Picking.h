#pragma once
#include "Camera.h"

namespace Core
{
	extern int g_DisplayWidth;
	extern int g_DisplayHeight;
}

struct PickedObject
{
	float distance = 0.f;
	string objectName;
	string instName;
	int instanceID;	

	UINT materialIndex;

	int zLayer = -1;
};

namespace Picking
{
	PickedObject RayIntersect2D(int screenX, int screenY);
	PickedObject RayIntersect2D(int screenX, int screenY, std::string sceneName, std::string layer = OBJECT_TYPE_UI2D);
	PickedObject RayIntersect2DZLayer(int screenX, int screenY, std::string sceneName, std::string layer = OBJECT_TYPE_UI2D);

	void RayAtViewSpace(XMVECTOR& original, XMVECTOR& direction, int screenX, int screenY);
}
