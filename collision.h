/*============================================================================
Contents   :  [collision.h]
              
Author     : Chin Qing You
LastUpdate : 2026/07/01
-----------------------------------------------------------------------------

============================================================================*/
#ifndef COLLISION_H
#define COLLISION_H

#include <DirectXMath.h>

struct CollisionCircle
{
	DirectX::XMFLOAT2 position;
	float radius;
};

bool Collision_IsOverlap(const CollisionCircle& a, const CollisionCircle& b);

struct CollisionRect
{
	DirectX::XMFLOAT2 min;
	DirectX::XMFLOAT2 max;
};

bool Collision_IsOverlap(const CollisionRect& a, const CollisionRect& b);

struct HitData
{
	bool isHit;
	DirectX::XMFLOAT2 normal;
	CollisionRect rectangle;
};

struct CollisionCapsule
{
	DirectX::XMFLOAT2 start;
	DirectX::XMFLOAT2 end;
	float half_thickness;
};

bool Collision_IsOverlap(const CollisionCapsule& capsule, const CollisionCircle& circle);

HitData Collision_IsHit(const CollisionRect& a, const CollisionRect& b);

float Collision_PointSegmentDistanceSq(
	const DirectX::XMFLOAT2& point,
	const DirectX::XMFLOAT2& seg_start,
	const DirectX::XMFLOAT2& seg_end);

class CollisionAABB
{
private:
	float m_half_width{};
	float m_half_height{};
	float m_half_depth{};
	DirectX::XMFLOAT3 m_center{};

public:
	CollisionAABB() = default;
	CollisionAABB(float half_width, float half_height, float half_depth, const DirectX::XMFLOAT3& center)
		: m_half_width(half_width), m_half_height(half_height), m_half_depth(half_depth), m_center(center) {};

	void SetcCenterPosition(const DirectX::XMFLOAT3& center) { m_center = center; }
	const DirectX::XMFLOAT3& GetCenter() const { return m_center; }

	// Checks if a point is inside the AABB.
	// bool IsOverlap(const XMFLOAT3& point);

	bool IsOverlap(const CollisionAABB& target) const;
};

#endif
