/*============================================================================
Contents   :  [map_move_block.h]

Author     : Chin Qing You
LastUpdate : 2026/11/14
-----------------------------------------------------------------------------

============================================================================*/
#include "map_move_block.h"

void MapMoveBlock::Update(float delta_time)
{
	const DirectX::XMFLOAT3 old_position = GetPosition();

	m_accumulated_time += delta_time;

	DirectX::XMFLOAT3 newPosition = m_BasePosition;
	newPosition.x += sin(m_accumulated_time) * 2.0f;

	SetPosition(newPosition);

	m_move_delta = {
		newPosition.x - old_position.x,
		newPosition.y - old_position.y,
		newPosition.z - old_position.z };
}
