/*============================================================================
Contents   :  [game_map.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/12
-----------------------------------------------------------------------------
todo: refactor with class later and able to work with multiple textures/models
============================================================================*/
#include "game_map.h"
#include "texture.h"
#include "map_model.h"
#include "map_block.h"
#include "map_move_block.h"

constexpr unsigned int MAP_OBJECT_MAX{ 1000 };
static MapObject* g_MapObjects[MAP_OBJECT_MAX]{};

static int g_TextureID_Ground{ -1 };
//static MODEL* g_pModel_Stone{ nullptr };

void GameMap_Initialize()
{
	for (MapObject*& p : g_MapObjects)
	{
		if (p) delete p;
		p = nullptr;
	}

	// load the resources for the map, such as textures and models
	g_TextureID_Ground = Texture_Load(L"assets/textures/cube.png", true);

	// make the map datas
	g_MapObjects[0] = new MapBlock({ -3.0f, 0.5f, -3.0f }, g_TextureID_Ground);
	g_MapObjects[1] = new MapBlock({ -4.0f, 1.5f, -2.0f }, g_TextureID_Ground);
	g_MapObjects[2] = new MapBlock({  3.0f, 0.5f, -3.0f }, g_TextureID_Ground);
	g_MapObjects[3] = new MapBlock({  3.0f, 3.5f,  3.0f }, g_TextureID_Ground);

	g_MapObjects[4] = new MapMoveBlock({ 1.0f, 0.5f, 1.0f }, g_TextureID_Ground);
}

void GameMap_Finalize()
{
	for (MapObject*& p : g_MapObjects)
	{
		if (p) delete p;
		p = nullptr;
	}

	Texture_Release(g_TextureID_Ground);
}

void GameMap_Update(float delta_time)
{
	for (MapObject* p : g_MapObjects)
	{
		if (!p) break;
		p->Update(delta_time);
	}
}

void GameMap_Draw()
{
	for (const MapObject* p : g_MapObjects)
	{
		if (!p) break;
		p->Draw();
	}
}

HitData3D GameMap_CheckCollision(const CollisionAABB& aabb)
{
	for (const MapObject* p : g_MapObjects)
	{
		if (!p) continue;

		HitData3D hit = p->IsHit(aabb);
		if (hit.IsHit())
		{
			return hit;
		}
	}

	return {};
}

DirectX::XMFLOAT3 GameMap_GetFloorMove(const CollisionAABB& aabb)
{
	for (const MapObject* p : g_MapObjects)
	{
		if (!p) continue;

		// nullptr if this object isn't a MapMoveBlock
		const MapMoveBlock* move_block = dynamic_cast<const MapMoveBlock*>(p);
		if (!move_block) continue;

		if (move_block->IsHit(aabb).IsHit()) return move_block->GetMoveDelta();
	}
	return { 0.0f, 0.0f, 0.0f };
}
