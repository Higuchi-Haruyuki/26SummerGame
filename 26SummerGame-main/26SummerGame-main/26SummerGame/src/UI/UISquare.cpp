#include "UISquare.h"
#include <DxLib.h>

UISquare::UISquare(const Vector& position, const Vector& size, unsigned int color, int alpha):
	Base_UIElement(position, size), 
	m_color(color), 
	m_alpha(alpha)
{

}

void UISquare::Draw() const
{
	if (!m_isVisible) return;

	float halfWidth = m_size.m_x * 0.5f;
	float halfHeight = m_size.m_y * 0.5f;

	if(m_alpha < 255) SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);

	DrawBox(
		static_cast<int>(m_position.m_x - halfWidth),
		static_cast<int>(m_position.m_y - halfHeight),
		static_cast<int>(m_position.m_x + halfWidth),
		static_cast<int>(m_position.m_y + halfHeight),
		m_color,
		true
	);

	if (m_alpha < 255) SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

bool UISquare::OnClick(const Vector& screenPos)
{
	m_onClick.Invoke();
	return true;
}

bool UISquare::OnDrop(const Vector& screenPos)
{
	m_onDrop.Invoke();
	return true;
}

bool UISquare::OnDragBegin(const Vector& screenPos)
{
	m_onDragBegin.Invoke();
	return true;
}

void UISquare::OnDrag(const Vector& screenPos, const Vector& delta)
{
	m_onDrag.Invoke(screenPos);
}

bool UISquare::OnDragEnd(const Vector& screenPos)
{
	m_onDragEnd.Invoke();
	return true;
}
