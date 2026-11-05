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

class HitData3D;

class CollisionAABB
{
public:
	CollisionAABB() = default;
	CollisionAABB(float half_width, float half_height, float half_depth, const DirectX::XMFLOAT3& center)
		: m_half_width(half_width), m_half_height(half_height), m_half_depth(half_depth), m_center(center) {};

	void SetCenterPosition(const DirectX::XMFLOAT3& center) { m_center = center; }
	const DirectX::XMFLOAT3& GetCenter() const { return m_center; }

	float GetHalfWidth() const { return m_half_width; }
	float GetHalfHeight() const { return m_half_height; }
	float GetHalfDepth() const { return m_half_depth; }

	bool IsOverlap(const CollisionAABB& target) const;
	HitData3D IsHit(const CollisionAABB& target) const;

private:
	float m_half_width{};
	float m_half_height{};
	float m_half_depth{};
	DirectX::XMFLOAT3 m_center{};

};

class HitData3D
{
public:
	HitData3D() = default;
	HitData3D(bool isHit) : isHit(isHit) {};

	bool IsHit() const { return isHit; };
	void SetNormal(const DirectX::XMFLOAT3& normal) { m_normal = normal; };
	const DirectX::XMFLOAT3& GetNormal() const { return m_normal; };
	void SetAABB(const CollisionAABB& aabb) { m_aabb = aabb; };
	const CollisionAABB& GetAABB() const { return m_aabb; };
	void  SetDepth(float depth) { m_depth = depth; }
	float GetDepth() const { return m_depth; }

private:
	bool isHit{ false };
	float m_depth{};
	DirectX::XMFLOAT3 m_normal{};
	CollisionAABB m_aabb{};

};

#endif
