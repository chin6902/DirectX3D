/*============================================================================
Contents   :  [game_enemy.h]
              
Author     : Chin Qing You
LastUpdate : 2026/11/26
-----------------------------------------------------------------------------

============================================================================*/
#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include <DirectXMath.h>
#include "collision.h"

class GameEnemy
{
public:
	GameEnemy(const DirectX::XMFLOAT3& position, int hp) : m_Position(position), m_HP(hp) {}
	virtual ~GameEnemy() = default;

	GameEnemy(const GameEnemy&) = delete;
	GameEnemy& operator=(const GameEnemy&) = delete;

	virtual void Update(float delta_time) = 0;
	virtual void Draw() const = 0;
	virtual CollisionAABB GetAABBAt(const DirectX::XMFLOAT3& position) const = 0;

	virtual void TakeDamage(int damage);

	CollisionAABB GetAABB() const { return GetAABBAt(m_Position); }
	const DirectX::XMFLOAT3& GetPosition() const { return m_Position; }
	int  GetHP() const { return m_HP; }
	void Destroy() { m_IsDestroy = true; }
	bool IsDestroy() const { return m_IsDestroy; }

protected:
	virtual void OnDeath() {}
	void MoveWithCollision(float delta_time);

	DirectX::XMFLOAT3 m_Position{};
	DirectX::XMFLOAT3 m_Velocity{};
	int  m_HP{};
	bool m_IsGrounded{ false };

private:
	bool HitMap(const DirectX::XMFLOAT3& position) const;

	bool m_IsDestroy{ false };
};

#endif