/*============================================================================
Contents   :  [map_block.h]

Author     : Chin Qing You
LastUpdate : 2026/11/12
-----------------------------------------------------------------------------

============================================================================*/
#ifndef MAP_BLOCK_H
#define MAP_BLOCK_H

#include "map_object.h"

class MapBlock : public MapObject
{
public:
	MapBlock(const DirectX::XMFLOAT3& position, int texture_id) : MapObject(position), m_texture_id(texture_id) {};

    //void Update(float delta_time) override {};
	void Draw() const override;

	HitData3D IsHit(const CollisionAABB& aabb) const override;
	//CollisionAABB GetAABB() const { return CollisionAABB{ 0.5f, 0.5f, 0.5f, GetPosition() }; };

private:
	int m_texture_id{ -1 };

};

#endif
