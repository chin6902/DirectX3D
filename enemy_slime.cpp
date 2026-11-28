/*============================================================================
Contents   :  [enemy_slime.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/28
-----------------------------------------------------------------------------

============================================================================*/
#include <algorithm>
#include "enemy_slime.h"
#include "model.h"
#include "game_explosion.h"

#ifdef _DEBUG
#include "collision3d_debug.h"
#endif

using namespace DirectX;

static constexpr int   SLIME_HP = 3;
static constexpr float SLIME_MODEL_SCALE = 1.0f;  

static MODEL* s_pModel{ nullptr };

EnemySlime::EnemySlime(const DirectX::XMFLOAT3& position) : GameEnemy(position, SLIME_HP) {}

bool EnemySlime::LoadResources()
{
    s_pModel = ModelLoad("assets/models/slime.fbx", SLIME_MODEL_SCALE, false);
    return s_pModel != nullptr;
}

void EnemySlime::UnloadResources()
{
    if(s_pModel)
    {
        ModelRelease(s_pModel);
        s_pModel = nullptr;
    }
}

void EnemySlime::Update(float delta_time)
{
    MoveWithCollision(delta_time);
}

void EnemySlime::Draw() const
{
    if (!s_pModel) return;

    const CollisionAABB& aabb = s_pModel->AABB;
    const XMFLOAT3& c = aabb.GetCenter();

    XMMATRIX w =
        XMMatrixTranslation(-c.x, aabb.GetHalfHeight() - c.y, -c.z) *
        XMMatrixRotationY(m_Angle) *
        XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

    ModelDraw(s_pModel, w);

#ifdef _DEBUG
    Collision3D_Debug_Add(GetAABB());
#endif
}

CollisionAABB EnemySlime::GetAABBAt(const XMFLOAT3& position) const
{
    if (!s_pModel) return { 0.5f, 0.5f, 0.5f, position };

    const CollisionAABB& aabb = s_pModel->AABB;
    const float half_xz = std::min(aabb.GetHalfWidth(), aabb.GetHalfDepth());

    // bottom of the box at the feet, so the center is half a height above them
    XMFLOAT3 center = position;
    center.y += aabb.GetHalfHeight();

    return CollisionAABB(half_xz, aabb.GetHalfHeight(), half_xz, center);
}

void EnemySlime::OnDeath()
{
    GameExplosion_Spawn(GetAABB().GetCenter());
}
