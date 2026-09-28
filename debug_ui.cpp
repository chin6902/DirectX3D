/*============================================================================
Contents   :  [debug_ui.cpp]

Author     : Chin Qing You
LastUpdate : 2026/09/21
-----------------------------------------------------------------------------

============================================================================*/
#include "debug_ui.h"

#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "direct3d.h"

#include "game.h"
#include "camera_free.h"
#include "game_object.h"

// Declared inside #if 0 in imgui_impl_win32.h, so declare it ourselves
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static bool g_Initialized = false;

bool DebugUI_Initialize(HWND hWnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// The process is per-monitor DPI aware (main.cpp), so scale to the monitor
	const float dpi_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST));
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(dpi_scale);
	style.FontScaleDpi = dpi_scale;

	if (!ImGui_ImplWin32_Init(hWnd))
	{
		return false;
	}

	if (!ImGui_ImplDX11_Init(Direct3D_GetDevice(), Direct3D_GetDeviceContext()))
	{
		return false;
	}

	g_Initialized = true;
	return true;
}

void DebugUI_Finalize()
{
	if (!g_Initialized)
	{
		return;
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	g_Initialized = false;
}

bool DebugUI_ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// WndProc receives messages before Initialize runs (window creation)
	if (!g_Initialized)
	{
		return false;
	}

	return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam) != 0;
}

void DebugUI_Begin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void DebugUI_Draw()
{
	Game_DrawDebugUI();
	CameraFree_DrawDebugUI();
	GameObject_DrawDebugUI();
}

void DebugUI_End()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool DebugUI_WantsInput()
{
	const ImGuiIO& io = ImGui::GetIO();
	return io.WantCaptureMouse || io.WantCaptureKeyboard;
}

#else // Release: stubs

bool DebugUI_Initialize(HWND) { return true; }
void DebugUI_Finalize() {}
bool DebugUI_ProcessMessage(HWND, UINT, WPARAM, LPARAM) { return false; }
void DebugUI_Begin() {}
void DebugUI_Draw() {}
void DebugUI_End() {}
bool DebugUI_WantsInput() { return false; }

#endif