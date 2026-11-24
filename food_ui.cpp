/*============================================================================
Contents   :  [food_ui.h]

Author     : Chin Qing You
LastUpdate : 2026/11/21
-----------------------------------------------------------------------------

============================================================================*/
#include "food_ui.h"

#ifdef _DEBUG	
#include <cstdio>

#include "imgui.h"
#include "food_system.h"
#include "AIService.h"		

static bool g_Visible = false;
static bool g_FocusRequest = false;	
static char g_Input[128] = "";	
static int  g_LastStatus = 0;		

void FoodUI_SetVisible(bool visible)
{
	g_Visible = visible;
}

static void DrawTasteBar(const char* label, int value, const ImVec4& color)
{
	char overlay[32];
	snprintf(overlay, sizeof(overlay), "%s %d", label, value);

	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
	ImGui::ProgressBar(value / 10.0f, ImVec2(ImGui::GetFontSize() * 6.0f, 0.0f), overlay);
	ImGui::PopStyleColor();
}

static void DrawInput()
{
	if (g_FocusRequest)
	{
		ImGui::SetKeyboardFocusHere();		
		g_FocusRequest = false;
	}

	ImGui::SetNextItemWidth(-1.0f);		
	bool enterPressed = ImGui::InputTextWithHint("##food_input", "Type a dish, then press Enter",
		g_Input, sizeof(g_Input), ImGuiInputTextFlags_EnterReturnsTrue);

	if (enterPressed && g_Input[0] != '\0')
	{
		g_LastStatus = FoodSystem_Request(g_Input);
		g_Input[0] = '\0';
	}
}

static void DrawStatus()
{
	if (FoodSystem_IsBusy())
	{
		int dots = 1 + (int)(ImGui::GetTime() * 3.0) % 3;
		ImGui::Text("Judging: %s%.*s", FoodSystem_GetPendingWord(0), dots, "...");

		if (FoodSystem_GetPendingCount() > 1)
		{
			ImGui::SameLine();
			ImGui::TextDisabled("(+%d waiting)", FoodSystem_GetPendingCount() - 1);
		}
	}
	else if (g_LastStatus == FOOD_REQUEST_REJECTED)
	{
		ImGui::TextDisabled("Not sent: the text was empty or too long.");
	}
	else if (g_LastStatus == FOOD_REQUEST_BUSY)
	{
		ImGui::TextDisabled("Wait for the dishes that are being judged.");
	}
	else
	{
		ImGui::TextDisabled("Press Enter to type a dish.");
	}
}

static void DrawResult(const FoodResult& r)
{
	const FoodData& f = r.food;

	if (f.isFood)
	{
		ImGui::TextUnformatted(f.name.c_str());
		ImGui::SameLine();
		ImGui::TextDisabled("[%s]  %s", f.cuisine.c_str(), SourceName(f.source));

		DrawTasteBar("sweet", f.sweet, ImVec4(0.90f, 0.50f, 0.65f, 1.0f));
		ImGui::SameLine();
		DrawTasteBar("salty", f.salty, ImVec4(0.45f, 0.60f, 0.85f, 1.0f));
		ImGui::SameLine();
		DrawTasteBar("sour", f.sour, ImVec4(0.75f, 0.75f, 0.30f, 1.0f));
		ImGui::SameLine();
		DrawTasteBar("spicy", f.spicy, ImVec4(0.85f, 0.35f, 0.25f, 1.0f));

		if (!f.ingredients.empty())
		{
			string tags = "with:";
			for (const string& tag : f.ingredients)
			{
				tags += " " + tag;
			}
			ImGui::TextDisabled("%s", tags.c_str());
		}
	}
	else
	{
		// Refused: show what the player typed and why it was refused.
		ImGui::TextUnformatted(r.word.c_str());
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.95f, 0.65f, 0.30f, 1.0f), "%s", RefusalReason(f));

		if (!f.reason.empty())
		{
			ImGui::TextDisabled("AI: %s", f.reason.c_str());
		}
	}

	// Set when the AI could not be used
	// answer came from the fallback table instead.
	if (!r.error.empty())
	{
		ImGui::TextDisabled("AI not used: %s", r.error.c_str());
	}
}

void FoodUI_Draw()
{
	if (!g_Visible)
	{
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

	// Enter starts typing, but only when no text box is in use already
	if (!io.WantTextInput && (ImGui::IsKeyPressed(ImGuiKey_Enter, false) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false)))
	{
		g_FocusRequest = true;
	}

	const float font = ImGui::GetFontSize();
	ImGui::SetNextWindowPos(ImVec2(font, io.DisplaySize.y - font * 23.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(font * 30.0f, font * 22.0f), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Food"))
	{
		DrawInput();
		DrawStatus();
		ImGui::Separator();

		for (int i = 0; i < FoodSystem_GetResultCount(); i++)
		{
			const FoodResult& r = FoodSystem_GetResult(i);

			ImGui::PushID(r.id);
			DrawResult(r);
			ImGui::Separator();
			ImGui::PopID();
		}
	}
	ImGui::End();		
}

#else 

void FoodUI_SetVisible(bool) {}
void FoodUI_Draw() {}

#endif
