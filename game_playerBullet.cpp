/*============================================================================
Contents   :  [game_playerBullet.cpp]

Author     : Chin Qing You
LastUpdate : 2026/06/29
-----------------------------------------------------------------------------

============================================================================*/
#include "game_playerBullet.h"
#include "config.h"
#include "texture.h"
#include "billboard.h"
#include "collision.h"
#include "collision3d_debug.h"

using namespace DirectX;

static int g_bullet_texture_ID = -1;

struct Bullet 
{
	XMFLOAT3 position{};
	XMFLOAT3 velocity{};

	float life_time = 0.0f;
	bool isDestroy{};
};

static constexpr int MAX_BULLETS = 100;
static Bullet g_Bullets[MAX_BULLETS]{};
static int g_BulletFireCount = 0;

static constexpr float BULLET_SIZE = 1.0f;   
static constexpr float BULLET_HIT = 0.6f;   
static constexpr float BULLET_SPEED = 15.0f;
static constexpr float LIFE_TIME_MAX = 4.0f;

void GamePlayerBullet_Initialize()
{
	g_bullet_texture_ID = Texture_Load(L"assets/textures/potion.png");
}

void GamePlayerBullet_Finalize()
{
	Texture_Release(g_bullet_texture_ID);
}

void GamePlayerBullet_Create(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity)
{
	if (g_BulletFireCount >= MAX_BULLETS)
	{
		return;
	}

	Bullet& r = g_Bullets[g_BulletFireCount];
	r.position = position;
	r.velocity = velocity;
	r.life_time = 0;
	r.isDestroy = false;
	g_BulletFireCount++;
}

void GamePlayerBullet_Update(float delta_time)
{
	for (int i = 0; i < g_BulletFireCount; i++)
	{
		Bullet& b = g_Bullets[i];

		XMVECTOR pos = XMLoadFloat3(&b.position);
		XMVECTOR vel = XMLoadFloat3(&b.velocity);

		pos += vel * delta_time;
		XMStoreFloat3(&b.position, pos);

		b.life_time += delta_time;
		if (b.life_time > LIFE_TIME_MAX)
		{
			b.isDestroy = true;
		}
	}
}

void GamePlayerBullet_Draw()
{
	for (int i = 0; i < g_BulletFireCount; i++)
	{
		const Bullet& b = g_Bullets[i];

		Billboard_Draw(g_bullet_texture_ID, b.position, { .size = { BULLET_SIZE, BULLET_SIZE } });
	}

#ifdef _DEBUG
	for (int i = 0; i < g_BulletFireCount; i++)
	{
		CollisionAABB ca = GamePlayerBullet_GetCollision(i);
		Collision3D_Debug_Add(ca);
	}
#endif
}

int GamePlayerBullet_GetActiveCount()
{
	return g_BulletFireCount;
}

void GamePlayerBullet_Destroy(int bulletIndex)
{
	if (bulletIndex < 0 || bulletIndex >= g_BulletFireCount)
	{
		return;
	}

	g_Bullets[bulletIndex].isDestroy = true;
}

void GamePlayerBullet_CleanUp()
{
	for (int i = g_BulletFireCount - 1; i >= 0; --i)
	{
		if (g_Bullets[i].isDestroy)
		{
			g_Bullets[i] = g_Bullets[g_BulletFireCount - 1];
			g_BulletFireCount--;
		}
	}
}

void GamePlayerBullet_Fire(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction)
{
	XMVECTOR dir = XMVector3Normalize(XMLoadFloat3(&direction));

	XMFLOAT3 velocity;
	XMStoreFloat3(&velocity, dir * BULLET_SPEED);

	GamePlayerBullet_Create(position, velocity);
}

CollisionAABB GamePlayerBullet_GetCollision(int bulletIndex)
{
	if (bulletIndex < 0 || bulletIndex >= g_BulletFireCount)
	{
		return {}; 
	}

	const Bullet& b = g_Bullets[bulletIndex];

	const float half = BULLET_HIT * 0.5f;
	return CollisionAABB(half, half, half, b.position);
}


