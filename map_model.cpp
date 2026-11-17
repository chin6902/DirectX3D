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

	CollisionAABB scaled_aabb{
		m_pModel->AABB.GetHalfWidth() * m_scale.x,
		m_pModel->AABB.GetHalfHeight() * m_scale.y,
		m_pModel->AABB.GetHalfDepth() * m_scale.z,
		GetPosition()
	};

	Collision3D_Debug_Add(scaled_aabb);
#endif

	ModelDraw(m_pModel, s * r * t);
}

HitData3D MapModel::IsHit(const CollisionAABB & aabb) const
{
	CollisionAABB scaled_aabb{
		m_pModel->AABB.GetHalfWidth() * m_scale.x,
		m_pModel->AABB.GetHalfHeight() * m_scale.y,
		m_pModel->AABB.GetHalfDepth() * m_scale.z,
		GetPosition()
	};

	return m_pModel->AABB.IsHit(scaled_aabb);
}
