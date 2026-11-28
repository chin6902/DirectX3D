/*============================================================================
Contents   :  [flipbook.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/27
-----------------------------------------------------------------------------

============================================================================*/
#include "flipbook.h"

using namespace DirectX;

FlipBookPlayer::FlipBookPlayer(const FlipBookSheet* sheet, AnimPlayMode mode)
	: m_pSheet(sheet), m_Mode(mode){}

void FlipBookPlayer::Restart()
{
	m_Frame = 0;
	m_Time = 0.0f;
	m_Finished = false;
}

void FlipBookPlayer::Update(float delta_time)
{
	if (!m_pSheet || m_Finished || m_Mode == AnimPlayMode::FREEZE) { return; }
	if (m_pSheet->frame_time <= 0.0f) { return; }   

	m_Time += delta_time;

	while (m_Time >= m_pSheet->frame_time)
	{
		m_Time -= m_pSheet->frame_time;
		m_Frame++;

		if (m_Frame < m_pSheet->frame_count) { continue; }   

		if (m_Mode == AnimPlayMode::LOOP)
		{
			m_Frame = 0;
		}
		else // ONE_SHOT
		{
			m_Frame = m_pSheet->frame_count - 1;
			m_Time = 0.0f;
			m_Finished = true;
			break;
		}
	}
}

XMFLOAT4 FlipBookPlayer::GetUVRect() const
{
	if (!m_pSheet || m_pSheet->columns <= 0) { return { 0.0f, 0.0f, 1.0f, 1.0f }; }

	const float tex_w = static_cast<float>(Texture_GetWidth(m_pSheet->texture_id));
	const float tex_h = static_cast<float>(Texture_GetHeight(m_pSheet->texture_id));
	if (tex_w == 0.0f || tex_h == 0.0f) { return { 0.0f, 0.0f, 1.0f, 1.0f }; }

	const int col = m_Frame % m_pSheet->columns;
	const int row = m_Frame / m_pSheet->columns;

	return {
		col * m_pSheet->frame_width / tex_w,
		row * m_pSheet->frame_height / tex_h,
		m_pSheet->frame_width / tex_w,
		m_pSheet->frame_height / tex_h,
	};
}

void FlipBookPlayer::DrawBillboard(const XMFLOAT3& position, const BillboardDrawParams& params) const
{
	if (!m_pSheet) { return; }

	BillboardDrawParams p = params;
	p.uv_rect = GetUVRect();
	Billboard_Draw(m_pSheet->texture_id, position, p);
}
