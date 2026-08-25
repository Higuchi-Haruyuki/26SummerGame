#include "UIProgressBar.h"

UIProgressBar::UIProgressBar(const Vector& position, const Vector& size, unsigned int color, int alpha):
	Base_UIElement(position, size),
	m_color(color),
	m_alpha(alpha)
{

}

void UIProgressBar::Draw() const
{
	if (!m_isVisible) return;

	const float halfWidth = m_size.m_x * 0.5f;
	const float halfHeight = m_size.m_y * 0.5f;

	const int leftUpX = m_position.m_x - halfWidth;
	const int leftUpY = m_position.m_y - halfHeight;
	const int rightDownX = leftUpX + halfWidth * 2.0f * m_progress;
	const int rightDownY = m_position.m_y + halfHeight;

	if (m_alpha < 255) SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);

	DrawBox(
		leftUpX,leftUpY,rightDownX,rightDownY,
		m_color,
		true
	);

	if (m_alpha < 255) SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}
