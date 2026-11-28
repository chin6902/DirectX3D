#include "game_explosion.h"
#include "flipbook.h"
#include "texture.h"

using namespace DirectX;

struct Explosion
{
	XMFLOAT3 position{};
	FlipBookPlayer anim{};
	bool     active{ false };
};

static constexpr int MAX_EXPLOSIONS = 32;

static int g_TextureExplosion{ -1 };
static FlipBookSheet g_ExplosionSheet;
static Explosion g_Explosions[MAX_EXPLOSIONS];

void GameExplosion_Initialize()
{
	g_TextureExplosion = Texture_Load(L"assets/textures/explosion.png", true);

	g_ExplosionSheet = {
		.texture_id = g_TextureExplosion,
		.frame_width = 128,
		.frame_height = 128,
		.frame_count = 8,
		.columns = 4,
		.frame_time = 0.1f
	};

	for (Explosion& e : g_Explosions)
	{
		e = {}; 
	}
}

void GameExplosion_Finalize()
{
	for (Explosion& e : g_Explosions)
	{
		e = {};
	}
	Texture_Release(g_TextureExplosion);
}

void GameExplosion_Update(float delta_time)
{
	for (Explosion& e : g_Explosions)
	{
		if (!e.active) continue;

		e.anim.Update(delta_time);
		if (e.anim.IsFinished())
		{
			e.active = false;
		}
	}
}

void GameExplosion_Draw()
{
	for (const Explosion& e : g_Explosions)
	{
		if (!e.active) continue;
		e.anim.DrawBillboard(e.position, {.size = { 3.0f, 3.0f }});
	}
}

void GameExplosion_Spawn(const XMFLOAT3& position)
{
	for (Explosion& e : g_Explosions)
	{
		if (e.active) continue;

		e.position = position;
		e.anim = FlipBookPlayer(&g_ExplosionSheet, AnimPlayMode::ONE_SHOT);
		e.active = true;

		return;
	}
}