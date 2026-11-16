/*============================================================================
Contents   :  [game_player.cpp]

Author     : Chin Qing You
LastUpdate : 2026/10/21
-----------------------------------------------------------------------------

============================================================================*/
#include <algorithm>
#include <DirectXMath.h>

#include "game_player.h"
#include "camera_free.h"
#include "player_camera.h"
#include "input_keyboard.h"
#include "model.h"
#include "collision3d_debug.h"

#include "game_map.h"
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
static void ResolveHit(XMVECTOR& position, XMVECTOR& velocity, const HitData3D& hit);
static bool HitMap(const XMFLOAT3& p);

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
    HitData3D push_hit = GameMap_CheckCollision(MakePlayerAABB(XMLoadFloat3(&g_Position)));

    if (g_IsGrounded)
    {
        XMFLOAT3 feet = g_Position;
        feet.y -= 0.1f;

        const XMFLOAT3 move = GameMap_GetFloorMove(MakePlayerAABB(XMLoadFloat3(&feet)));

        XMFLOAT3 carried = { g_Position.x + move.x, g_Position.y + move.y, g_Position.z + move.z };
        if (!HitMap(carried))   // don't carry the player into a wall
        {
            g_Position = carried;
        }
    }

    if (push_hit.IsHit())
    {
        XMVECTOR p = XMLoadFloat3(&g_Position);
        p += XMLoadFloat3(&push_hit.GetNormal()) * (push_hit.GetDepth() + 0.001f);
        XMStoreFloat3(&g_Position, p);
    }

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

    // --- Integrate position (one axis at a time against the map) ---
    g_IsGrounded = false;   // moved up from the object collision section

    XMFLOAT3 pos = g_Position;
    XMFLOAT3 vel;
    XMStoreFloat3(&vel, velocity);

    pos.x += vel.x * delta_time;
    if (HitMap(pos)) { pos.x = g_Position.x; vel.x = 0.0f; }

    pos.z += vel.z * delta_time;
    if (HitMap(pos)) { pos.z = g_Position.z; vel.z = 0.0f; }

    pos.y += vel.y * delta_time;
    if (HitMap(pos))
    {
        if (vel.y < 0.0f) g_IsGrounded = true;  
        pos.y = g_Position.y;
        vel.y = 0.0f;
    }

    position = XMLoadFloat3(&pos);
    velocity = XMLoadFloat3(&vel);

    for (int i = 0; i < OBJECT_MAX; i++)
    {
        const GameObject* object = GameObject_Get(i);
        if (object == nullptr) continue;

        HitData3D hit = MakePlayerAABB(position).IsHit(GameObject_GetAABB(*object));
        if (hit.IsHit()) ResolveHit(position, velocity, hit);
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

static XMMATRIX GetPlayerRotation()
{
    return XMMatrixRotationY(g_Angle - XM_PIDIV2);
}

void GamePlayer_Draw()
{
    XMMATRIX t = XMMatrixTranslationFromVector(XMLoadFloat3(&g_Position));
    ModelDraw(g_pModel, GetPlayerRotation() * t);

#ifdef _DEBUG
    Collision3D_Debug_Add(GamePlayer_GetAABB());
#endif
}

const DirectX::XMFLOAT3& GamePlayer_GetPosition()
{
	return g_Position;
}

static CollisionAABB MakePlayerAABB(FXMVECTOR position)
{
    if (!g_pModel) return {};

    const CollisionAABB& model_aabb = g_pModel->AABB;

    // Square footprint so the box is the same at every facing.
    // Uses the smaller side (the body), so the arms don't count.
    const float half_xz = std::min(model_aabb.GetHalfWidth(), model_aabb.GetHalfDepth());

    // Vertical offset only, so turning never moves the box
    XMFLOAT3 center;
    XMStoreFloat3(&center, position);
    center.y += model_aabb.GetCenter().y;

    return CollisionAABB(half_xz, model_aabb.GetHalfHeight(), half_xz, center);
}

CollisionAABB GamePlayer_GetAABB()
{
    return MakePlayerAABB(XMLoadFloat3(&g_Position));
}

static void ResolveHit(XMVECTOR& position, XMVECTOR& velocity, const HitData3D& hit)
{
    XMVECTOR normal = XMLoadFloat3(&hit.GetNormal());

    // Push out
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

bool HitMap(const XMFLOAT3& p)
{
    return GameMap_CheckCollision(MakePlayerAABB(XMLoadFloat3(&p))).IsHit();
}

