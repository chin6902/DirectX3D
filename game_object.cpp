/*============================================================================
Contents   :  [game_object.cpp]

Author     : Chin Qing You
-----------------------------------------------------------------------------

============================================================================*/
#include "game_object.h"
#include "cube.h"

using namespace DirectX;

#ifdef _DEBUG
#include <cstdio>
#include "imgui.h"
#endif

static GameObject g_Objects[OBJECT_MAX]{};

static void CopyName(char* destination, const char* source)
{
	int i = 0;

	while (source != nullptr && source[i] != '\0' && i < OBJECT_NAME_MAX - 1)
	{
		destination[i] = source[i];
		i++;
	}

	destination[i] = '\0';
}

void GameObject_Initialize()
{
	for (GameObject& object : g_Objects)
	{
		object.active = false;
	}
}

int GameObject_Spawn(const char* name, const XMFLOAT3& position)
{
	for (int i = 0; i < OBJECT_MAX; i++)
	{
		if (g_Objects[i].active)
		{
			continue;
		}

		GameObject& object = g_Objects[i];
		CopyName(object.name, (name != nullptr && name[0] != '\0') ? name : "Object");
		object.position = position;
		object.rotation = { 0.0f, 0.0f, 0.0f };
		object.scale = { 1.0f, 1.0f, 1.0f };
		object.active = true;

		return i;
	}

	return -1;
}

void GameObject_Destroy(int index)
{
	if (index < 0 || index >= OBJECT_MAX)
	{
		return;
	}

	g_Objects[index].active = false;
}

XMMATRIX GameObject_GetWorldMatrix(const GameObject& object)
{
	return XMMatrixScaling(object.scale.x, object.scale.y, object.scale.z)
		* XMMatrixRotationRollPitchYaw(object.rotation.x, object.rotation.y, object.rotation.z)
		* XMMatrixTranslation(object.position.x, object.position.y, object.position.z);
}

void GameObject_Draw()
{
	for (const GameObject& object : g_Objects)
	{
		if (!object.active)
		{
			continue;
		}

		Cube_Draw(GameObject_GetWorldMatrix(object));
	}
}

GameObject* GameObject_Get(int index)
{
	if (index < 0 || index >= OBJECT_MAX || !g_Objects[index].active)
	{
		return nullptr;
	}

	return &g_Objects[index];
}

#ifdef _DEBUG

// Editor-only state. Which tabs are open is not game data, so it lives
// outside GameObject and never exists in a Release build.
static bool g_InspectorOpen[OBJECT_MAX]{};
static int  g_FocusRequest = -1;   // tab to select this frame, -1 = none
static int  g_SpawnCounter = 0;    // keeps auto-generated names unique

static void DrawHierarchy()
{
	ImGui::Begin("Hierarchy");

	if (ImGui::Button("Spawn Cube"))
	{
		char name[OBJECT_NAME_MAX];
		snprintf(name, sizeof(name), "Cube %d", ++g_SpawnCounter);

		const int spawned = GameObject_Spawn(name, { 0.0f, 0.5f, 0.0f });

		if (spawned >= 0)
		{
			g_InspectorOpen[spawned] = true;
			g_FocusRequest = spawned;
		}
	}

	ImGui::Separator();

	for (int i = 0; i < OBJECT_MAX; i++)
	{
		if (!g_Objects[i].active)
		{
			continue;
		}

		ImGui::PushID(i);

		// Passing g_InspectorOpen[i] highlights rows whose tab is open
		if (ImGui::Selectable(g_Objects[i].name, g_InspectorOpen[i]))
		{
			g_InspectorOpen[i] = true;
			g_FocusRequest = i;
		}

		ImGui::PopID();
	}

	ImGui::End();
}

static void DrawTransform(GameObject& object)
{
	ImGui::InputText("Name", object.name, OBJECT_NAME_MAX);

	ImGui::DragFloat3("Position", &object.position.x, 0.01f);
	ImGui::SliderAngle("Rotation X", &object.rotation.x);
	ImGui::SliderAngle("Rotation Y", &object.rotation.y);
	ImGui::SliderAngle("Rotation Z", &object.rotation.z);
	ImGui::DragFloat3("Scale", &object.scale.x, 0.01f, 0.01f, 10.0f);

	if (ImGui::Button("Reset Transform"))
	{
		object.rotation = { 0.0f, 0.0f, 0.0f };
		object.scale = { 1.0f, 1.0f, 1.0f };
	}
}

static void DrawInspector()
{
	int pending_destroy = -1;

	ImGui::Begin("Inspector");

	if (ImGui::BeginTabBar("objects", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs))
	{
		for (int i = 0; i < OBJECT_MAX; i++)
		{
			if (!g_Objects[i].active || !g_InspectorOpen[i])
			{
				continue;
			}

			// Text after ### is the ID, so renaming does not reset the tab
			char label[OBJECT_NAME_MAX + 16];
			snprintf(label, sizeof(label), "%s###obj%d", g_Objects[i].name, i);

			const ImGuiTabItemFlags flags = (g_FocusRequest == i) ? ImGuiTabItemFlags_SetSelected : 0;

			// &g_InspectorOpen[i] gives the tab its close button
			if (ImGui::BeginTabItem(label, &g_InspectorOpen[i], flags))
			{
				ImGui::PushID(i);

				DrawTransform(g_Objects[i]);

				ImGui::Separator();

				if (ImGui::Button("Delete Object"))
				{
					pending_destroy = i;   // never destroy inside the tab
				}

				ImGui::PopID();
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

	if (pending_destroy >= 0)
	{
		GameObject_Destroy(pending_destroy);
		g_InspectorOpen[pending_destroy] = false;
	}
}

void GameObject_DrawDebugUI()
{
	DrawHierarchy();
	DrawInspector();

	g_FocusRequest = -1;
}

#endif