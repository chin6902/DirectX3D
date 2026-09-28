/*============================================================================
Contents   :  [camera_free.h]

Author     : Chin Qing You
LastUpdate : 2026/09/18
-----------------------------------------------------------------------------

============================================================================*/
#include <algorithm>

#include "camera_free.h"
#include "config.h"
#include "input_keyboard.h"

#ifdef _DEBUG
#include "debug_text.h"
#include "debug_ostream.h"
#include "direct3d.h"
#include "imgui.h"
#endif

using namespace DirectX;

static XMFLOAT4X4 g_View{};
static XMFLOAT4X4 g_Projection{};

// Camera orientation vectors
static XMFLOAT3 g_Front{};
static XMFLOAT3 g_Right{};
static XMFLOAT3 g_Up{};

static float g_Fov{ 1.0f };

// Camera position
static XMFLOAT3 g_Position{};

void CameraFree_Initialize(const XMFLOAT3& position, float angle_x, float angle_y)
{
	g_Position = position;
	g_Fov = 60.0f;

	g_Front = { 0.0f, 0.0f, 1.0f };
	g_Right = { 1.0f, 0.0f, 0.0f };
	g_Up = { 0.0f, 1.0f, 0.0f };
	XMVECTOR front = XMLoadFloat3(&g_Front);
	XMVECTOR right = XMLoadFloat3(&g_Right);
	XMVECTOR up = XMLoadFloat3(&g_Up);

	// front vector and up vector rotation around the x_angle
	XMMATRIX rotationX = XMMatrixRotationX(angle_x);
	front =  XMVector3TransformNormal(front, rotationX);
	up = XMVector3TransformNormal(up, rotationX);

	// front vector and up vector rotation around the x_angle
	XMMATRIX rotationY = XMMatrixRotationY(angle_y);
	front =  XMVector3TransformNormal(front, rotationY);
	up = XMVector3TransformNormal(up, rotationY);

	// front vector and up vector cross product to get the right vector
	right = XMVector3Cross(up, front);

	// Normalize the vectors
	front = XMVector3Normalize(front);
	right = XMVector3Normalize(right);
	up = XMVector3Normalize(up);

	// XMFLOAT3 to XMVECTOR
	XMStoreFloat3(&g_Front, front);
	XMStoreFloat3(&g_Right, right);
	XMStoreFloat3(&g_Up, up);
}

void CameraFree_Finalize()
{

}

