#include "game_explosion.h"
#include "flipbook_animation.h"
#include "texture.h"
#include "render_state.h"

using namespace DirectX;

struct Explosion
{
	XMFLOAT3 position{};
	int      animId{ -1 };
	bool     active{ false };
};

static constexpr int MAX_EXPLOSIONS = 32;

static int g_TextureExplosion{ -1 };
static Explosion g_Explosions[MAX_EXPLOSIONS];

void GameExplosion_Initialize()
{
	g_TextureExplosion = Texture_Load(L"assets/textures/explosion.png", true);

	for (Explosion& e : g_Explosions)
	{
		e = {};
		e.animId = FlipBookAnimation_Create(g_TextureExplosion, 128, 128, 8, 4, 0.1f);
		FlipBookAnimation_SetMode(e.animId, AnimPlayMode::FREEZE);  
	}
}

void GameExplosion_Finalize()
{
	for (Explosion& e : g_Explosions)
	{
		FlipBookAnimation_Destroy(e.animId);   
		e = {};
	}
	Texture_Release(g_TextureExplosion);
}

void GameExplosion_Update(float /*delta_time*/)
{
	for (Explosion& e : g_Explosions)
	{
		if (!e.active) continue;

		if (FlipBookAnimation_IsFinished(e.animId))
		{
			e.active = false;
			FlipBookAnimation_SetMode(e.animId, AnimPlayMode::FREEZE);
		}
	}
}

void GameExplosion_Draw()
{
	RenderState_SetDepth(kDepth_ReadOnly);
	for (const Explosion& e : g_Explosions)
	{
		if (!e.active) continue;
		FlipBookAnimation_DrawBillboard(e.animId, e.position, { .size = { 3.0f, 3.0f } });
	}
}

void GameExplosion_Spawn(const XMFLOAT3& position)
{
	for (Explosion& e : g_Explosions)
	{
		if (e.active) continue;

		e.position = position;
		e.active = true;

		FlipBookAnimation_SetMode(e.animId, AnimPlayMode::ONE_SHOT);
		FlipBookAnimation_SetFrame(e.animId, 0);  
		return;
	}
}