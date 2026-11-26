/*============================================================================
Contents   :  [game.cpp]
              
Author     : Chin Qing You
LastUpdate : 2026/06/24
-----------------------------------------------------------------------------

============================================================================*/
#include "game.h"
#include "input_keyboard.h"
#include "texture.h"
#include "sprite.h"
#include "config.h"
#include "flipbook_animation.h"
#include "debug_ui.h"
#include "sampler.h"
#include "render_state.h"

#include "shader_manager.h"
#include "shader_constant.h"

#include "result.h"
#include "camera.h"
#include "collision.h"
#include "collision_debug.h"
#include "collision3d_debug.h"
#include "scene.h"
#include "fade.h"
#include "game_text.h"
#include "game_audio.h"
#include "mouse_ui.h"
#include "light.h"
#include "grid.h"
#include "mesh_field.h"
#include "camera_free.h"
#include "player_camera.h"
#include "game_object.h"
#include "game_map.h"
#include "cube.h"

#include "billboard.h"

#include "game_player.h"
#include "food_ui.h"	

#include "game_playerBullet.h"
#include "game_explosion.h"

using namespace DirectX;

enum State
{
	STATE_PLAYING,
	STATE_PAUSE,
	STATE_LEVELUP,
	STATE_GAMEOVER,
	STATE_GAMECLEAR,
};

static constexpr float END_HOLD_GAMEOVER = 1.40f;
static constexpr float END_HOLD_CLEAR = 2.00f;
static bool g_RunStarted = false;

static State g_GameState = STATE_PLAYING;
static bool  g_IsChangeScene = false;
static float g_EndTimer = 0.0f;
static float g_RunTime = 0.0f;
static int   g_BgmId = -1;

static int g_TextureID_Ground{ -1 };
static int g_TextureID_Potion{ -1 };
bool player_hit = false;

void Collision_BulletsVsObjects();
void Collision_BulletsVsMapObject();

void Game_Initialize()
{
	g_GameState = STATE_PLAYING;
	g_IsChangeScene = false;  
	g_EndTimer = 0.0f;
	g_RunTime = 0.0f;
	g_RunStarted = false;

	g_TextureID_Ground = Texture_Load(L"assets/textures/stone.png", true);
	g_TextureID_Potion = Texture_Load(L"assets/textures/potion.png", true);

	ShaderConstant_Get3D().Initialize();
	GameText_Initialize();
	FlipBookAnimation_Initialize();

	GamePlayer_Initialize({ 0.0f, 0.0f, 0.0f }, 0.0f);

	CameraFree_Initialize({ 0.0f, 3.0f, -5.0f }, XMConvertToRadians(30.0f), XMConvertToRadians(0.0f));
	PlayerCamera_Initialize(GamePlayer_GetPosition(), 9.0f, XMConvertToRadians(25.0f), 0.0f);
	Cube_Initialize();
	GameMap_Initialize();
	GameObject_Initialize();

	GameObject_Spawn("Cube", { 0.0f, 0.5f, 0.0f });
	MeshField_Initialize(1.0f, 1.0f, 50, 50);
	Grid_Initialize();

	Billboard_Initialize();
	GamePlayerBullet_Initialize();
	GameExplosion_Initialize();

	Light_Initialize();

	// --- UI ---
	MouseUI_Initialize();
	FoodUI_SetVisible(true);

#ifdef _DEBUG
	//Collision_Debug_Initialize();
	Collision3D_Debug_Initialize();
#endif

	Fade_Start(FADE_IN, 1.0f, { 0.0f, 0.0f, 0.0f, 0.0f });
}

void Game_Finalize()
{
#ifdef _DEBUG
	Collision3D_Debug_Finalize();
	//Collision_Debug_Finalize();
#endif
	// ---UI ---
	FoodUI_SetVisible(false);
	MouseUI_Finalize();
	GameExplosion_Finalize();
	GamePlayerBullet_Finalize();
	
	Light_Finalize();
	Billboard_Finalize();
	Grid_Finalize();	
	MeshField_Finalize();
	GameMap_Finalize();
	Cube_Finalize();
	CameraFree_Finalize();
	PlayerCamera_Finalize();
	GamePlayer_Finalize();

	Texture_Release(g_TextureID_Ground);
	Texture_Release(g_TextureID_Potion);

	FlipBookAnimation_Finalize();
	GameText_Finalize();

	ShaderConstant_Get3D().Finalize();
}

static void EnterEndState(bool cleared)
{
	g_GameState = cleared ? STATE_GAMECLEAR : STATE_GAMEOVER;
	g_EndTimer = cleared ? END_HOLD_CLEAR : END_HOLD_GAMEOVER;

	GameAudio_StopMusic();

}

static bool IsRunCleared()
{
	return false;
}

static void UpdateEndHold(float delta_time)
{
	FlipBookAnimation_Update(delta_time);

	if (g_EndTimer > 0.0f)
	{
		g_EndTimer -= delta_time;
		return;
	}

	if (!g_IsChangeScene)
	{
		Fade_Start(FADE_OUT, 1.0f, { 0.0f, 0.0f, 0.0f, 1.0f });
		g_IsChangeScene = true;
	}
}

