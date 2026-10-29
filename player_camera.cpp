/*============================================================================
Contents   :  [player_camera.h]

Author     : Chin Qing You
LastUpdate : 2026/10/27
-----------------------------------------------------------------------------

============================================================================*/
#include <algorithm>

#include "player_camera.h"
#include "config.h"
#include "input_keyboard.h"

using namespace DirectX;

static XMFLOAT4X4 g_View{};
static XMFLOAT4X4 g_Projection{};

static XMFLOAT3 g_Front{ 0.0f, 0.0f, 1.0f };
static XMFLOAT3 g_Right{ 1.0f, 0.0f, 0.0f };
static XMFLOAT3 g_Up{ 0.0f, 1.0f, 0.0f };

static float g_Fov{ 60.0f };

static XMFLOAT3 g_Position{};
static XMFLOAT3 g_TargetPosition{};

static float g_Length{};

static float g_AngleX{};
static float g_AngleY{};

void PlayerCamera_Initialize(const DirectX::XMFLOAT3& target_position, float length, float angle_x, float angle_y)
{
	g_Length = length;
	g_AngleX = angle_x;
	g_AngleY = angle_y;

	PlayerCamera_SetTargetPosition(target_position);

	g_Fov = 60.0f;

	XMVECTOR front = { 0.0f, 0.0f, 1.0f };
	XMVECTOR right = { 1.0f, 0.0f, 0.0f };
	XMVECTOR up = { 0.0f, 1.0f, 0.0f };

	// front vector and up vector rotation around the x_angle
	XMMATRIX rotationX = XMMatrixRotationX(angle_x);
	front = XMVector3TransformNormal(front, rotationX);
	up = XMVector3TransformNormal(up, rotationX);

	// front vector and up vector rotation around the y_angle
	XMMATRIX rotationY = XMMatrixRotationY(angle_y);
	front = XMVector3TransformNormal(front, rotationY);
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

void PlayerCamera_Finalize()
{

}

void PlayerCamera_Update(float delta_time)
{
	XMVECTOR front = { 0.0f, 0.0f, 1.0f };
	XMVECTOR right = { 1.0f, 0.0f, 0.0f };


	// front vector and up vector rotation around the y_angle
	XMMATRIX rotationY = XMMatrixRotationY(g_AngleY);
	front = XMVector3TransformNormal(front, rotationY);

	// front vector and up vector cross product to get the right vector
	right = XMVector3Cross(XMVECTOR{0.0f, 1.0f, 0.0f}, front);

	// Normalize the vectors
	front = XMVector3Normalize(front);
	right = XMVector3Normalize(right);

	// XMFLOAT3 to XMVECTOR
	XMStoreFloat3(&g_Front, front);
	XMStoreFloat3(&g_Right, right);

	XMVECTOR position = XMLoadFloat3(&g_Position);
	XMVECTOR target = XMLoadFloat3(&g_TargetPosition);
	XMVECTOR up = { 0.0f, 1.0f, 0.0f };

	// View matrix calculation using LookToLH
	XMMATRIX view = XMMatrixLookAtLH(
		position,
		target,
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

void PlayerCamera_SetTargetPosition(const DirectX::XMFLOAT3& target_position)
{
	g_TargetPosition = target_position;

	XMVECTOR position = { 0.0f, 0.0f, -g_Length };
	XMMATRIX rx = XMMatrixRotationX(g_AngleX);
	XMMATRIX ry = XMMatrixRotationY(g_AngleY);

	// when jump disable y position and fix it to 0.5f
	g_TargetPosition.y = 0.5f;

	XMMATRIX t = XMMatrixTranslationFromVector(XMLoadFloat3(&g_TargetPosition));

	position = XMVector3TransformCoord(position, rx * ry * t);

	XMStoreFloat3(&g_Position, position);
}

void PlayerCamera_Orbit(float rotation_angle)
{
	g_AngleY += rotation_angle;
}

DirectX::XMMATRIX PlayerCamera_GetViewMatrix()
{
	return XMLoadFloat4x4(&g_View);
}

DirectX::XMMATRIX PlayerCamera_GetProjectionMatrix()
{
	return XMLoadFloat4x4(&g_Projection);
}

DirectX::XMVECTOR PlayerCamera_GetFrontVector()
{
	return XMLoadFloat3(&g_Front);
}

DirectX::XMVECTOR PlayerCamera_GetRightVector()
{
	return XMLoadFloat3(&g_Right);
}
