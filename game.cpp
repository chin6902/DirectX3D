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

#include "result.h"
#include "camera.h"
#include "collision.h"
#include "collision_debug.h"
#include "shader3d.h"
#include "scene.h"
#include "fade.h"
#include "game_text.h"
#include "game_audio.h"
#include "mouse_ui.h"
#include "light.h"
#include "cube.h"
#include "mesh_field.h"
#include "camera_free.h"
#include "player_camera.h"
#include "game_object.h"

#include "game_player.h"

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

static State g_gameState = STATE_PLAYING;
static bool  g_IsChangeScene = false;
static float g_EndTimer = 0.0f;
static float g_RunTime = 0.0f;
static int   g_BgmId = -1;

static int g_TextureID_Ground{ -1 };

bool player_hit = false;

//void Collision_CheckPlayerVsEnemies();

void Game_Initialize()
{
	g_gameState = STATE_PLAYING;
	g_IsChangeScene = false;  
	g_EndTimer = 0.0f;
	g_RunTime = 0.0f;
	g_RunStarted = false;

	g_TextureID_Ground = Texture_Load(L"assets/textures/stone.png", true);

	GameText_Initialize();
	FlipBookAnimation_Initialize();

	GamePlayer_Initialize({ 0.0f, 0.0f, 0.0f }, 0.0f);

	CameraFree_Initialize({ 0.0f, 3.0f, -5.0f }, XMConvertToRadians(30.0f), XMConvertToRadians(0.0f));
	PlayerCamera_Initialize(GamePlayer_GetPosition(), 9.0f, XMConvertToRadians(25.0f), 0.0f);
	Cube_Initialize();
	GameObject_Initialize();

	GameObject_Spawn("Cube", { 0.0f, 0.5f, 0.0f });
	MeshField_Initialize(1.0f, 1.0f, 50, 50);

	Light_Initialize();

	// --- UI ---
	MouseUI_Initialize();

#ifdef _DEBUG
	Collision_Debug_Initialize();
#endif

	Fade_Start(FADE_IN, 1.0f, { 0.0f, 0.0f, 0.0f, 0.0f });
}

void Game_Finalize()
{
#ifdef _DEBUG
	Collision_Debug_Finalize();
#endif
	// ---UI ---
	MouseUI_Finalize();
	
	Light_Finalize();
	MeshField_Finalize();
	Cube_Finalize();
	CameraFree_Finalize();
	PlayerCamera_Finalize();
	GamePlayer_Finalize();

	FlipBookAnimation_Finalize();
	GameText_Finalize();
}

static void EnterEndState(bool cleared)
{
	g_gameState = cleared ? STATE_GAMECLEAR : STATE_GAMEOVER;
	g_EndTimer = cleared ? END_HOLD_CLEAR : END_HOLD_GAMEOVER;

	GameAudio_StopMusic();

}

static bool IsRunCleared()
{
	return false;
}

static void UpdateEndHold(float delta_time)
{
	Camera_Update(delta_time);
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
		if (g_gameState == STATE_PLAYING) { g_gameState = STATE_PAUSE; }
		else if (g_gameState == STATE_PAUSE) { g_gameState = STATE_PLAYING; }
	}

	CameraFree_Update(delta_time);

	const CollisionAABB playerAABB = GamePlayer_GetAABB();

	for (int i = 0; i < OBJECT_MAX; i++)
	{
		const GameObject* object = GameObject_Get(i);   // nullptr for inactive slots

		if (object == nullptr)
		{
			continue;
		}

		if (playerAABB.IsOverlap(GameObject_GetAABB(*object)))
		{
			player_hit = true;
		}
		else
		{
			player_hit = false;
		}
	}

#ifdef _DEBUG
	Cube_Update(delta_time);
#endif

	switch (g_gameState)
	{
	case STATE_PLAYING:
		g_RunTime += delta_time;

		// --- Bgm ---
		GameAudio_UpdateMusic(delta_time);

		// --- Game Update ---
		FlipBookAnimation_Update(delta_time);

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

		PlayerCamera_SetTargetPosition(GamePlayer_GetPosition());
		PlayerCamera_Update(delta_time);

		Shader3d_SetViewMatrix(PlayerCamera_GetViewMatrix());
		Shader3d_SetProjectionMatrix(PlayerCamera_GetProjectionMatrix());

		// --- collision check ---

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
	Light_SetDirectionalLight({ -0.5f, -0.8f, 1.0f }, { 0.8f, 0.8f, 0.8f, 1.0f }, { 0.4f, 0.4f, 0.4f, 1.0f });
	Texture_SetTexture(g_TextureID_Ground);
	MeshField_Draw();


	if (player_hit) {
		GameText_Draw(10.0f, 10.0f, "Player collided with object!", 1.0f, { 1.0f, 0.0f, 0.0f });
	}
	else
	{
		GameText_Draw(10.0f, 10.0f, "Player is safe.", 1.0f, { 0.0f, 1.0f, 0.0f });
	}

	// --- 3D texture ---
	Sampler_SetFilter(kSamplerFilter_Anisotropic);
	GameObject_Draw();
	
	GamePlayer_Draw();
	
	// --- 2D texture ---
	Sampler_SetFilter(kSamplerFilter_Linear);

	// --- UI ---
	Sampler_SetFilter(kSamplerFilter_Point);

	Sprite_Flush();
}

#ifdef _DEBUG
#include "imgui.h"

void Game_DrawDebugUI()
{
}
#endif
