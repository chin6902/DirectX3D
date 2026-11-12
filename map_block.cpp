/*============================================================================
Contents   :  [map_block.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/12
-----------------------------------------------------------------------------

============================================================================*/
#include "map_block.h"
#include "texture.h"
#include "cube.h"

using namespace DirectX;

void MapBlock::Draw() const
{
	Texture_SetTexture(m_texture_id);
	Cube_Draw(XMMatrixTranslationFromVector(XMLoadFloat3(&GetPosition())));
}

HitData3D MapBlock::IsHit(const CollisionAABB& aabb) const
{
	return CollisionAABB{ 0.5f, 0.5f, 0.5f, GetPosition() }.IsHit(aabb);
}
