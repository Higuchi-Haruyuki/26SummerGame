#include "UISquare.h"
#include <DxLib.h>

UISquare::UISquare(const Vector& position, const Vector& size, unsigned int color, int alpha):
	Base_UIElement(position, size), 
	m_color(color), 
	m_alpha(alpha)
{

}

void UISquare::Update(float deltaTime)
{
	m_isPressedLastFrame = m_isPressed;
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

bool UISquare::OnPointerDown(const Vector& screenPos)
{
	m_isPressed = true;
	return false;
}

bool UISquare::OnPointerUp(const Vector& screenPos)
{
	m_isPressed = false;
	return false;
}

bool UISquare::OnClick(const Vector& screenPos)
{
	if (m_onClick) m_onClick();
	return true;
}

bool UISquare::OnDrop(const Vector& screenPos)
{
	if (m_onDrop)
		m_onDrop();
	return true;
}

bool UISquare::OnDragBegin(const Vector& screenPos)
{
	if (m_onDragBegin)
		m_onDragBegin();
	return true;
}

void UISquare::OnDrag(const Vector& screenPos, const Vector& delta)
{
	const int size = 50;
	DrawBox(screenPos.m_x - size, screenPos.m_y - size, screenPos.m_y + size, screenPos.m_y + size,0xd3d3d3,true);
}
