/*============================================================================
Contents   :  [camera_free.h]

Author     : Chin Qing You
LastUpdate : 2026/09/18
-----------------------------------------------------------------------------

============================================================================*/
#include "camera_free.h"
#include "config.h";

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
	XMVECTOR up = XMLoadFloat3(&g_Up);

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

// camera_free.cpp
XMMATRIX CameraFree_GetViewMatrix()
{
	return XMLoadFloat4x4(&g_View);
}

XMMATRIX CameraFree_GetProjectionMatrix()
{
	return XMLoadFloat4x4(&g_Projection);
}
