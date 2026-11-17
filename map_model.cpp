/*============================================================================
Contents   :  [map_model.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/12
-----------------------------------------------------------------------------

============================================================================*/
#include "map_model.h"
#include "collision3d_debug.h"

using namespace DirectX;

void MapModel::Draw() const
{
	XMMATRIX s = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	XMMATRIX r = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
	XMMATRIX t = XMMatrixTranslationFromVector(XMLoadFloat3(&GetPosition()));

#ifdef _DEBUG
	Collision3D_Debug_Add(GetWorldAABB());
#endif

	ModelDraw(m_pModel, s * r * t);
}

HitData3D MapModel::IsHit(const CollisionAABB & aabb) const
{
	return aabb.IsHit(GetWorldAABB());
}

CollisionAABB MapModel::GetWorldAABB() const
{
	const CollisionAABB& model_aabb = m_pModel->AABB;

	// Model-space center offset, scaled and rotated the same way as Draw
	XMMATRIX s = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	XMMATRIX r = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
	XMVECTOR offset = XMVector3TransformCoord(XMLoadFloat3(&model_aabb.GetCenter()), s * r);

	XMFLOAT3 center;
	XMStoreFloat3(&center, XMLoadFloat3(&GetPosition()) + offset);

	return CollisionAABB{
		model_aabb.GetHalfWidth() * m_scale.x,
		model_aabb.GetHalfHeight() * m_scale.y,
		model_aabb.GetHalfDepth() * m_scale.z,
		center };
}