void CameraFree_Update(float delta_time)
{
	// XMFLOAT3 to XMVECTOR
	XMVECTOR position = XMLoadFloat3(&g_Position);
	XMVECTOR front = XMLoadFloat3(&g_Front);
	XMVECTOR right = XMLoadFloat3(&g_Right);
	XMVECTOR up = XMLoadFloat3(&g_Up);

	constexpr float MOVE_SPEED_PER_SECOND = 5.0f;
	constexpr float ROTATION_SPEED_PER_SECOND = XM_PI;
	constexpr float ZOOM_SPEED_PER_SECOND = 30.0f;

	if (InputKeyboard_IsPress(KK_W))
	{
		XMVECTOR direction{ XMVector3Normalize(front * XMVECTOR{1.0f,0.0f,1.0f,1.0f})};
		position += direction * delta_time * MOVE_SPEED_PER_SECOND;
	}
	if (InputKeyboard_IsPress(KK_S))
	{
		XMVECTOR direction{ XMVector3Normalize(front * XMVECTOR{1.0f,0.0f,1.0f,1.0f})};
		position -= direction * delta_time * MOVE_SPEED_PER_SECOND;
	}
	if (InputKeyboard_IsPress(KK_D))
	{
		position += right * delta_time * MOVE_SPEED_PER_SECOND;
	}
	if (InputKeyboard_IsPress(KK_A))
	{
		position -= right * delta_time * MOVE_SPEED_PER_SECOND;
	}
	if (InputKeyboard_IsPress(KK_Q))
	{
		XMVECTOR direction{ XMVector3Normalize(up * XMVECTOR{0.0f,1.0f,1.0f,1.0f}) };
		position += up * delta_time * MOVE_SPEED_PER_SECOND;
	}
	if (InputKeyboard_IsPress(KK_E))
	{
		XMVECTOR direction{ XMVector3Normalize(up * XMVECTOR{0.0f,1.0f,1.0f,1.0f}) };
		position -= up * delta_time * MOVE_SPEED_PER_SECOND;
	}
	if (InputKeyboard_IsPress(KK_RIGHT))
	{
		XMMATRIX rotation_y = XMMatrixRotationY(ROTATION_SPEED_PER_SECOND * delta_time);
		front = XMVector3Normalize(XMVector3TransformNormal(front, rotation_y));
		up = XMVector3Normalize(XMVector3TransformNormal(up, rotation_y));
		right = XMVector3Normalize(XMVector3Cross(up, front));
	}
	if (InputKeyboard_IsPress(KK_LEFT))
	{
		XMMATRIX rotation_y = XMMatrixRotationY(-ROTATION_SPEED_PER_SECOND * delta_time);
		front = XMVector3Normalize(XMVector3TransformNormal(front, rotation_y));
		up = XMVector3Normalize(XMVector3TransformNormal(up, rotation_y));
		right = XMVector3Normalize(XMVector3Cross(up, front));
	}
	if (InputKeyboard_IsPress(KK_UP))
	{
		XMMATRIX rotation_right = XMMatrixRotationAxis(right, -ROTATION_SPEED_PER_SECOND * delta_time);
		front = XMVector3Normalize(XMVector3TransformNormal(front, rotation_right));
		up = XMVector3Normalize(XMVector3TransformNormal(up, rotation_right));
	}
	if (InputKeyboard_IsPress(KK_DOWN))
	{
		XMMATRIX rotation_right = XMMatrixRotationAxis(right, ROTATION_SPEED_PER_SECOND * delta_time);
		front = XMVector3Normalize(XMVector3TransformNormal(front, rotation_right));
		up = XMVector3Normalize(XMVector3TransformNormal(up, rotation_right));
	}
	if (InputKeyboard_IsPress(KK_T))
	{
		g_Fov -= ZOOM_SPEED_PER_SECOND * delta_time;
	}
	if (InputKeyboard_IsPress(KK_Y))
	{
		g_Fov += ZOOM_SPEED_PER_SECOND * delta_time;
	}

	g_Fov = std::clamp(g_Fov, 1.0f, 120.0f);

	XMStoreFloat3(&g_Position, position);
	XMStoreFloat3(&g_Right, right);
	XMStoreFloat3(&g_Up, up);
	XMStoreFloat3(&g_Front, front);

	// View matrix calculation using LookToLH
	XMMATRIX view = XMMatrixLookToLH(
		position,
		front,
		up
	);

	// Projection matrix calculation using PerspectiveFovLH
	XMMATRIX perspective = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(g_Fov), // Field of view
		static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), // Aspect ratio
		0.01f, 1000.0f // Near and far planes
	);

	// Store the matrices in XMFLOAT4X4
	XMStoreFloat4x4(&g_View, view);
	XMStoreFloat4x4(&g_Projection, perspective);
}

void CameraFree_DrawDebugUI()
{
#ifdef _DEBUG
	ImGui::Begin("Camera");

	ImGui::Text("Position  %.2f, %.2f, %.2f", g_Position.x, g_Position.y, g_Position.z);
	ImGui::Text("Front     %.2f, %.2f, %.2f", g_Front.x, g_Front.y, g_Front.z);
	ImGui::DragFloat("FOV", &g_Fov, 0.5f, 1.0f, 120.0f);

	ImGui::End();
#endif
}

// camera_free.cpp
XMMATRIX CameraFree_GetViewMatrix()
{
	return XMLoadFloat4x4(&g_View);
}

XMMATRIX CameraFree_GetProjectionMatrix()
{
	return XMLoadFloat4x4(&g_Projection);
}
