/*============================================================================
Contents   :  [map_model.h]

Author     : Chin Qing You
LastUpdate : 2026/11/12
-----------------------------------------------------------------------------
todo: make it so that aabb can be assigned
============================================================================*/
#ifndef MAP_MODEL_H
#define MAP_MODEL_H

#include "map_object.h"
#include "model.h"

class MapModel : public MapObject
{
public:
	MapModel(MODEL* pModel,
		const DirectX::XMFLOAT3& position = { 0.0f, 0.0f, 0.0f },
		const DirectX::XMFLOAT3& scale	  = { 1.0f, 1.0f, 1.0f },
		const DirectX::XMFLOAT3& rotation = { 0.0f, 0.0f, 0.0f })
		: m_pModel(pModel), MapObject(position), m_scale(scale), m_rotation(rotation) {};

	//void Update(float delta_time) override {};
	void Draw() const override;

	HitData3D IsHit(const CollisionAABB& aabb) const override;
	CollisionAABB GetWorldAABB() const;
	CollisionAABB GetAABB() const override { return GetWorldAABB(); };

private:
	MODEL* m_pModel{ nullptr };
	DirectX::XMFLOAT3 m_scale{};
	DirectX::XMFLOAT3 m_rotation{}; // z, x, y order

};

#endif
