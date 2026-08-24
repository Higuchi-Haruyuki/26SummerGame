#include "UIButton.h"
#include "DrawText.h"
#include "ResourceType.h"
#include "ResourceManager.h"
#include <format>
#include <string>

UIButton::UIButton(const Vector& position, const Vector& size, const std::string& label):
	Base_UIElement(position,size),
	m_label(label)

{
}

void UIButton::Draw() const
{
	// ホバー/押下状態によって色を変える(具体的な配色・フォントは既存のResourceManager等に合わせて調整する)
	const unsigned int fillColor = m_isPressed ? 0x555555 : (m_isHovered ? 0x333333 : 0x000000);
	const unsigned int edgeColor = m_isPressed ? 0x555555 : (m_isHovered ? 0x333333 : 0x000000);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	DrawBox(
		static_cast<int>(m_position.m_x), static_cast<int>(m_position.m_y),
		static_cast<int>(m_position.m_x + m_size.m_x), static_cast<int>(m_position.m_y + m_size.m_y),
		fillColor, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 100);

	// TODO: 既存のPlayerUI::DrawMiniUIと同様にDrawCenter経由でフォントハンドルを指定して描画する
	DrawFormatString(
		static_cast<int>(m_position.m_x + 10), static_cast<int>(m_position.m_y + m_size.m_y / 2 - 8),
		0xffffff, "%s", m_label.c_str());

	text::DrawCenterFormatStringToHandleWithEdge(
		static_cast<int>(m_position.m_x), static_cast<int>(m_position.m_y),
		fillColor, edgeColor,
		ResourceManager::GetInstance().GetHandle(FontId::kFactoryUIText),
		"%s", m_label.c_str());

}

bool UIButton::OnPointerDown(const Vector& screenPos)
{
	m_isPressed = true;
	return true;
}

bool UIButton::OnPointerUp(const Vector& screenPos)
{
	if (!m_isPressed) return false;

	m_isPressed = false;

	if (m_onClick) m_onClick();

	return true;
}
