/*============================================================================
Contents   :  [game_enemy.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/26
-----------------------------------------------------------------------------

============================================================================*/
#include "game_enemy.h"
#include "game_map.h"
#include "game_object.h"

using namespace DirectX;

static constexpr float ENEMY_GRAVITY_Y = -200.0f;

void GameEnemy::TakeDamage(int damage)
{
	if (m_HP <= 0) { return; }

	m_HP -= damage;

	if (m_HP <= 0)
	{
		m_IsDestroy = true;
		OnDeath();
	}
}

void GameEnemy::MoveWithCollision(float delta_time)
{
	m_Velocity.y += ENEMY_GRAVITY_Y * delta_time;   

	XMFLOAT3 pos = m_Position;                      

	pos.x += m_Velocity.x * delta_time;            
	if (HitMap(pos)) { pos.x = m_Position.x; m_Velocity.x = 0.0f; }   

	pos.z += m_Velocity.z * delta_time;            
	if (HitMap(pos)) { pos.z = m_Position.z; m_Velocity.z = 0.0f; }   

	m_IsGrounded = false;                           
	pos.y += m_Velocity.y * delta_time;             
	if (HitMap(pos))
	{
		if (m_Velocity.y < 0.0f) { m_IsGrounded = true; }   
		pos.y = m_Position.y;
		m_Velocity.y = 0.0f;
	}

	if (pos.y < 0.0f)                               
	{
		pos.y = 0.0f;
		m_Velocity.y = 0.0f;
		m_IsGrounded = true;
	}

	m_Position = pos;                               
}

bool GameEnemy::HitMap(const XMFLOAT3& position) const
{
	return GameMap_CheckCollision(GetAABBAt(position)).IsHit();
}
