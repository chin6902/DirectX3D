/*============================================================================
Contents   :  [enemy_manager.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/28
-----------------------------------------------------------------------------

============================================================================*/
#include <memory>
#include <vector>

#include "enemy_manager.h"
#include "game_enemy.h"      
#include "enemy_slime.h"

using namespace DirectX;

static constexpr int ENEMY_MAX = 128;

static std::vector<std::unique_ptr<GameEnemy>> g_Enemies;

static std::unique_ptr<GameEnemy> CreateEnemy(EnemyType type, const XMFLOAT3& position)
{
	switch (type)
	{
	case kEnemyType_Slime: return std::make_unique<EnemySlime>(position);
	default:               return nullptr;
	}
}

void EnemyManager_Initialize()
{
	g_Enemies.clear();
	g_Enemies.reserve(ENEMY_MAX);
	EnemySlime::LoadResources();
}

void EnemyManager_Finalize()
{
	g_Enemies.clear();
	EnemySlime::UnloadResources();
}

void EnemyManager_Update(float delta_time)
{
	for (const std::unique_ptr<GameEnemy>& enemy : g_Enemies)
	{
		if (!enemy->IsDestroy()) { enemy->Update(delta_time); }
	}
}

void EnemyManager_Draw()
{
	for (const std::unique_ptr<GameEnemy>& enemy : g_Enemies)
	{
		if (enemy) { enemy->Draw(); }
	}
}

void EnemyManager_CleanUp()
{
	std::erase_if(g_Enemies, [](const std::unique_ptr<GameEnemy>& enemy) { return enemy->IsDestroy(); });
}

GameEnemy* EnemyManager_Spawn(EnemyType type, const DirectX::XMFLOAT3 & position)
{
	if (static_cast<int>(g_Enemies.size()) >= ENEMY_MAX) { return nullptr; }

	std::unique_ptr<GameEnemy> enemy = CreateEnemy(type, position);
	if (!enemy) { return nullptr; }

	GameEnemy* raw = enemy.get();
	g_Enemies.push_back(std::move(enemy));
	return raw;
}

int EnemyManager_GetCount()
{
	return static_cast<int>(g_Enemies.size());
}

GameEnemy* EnemyManager_Get(int index)
{
	if (index < 0 || index >= static_cast<int>(g_Enemies.size())) { return nullptr; }
	return g_Enemies[index].get();
}
