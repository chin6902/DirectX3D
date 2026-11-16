/*============================================================================
Contents   :  [map_move_block.h]

Author     : Chin Qing You
LastUpdate : 2026/11/14
-----------------------------------------------------------------------------

============================================================================*/
#ifndef MAP_MOVE_BLOCK_H
#define MAP_MOVE_BLOCK_H

#include "map_block.h"

class MapMoveBlock : public MapBlock
{
public:
	MapMoveBlock(const DirectX::XMFLOAT3& position, int texture_id) : MapBlock(position, texture_id), m_BasePosition(position) {};

	void Update(float delta_time) override;

	const DirectX::XMFLOAT3& GetMoveDelta() const { return m_move_delta; }

private:
	float m_accumulated_time{ 0.0f };
	DirectX::XMFLOAT3 m_BasePosition{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_move_delta{};

};

#endif