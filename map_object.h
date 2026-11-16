/*============================================================================
Contents   :  [map_object.h]
              
Author     : Chin Qing You
LastUpdate : 2026/11/12
-----------------------------------------------------------------------------
make setting scale and rotation for map objects(now only in model)
============================================================================*/
#ifndef MAP_OBJECT_H
#define MAP_OBJECT_H

#include <DirectXMath.h>
#include "collision.h"

class MapObject
{
public:
    MapObject(const DirectX::XMFLOAT3& position) : m_position(position) {};
	virtual ~MapObject() = default;

    virtual void Update(float) {};
	virtual void Draw() const {};

    virtual HitData3D IsHit(const CollisionAABB&) const { return {}; }

protected :
	const DirectX::XMFLOAT3& GetPosition() const { return m_position; };
	void SetPosition(const DirectX::XMFLOAT3& position) { m_position = position; };

private:
    DirectX::XMFLOAT3 m_position{};

};

#endif
