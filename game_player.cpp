/*============================================================================
Contents   :  [game_player.cpp]

Author     : Chin Qing You
LastUpdate : 2026/10/21
-----------------------------------------------------------------------------

============================================================================*/
#include <DirectXMath.h>

#include "game_player.h"
#include "camera_free.h"
#include "player_camera.h"
#include "input_keyboard.h"
#include "model.h"

#include "game_object.h"

using namespace DirectX;

static XMFLOAT3 g_Position{};
static const float PLAYER_ACCELERATION = 100.0f;
static const float PLAYER_DRAG = 8.0f;
static const float PLAYER_ROTATION_SPEED_PER_SEC = XM_2PI * 5;
static XMFLOAT3 g_Velocity{};
static float g_AccumulatedTime{};

static const float GRAVITY_Y = -200.0f;
static const float JUMP_HEIGHT = 2.0f;
//static const float MAX_FALL_SPEED = 500.0f;
static bool g_IsGrounded = true;

static XMFLOAT3 g_FrontVector{};
static XMFLOAT3 g_RightVector{};

static XMFLOAT3 g_TargetDirection{};

// player y axis rotation
static float g_Angle{};

static MODEL* g_pModel{ nullptr };

static CollisionAABB MakePlayerAABB(FXMVECTOR position);

void GamePlayer_Initialize(const DirectX::XMFLOAT3& position, float angle_y)
{
	g_pModel = ModelLoad("assets/models/kirby4.fbx", 0.1f, false);
	g_Position = position;
	g_Velocity = { 0.0f, 0.0f, 0.0f };
	g_TargetDirection = { 1.0f, 0.0f, 0.0f };
	g_IsGrounded = true;

	g_Angle = angle_y;
	
	XMVECTOR front =XMVector3TransformNormal({1.0f, 0.0f, 0.0f}, XMMatrixRotationY(g_Angle));
	XMVECTOR right = XMVector3Cross({0.0f, 1.0f, 0.0f}, front);

	XMStoreFloat3(&g_FrontVector, XMVector3Normalize(front));
	XMStoreFloat3(&g_RightVector, XMVector3Normalize(right));
}

void GamePlayer_Finalize()
{
	ModelRelease(g_pModel);
}

void GamePlayer_Update(float delta_time)
{
    XMVECTOR direction = XMVectorZero();
    XMVECTOR velocity = XMLoadFloat3(&g_Velocity);
    XMVECTOR position = XMLoadFloat3(&g_Position);

    // --- Input ---
    if (InputKeyboard_IsPress(KK_W)) { direction += PlayerCamera_GetFrontVector(); }
    if (InputKeyboard_IsPress(KK_S)) { direction -= PlayerCamera_GetFrontVector(); }
    if (InputKeyboard_IsPress(KK_D)) { direction += PlayerCamera_GetRightVector(); }
    if (InputKeyboard_IsPress(KK_A)) { direction -= PlayerCamera_GetRightVector(); }

    // --- Jump & gravity ---
    if (InputKeyboard_IsTrigger(KK_SPACE) && g_IsGrounded)
    {
        float jumpSpeed = sqrtf(2.0f * -GRAVITY_Y * JUMP_HEIGHT);
        velocity = XMVectorSetY(velocity, jumpSpeed);
        g_IsGrounded = false;
    }
    velocity = XMVectorSetY(velocity, XMVectorGetY(velocity) + GRAVITY_Y * delta_time);

    // --- Horizontal acceleration ---
    direction = XMVectorSetY(direction, 0.0f);
    if (XMVectorGetX(XMVector3LengthSq(direction)) > 1e-6f)
    {
        direction = XMVector3Normalize(direction);
        XMStoreFloat3(&g_TargetDirection, direction);
        velocity += direction * PLAYER_ACCELERATION * delta_time;
    }

    // --- Rotation toward target direction ---
    float targetAngle = atan2f(-g_TargetDirection.z, g_TargetDirection.x);
    float diff = XMScalarModAngle(targetAngle - g_Angle);
    float maxStep = PLAYER_ROTATION_SPEED_PER_SEC * delta_time;

    if (std::abs(diff) <= maxStep) 
    {
        g_Angle = targetAngle;
    }
    else    
    {
        g_Angle += (diff > 0.0f) ? maxStep : -maxStep;
    }
    g_Angle = XMScalarModAngle(g_Angle);

    XMVECTOR front = XMVector3Normalize(XMVector3TransformNormal({ 1.0f, 0.0f, 0.0f }, XMMatrixRotationY(g_Angle)));
    XMVECTOR right = XMVector3Normalize(XMVector3Cross({ 0.0f, 1.0f, 0.0f }, front));
    XMStoreFloat3(&g_FrontVector, front);
    XMStoreFloat3(&g_RightVector, right);
    XMStoreFloat3(&g_TargetDirection, XMVector3Normalize(front));

    // --- Drag (horizontal only) ---
    velocity -= velocity * XMVECTOR{ 1.0f, 0.0f, 1.0f, 0.0f } * PLAYER_DRAG * delta_time;

    // --- Integrate position ---
    position += velocity * delta_time;

    // --- Object collision ---
    g_IsGrounded = false;

    for (int i = 0; i < OBJECT_MAX; i++)
    {
        const GameObject* object = GameObject_Get(i);
        if (object == nullptr) continue;

        // Rebuild every iteration: earlier objects may have moved us
        HitData3D hit = MakePlayerAABB(position).IsHit(GameObject_GetAABB(*object));
        if (!hit.IsHit()) continue;

        XMVECTOR normal = XMLoadFloat3(&hit.GetNormal());

        // Push out of the object
        position += normal * hit.GetDepth();

        // Remove only the velocity going into the surface (keeps sliding along walls)
        float intoSurface = XMVectorGetX(XMVector3Dot(velocity, normal));
        if (intoSurface < 0.0f)
        {
            velocity -= normal * intoSurface;
        }

        if (hit.GetNormal().y > 0.0f)
        {
            g_IsGrounded = true;
        }
    }

    // --- Ground collision ---
    if (XMVectorGetY(position) < 0.0f)
    {
        position = XMVectorSetY(position, 0.0f);
        velocity = XMVectorSetY(velocity, 0.0f);
        g_IsGrounded = true;
    }

    XMStoreFloat3(&g_Position, position);
    XMStoreFloat3(&g_Velocity, velocity);
}

void GamePlayer_Draw()
{
	XMMATRIX t = XMMatrixTranslationFromVector(XMLoadFloat3(&g_Position));

	XMMATRIX r = XMMatrixRotationY(g_Angle - XM_PIDIV2);

	ModelDraw(g_pModel, r * t);
}

const DirectX::XMFLOAT3& GamePlayer_GetPosition()
{
	return g_Position;
}

static CollisionAABB MakePlayerAABB(FXMVECTOR position)
{
    if (!g_pModel) return {};

    CollisionAABB aabb = g_pModel->AABB;
    const XMFLOAT3& local = aabb.GetCenter();

    XMFLOAT3 p;
    XMStoreFloat3(&p, position);
    aabb.SetCenterPosition({ p.x + local.x, p.y + local.y, p.z + local.z });
    return aabb;
}

CollisionAABB GamePlayer_GetAABB()
{
    return MakePlayerAABB(XMLoadFloat3(&g_Position));
}


