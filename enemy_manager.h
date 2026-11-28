/*============================================================================
Contents   :  [enemy_manager.h]
              
Author     : Chin Qing You
LastUpdate : 2026/11/26
-----------------------------------------------------------------------------

============================================================================*/
#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include <DirectXMath.h>
#include "collision.h"

enum EnemyType
{
	kEnemyType_Slime,
	kEnemyType_Max,
};

class GameEnemy
{
public:
	virtual ~GameEnemy() = default;

	virtual void Update(float delta_time) = 0;
	virtual void Draw() const = 0;

	void Destroy() { m_is_destroy = true; };
	virtual bool IsDestroy() const { return m_is_destroy; };

	void GameEnemy_Create(EnemyType type, const DirectX::XMFLOAT3& position);
	void GameEnemy_GetActiveCount();
	void GameEnemy_Destroy(int index);
	void GameEnemy_CleanUp();

	virtual CollisionAABB GetAABB() const { return {}; };

private:
	bool m_is_destroy{ false };

};

#endif
