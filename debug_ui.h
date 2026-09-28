/*============================================================================
Contents   :  [debug_ui.h]

Author     : Chin Qing You
LastUpdate : 2026/09/21
-----------------------------------------------------------------------------

============================================================================*/
#ifndef DEBUG_UI_H
#define DEBUG_UI_H

#include <Windows.h>

bool DebugUI_Initialize(HWND hWnd);
void DebugUI_Finalize();

// Call at the top of WndProc. true = ImGui consumed the message.
bool DebugUI_ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void DebugUI_Begin();   
void DebugUI_Draw();    
void DebugUI_End();   

// true while the mouse is over an ImGui window or a widget has keyboard focus
bool DebugUI_WantsInput();

#endif