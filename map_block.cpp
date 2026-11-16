/*============================================================================
Contents   :  [map_block.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/12
-----------------------------------------------------------------------------

============================================================================*/
#include "map_block.h"
#include "texture.h"
#include "cube.h"
#include "collision3d_debug.h"

using namespace DirectX;

void MapBlock::Draw() const
{
#ifdef _DEBUG
	Collision3D_Debug_Add(CollisionAABB{ 0.5f, 0.5f, 0.5f, GetPosition() });
#endif

	Texture_SetTexture(m_texture_id);
	Cube_Draw(XMMatrixTranslationFromVector(XMLoadFloat3(&GetPosition())));
}

HitData3D MapBlock::IsHit(const CollisionAABB& aabb) const
{
	// player (aabb) is the caller, so the normal pushes the player out of the block
	return aabb.IsHit(CollisionAABB{ 0.5f, 0.5f, 0.5f, GetPosition() });
}
