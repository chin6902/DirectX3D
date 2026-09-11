/*============================================================================
Contents   :  [camera.cpp]

Author     : Chin Qing You
LastUpdate : 2026/07/21
-----------------------------------------------------------------------------

============================================================================*/
#include <cmath>
#include <algorithm>

#include "camera.h"
#include "config.h"
#include "vector2.h"

static Vector2 g_Camera;
static constexpr float CAMERA_LERP_SPEED = 8.0f;

static float g_CameraX = 0.0f;
static float g_CameraY = 0.0f;

static float g_ShakeStrength = 0.0f;
static float g_ShakeTimer = 0.0f;
static float g_ShakeMax = 0.0f;

void Camera_Initialize()
{
}

void Camera_Update(float delta_time)
{
}

void Camera_Shake(float strength, float duration)
{
	if (strength <= g_ShakeStrength && g_ShakeTimer > 0.0f) { return; }

	g_ShakeStrength = strength;
	g_ShakeTimer = duration;
	g_ShakeMax = duration;
}

float Camera_GetX()
{
	return g_Camera.x;
}

float Camera_GetY()
{
	return g_Camera.y;
}

float Camera_WorldToScreenX(float world_x)
{
	return world_x - g_Camera.x;
}

float Camera_WorldToScreenY(float world_y)
{
	return world_y - g_Camera.y;
}

float Camera_ScreenToWorldX(float screen_x)
{ 
	return screen_x + g_Camera.x; 
}

float Camera_ScreenToWorldY(float screen_y)
{
	return screen_y + g_Camera.y; 
}
