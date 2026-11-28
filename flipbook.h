/*============================================================================
Contents   :  [flipbook.h]
              
Author     : Chin Qing You
LastUpdate : 2026/11/27
-----------------------------------------------------------------------------

============================================================================*/
#ifndef FLIPBOOK_H
#define FLIPBOOK_H

#include <DirectXMath.h>
#include "billboard.h"
#include "texture.h"

enum class AnimPlayMode
{
	LOOP,
	ONE_SHOT,
	FREEZE,
};

struct FlipBookSheet
{
	int   texture_id{ TEXTURE_INVALID_ID };
	int   frame_width{ 0 };      // pixels
	int   frame_height{ 0 };
	int   frame_count{ 0 };      // total frames
	int   columns{ 1 };          // frames per row
	float frame_time{ 0.1f };    // seconds per frame
};

class FlipBookPlayer
{
public:
	FlipBookPlayer() = default;
	explicit FlipBookPlayer(const FlipBookSheet* sheet, AnimPlayMode mode = AnimPlayMode::LOOP);

	void Update(float delta_time);
	void Restart();

	bool IsFinished() const { return m_Finished; }

	DirectX::XMFLOAT4 GetUVRect() const;
	void DrawBillboard(const DirectX::XMFLOAT3& position, const BillboardDrawParams& params = {}) const;

private:
	const FlipBookSheet* m_pSheet{ nullptr };   // not owned, just points at shared data
	int          m_Frame{ 0 };
	float        m_Time{ 0.0f };
	AnimPlayMode m_Mode{ AnimPlayMode::LOOP };
	bool         m_Finished{ false };
};

#endif
