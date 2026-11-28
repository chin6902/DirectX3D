/*============================================================================
Contents   :  [enemy_slime.h]
              
Author     : Chin Qing You
LastUpdate : 2026/11/28
-----------------------------------------------------------------------------

============================================================================*/
#ifndef ENEMY_SLIME_H
#define ENEMY_SLIME_H

#include "game_enemy.h"

class EnemySlime : public GameEnemy
{
public:
	explicit EnemySlime(const DirectX::XMFLOAT3& position);

	static bool LoadResources();
	static void UnloadResources();

	void Update(float delta_time) override;
	void Draw() const override;

	CollisionAABB GetAABBAt(const DirectX::XMFLOAT3& position) const override;

protected:
	void OnDeath() override;

private:
	float m_HopTimer{};
	float m_Angle{};

};

#endif