void Game_Update(float delta_time)
{
	if (!DebugUI_WantsInput() && InputKeyboard_IsTrigger(KK_P))
	{
		if (g_GameState == STATE_PLAYING) { g_GameState = STATE_PAUSE; }
		else if (g_GameState == STATE_PAUSE) { g_GameState = STATE_PLAYING; }
	}

	CameraFree_Update(delta_time);

#ifdef _DEBUG
#endif

	switch (g_GameState)
	{
	case STATE_PLAYING:
		g_RunTime += delta_time;

		// --- Bgm ---
		GameAudio_UpdateMusic(delta_time);

		// --- Game Update ---
		FlipBookAnimation_Update(delta_time);
		GameMap_Update(delta_time);
		GamePlayerBullet_Update(delta_time);
		
		// --- Player Update ---
		if (!InputKeyboard_IsPress(KK_LEFTSHIFT))
		{ 
			GamePlayer_Update(delta_time);
		}

		if (InputKeyboard_IsPress(KK_Q))
		{
			PlayerCamera_Orbit(-XM_PI * delta_time);
		}
		if (InputKeyboard_IsPress(KK_E))
		{
			PlayerCamera_Orbit(XM_PI * delta_time);
		}
		if (InputKeyboard_IsTrigger(KK_H))
		{
			XMVECTOR front = XMLoadFloat3(&GamePlayer_GetFrontVector());
			XMVECTOR spawn = XMLoadFloat3(&GamePlayer_GetAABB().GetCenter());
			spawn += front * 0.8f;   // start just in front of the player so it isn't hidden inside the model

			XMFLOAT3 spawnPos, dir;
			XMStoreFloat3(&spawnPos, spawn);
			XMStoreFloat3(&dir, front);

			GamePlayerBullet_Fire(spawnPos, dir);
		}

		PlayerCamera_SetTargetPosition(GamePlayer_GetPosition());
		PlayerCamera_Update(delta_time);

		ShaderConstant_Get3D().SetViewMatrix(PlayerCamera_GetViewMatrix());
		ShaderConstant_Get3D().SetProjectionMatrix(PlayerCamera_GetProjectionMatrix());

		Billboard_SetViewMatrix(PlayerCamera_GetViewMatrix());

		// --- collision check ---
		Collision_BulletsVsObjects();
		Collision_BulletsVsMapObject();
		GamePlayerBullet_CleanUp();
		GameExplosion_Update(delta_time);

		// --- UI ---

		if (!g_RunStarted)
		{
			g_RunStarted = true;
		}
		break;

	case STATE_PAUSE:
		break;

	case STATE_LEVELUP:
		break;

	case STATE_GAMEOVER:
	case STATE_GAMECLEAR:
		UpdateEndHold(delta_time);
		break;
	}

	if (g_IsChangeScene && Fade_IsFinished())
	{
		Scene_SetNextScene(SCENE_RESULT);
	}
}

void Game_Draw()
{
	// --- 3D ---
	ShaderConstant_Get3D().Bind();
	ShaderManager_Bind(kShader_3D);
	Light_SetDirectionalLight({ -0.5f, -0.8f, 1.0f }, { 0.8f, 0.8f, 0.8f, 1.0f }, { 0.4f, 0.4f, 0.4f, 1.0f });

	Sampler_SetFilter(kSamplerFilter_Anisotropic);
	Texture_SetTexture(g_TextureID_Ground);

	MeshField_Draw();
	GameMap_Draw();
	GameObject_Draw();
	GamePlayer_Draw();

	// --- primitives (world-space lines) ---
	ShaderManager_Bind(kShader_Primitive3D);
	ShaderConstant_Get3D().SetColor({ 0.5f, 1.0f, 0.1f, 1.0f });
	Grid_Draw();

	// --- transparent 3D (after all opaque) ---
	Billboard_Draw(g_TextureID_Potion, { 2.0f, 3.0f, 0.0f });
	Billboard_Draw(g_TextureID_Potion, { 2.5f, 3.0f, 0.0f }, { 1.0f, 1.0f });
	Billboard_Draw(g_TextureID_Potion, { 3.0f, 3.0f, 0.0f }, { 1.0f, 1.0f });
	

	GamePlayerBullet_Draw();
	GameExplosion_Draw();	

	ShaderManager_Bind(kShader_Billboard);
	Billboard_Flush();

#ifdef _DEBUG
	ShaderManager_Bind(kShader_Primitive3D);
	ShaderConstant_Get3D().SetColor({ 1.0f, 0.2f, 0.2f, 1.0f });
	Collision3D_Debug_Draw();
#endif

	// --- text / sprites ---
	Sprite_Flush();
}

#ifdef _DEBUG
#include "imgui.h"

void Game_DrawDebugUI()
{
}
#endif

// ===== rewrite game object with class ======
void Collision_BulletsVsObjects()
{
	for (int i = 0; i < GamePlayerBullet_GetActiveCount(); i++)
	{
		const CollisionAABB bullet = GamePlayerBullet_GetCollision(i);

		for (int j = 0; j < OBJECT_MAX; j++)
		{
			const GameObject* object = GameObject_Get(j);
			if (object == nullptr) continue;

			if (bullet.IsOverlap(GameObject_GetAABB(*object)))
			{
				GameExplosion_Spawn(bullet.GetCenter());
				GamePlayerBullet_Destroy(i);
				break;
			}
		}
	}
}

void Collision_BulletsVsMapObject()
{
	for (int i = GamePlayerBullet_GetActiveCount() - 1; i >= 0; i--)
	{
		const CollisionAABB bullet = GamePlayerBullet_GetCollision(i);

		if (GameMap_CheckCollision(bullet).IsHit())
		{
			GameExplosion_Spawn(bullet.GetCenter());
			GamePlayerBullet_Destroy(i);
		}
	}
}

