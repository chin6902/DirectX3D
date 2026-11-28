/*============================================================================
Contents   :  [game_enemy.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/26
-----------------------------------------------------------------------------

============================================================================*/
#include "game_enemy.h"

#ifdef _DEBUG
#include "collision3d_debug.h"
#endif

using namespace DirectX;

static constexpr int ENEMY_MAX = 128;
//static Enemy* g_Enemies[ENEMY_MAX]{};
static int g_EnemyCount{};

struct Enemy
{
	EnemyType type;
	DirectX::XMFLOAT3 position;
	bool is_destroy;
};

static float speed = 1.0f;

void GameEnemy_Initialize()
{
	/*for (GameEnemy*& p : g_Enemies)
	{
		if (p) delete p;
		p = nullptr;
	}
	g_EnemyCount = 0;*/
}

void GameEnemy_Finalize()
{
	/*for (GameEnemy*& p : g_Enemies)
	{
		if (p) delete p;
		p = nullptr;
	}
	g_EnemyCount = 0;*/
}

void GameEnemy_Register(GameEnemy* enemy)
{
	/*if (g_EnemyCount >= ENEMY_MAX) return;
	
	g_Enemies[g_EnemyCount] = enemy;
	g_EnemyCount++;*/
}

void GameEnemy_Update(float delta_time)
{
	/*for (int i = 0; i < g_EnemyCount; i++)
	{
		g_Enemies[i]->Update(delta_time);
	}*/
}

void GameEnemy_Draw()
{
	/*for (int i = 0; i < g_EnemyCount; i++)
	{
		g_Enemies[i]->Draw();
	}*/
}

void GameEnemy_Destroy(int index)
{
	//g_Enemies[index]->Destroy();
}

void GameEnemy_CleanUp()
{
	/*for (int i = g_EnemyCount - 1; i >= 0; i--)
	{
		if (g_Enemies[i]->IsDestroy())
		{
			delete g_Enemies[i];
			g_Enemies[i] = g_Enemies[--g_EnemyCount];
			g_Enemies[g_EnemyCount] = nullptr;
		}
	}*/
}

CollisionAABB GameEnemy_GetCollision(int index)
{
	/*if (index < 0 || index >= g_EnemyCount) { return {}; }
	return g_Enemies[index]->GetAABB();*/
	return {};
}

