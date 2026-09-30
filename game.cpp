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
#include "grid.h"
#include "camera_free.h"
#include "game_object.h"

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

//void Collision_CheckPlayerVsEnemies();

void Game_Initialize()
{
	g_gameState = STATE_PLAYING;
	g_IsChangeScene = false;  
	g_EndTimer = 0.0f;
	g_RunTime = 0.0f;
	g_RunStarted = false;

	GameText_Initialize();
	FlipBookAnimation_Initialize();

	CameraFree_Initialize({ 0.0f, 3.0f, -5.0f }, XMConvertToRadians(30.0f), XMConvertToRadians(0.0f));
	Cube_Initialize();
	GameObject_Initialize();
	GameObject_Spawn("Cube", { 0.0f, 0.5f, 0.0f });
	Grid_Initialize();
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
	Grid_Finalize();
	Cube_Finalize();
	CameraFree_Finalize();

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

	Shader3d_SetViewMatrix(CameraFree_GetViewMatrix());
	Shader3d_SetProjectionMatrix(CameraFree_GetProjectionMatrix());

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


		// --- collision check ---
		//Collision_CheckPlayerVsEnemies();

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
	Sampler_SetFilter(kSamplerFilter_Point);
	Grid_Draw();

	Light_SetDirectionalLight({ 0.0f, -1.0f, 0.0f }, { 0.3f, 0.3f, 0.3f, 1.0f });

	// --- 3D texture ---
	Sampler_SetFilter(kSamplerFilter_Anisotropic);
	GameObject_Draw();

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
