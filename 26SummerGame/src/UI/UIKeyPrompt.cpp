#include "UIKeyPrompt.h"
#include "ResourceManager.h"
#include <DxLib.h>
#include "DrawText.h"
#include "Color.h"

namespace
{
	constexpr unsigned int kBoxColor = static_cast<unsigned int>(Color::kMainColor);
	constexpr unsigned int kCharColor = static_cast<unsigned int>(Color::kMainCharColor);
	constexpr unsigned int kCharEdgeColor = static_cast<unsigned int>(Color::kSubCharColor);
	constexpr FontId kDefaultFontId = FontId::kFactoryUIText;
}


UIKeyPrompt::UIKeyPrompt(const Vector& position, const Vector& size, GraphicId resourceId, const std::string& text) :
	Base_UIElement(position, size),
	m_resourceManager(ResourceManager::GetInstance()),
	m_resourceId(resourceId),
	m_text(text),
	m_color(kBoxColor),
	m_fontID(kDefaultFontId)
{
}

void UIKeyPrompt::Draw() const
{
	int halfWidth = m_size.m_x * 0.5f;
	int halfHeight = m_size.m_y * 0.5f;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	DrawBox(
		static_cast<int>(m_position.m_x - halfWidth), static_cast<int>(m_position.m_y - halfHeight),
		static_cast<int>(m_position.m_x + halfWidth), static_cast<int>(m_position.m_y + halfHeight),
		m_color, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 100);

	text::DrawRightCenterFormatStringToHandleWithEdge(
		static_cast<int>(m_position.m_x + halfWidth - m_innerOffset),
		static_cast<int>(m_position.m_y),
		kCharColor, kCharEdgeColor,
		m_resourceManager.GetHandle(m_fontID),
		"{}", m_text);

	DrawRotaGraph(
		static_cast<int>(m_position.m_x - halfWidth + m_imagePositionOffset.m_x + m_innerOffset),
		static_cast<int>(m_position.m_y - halfHeight + m_imagePositionOffset.m_y + m_innerOffset),
		m_keyImageExRate, 0, m_resourceManager.GetHandle(m_resourceId), true);
}

bool UIKeyPrompt::OnClick(const Vector& screenPos)
{
	m_onClick.Invoke();
	return true;
}

void UIKeyPrompt::SetBackGroundDefaultColor()
{
	m_color = kBoxColor;
}

void UIKeyPrompt::SetBackGroundColor(Color color)
{
	m_color = static_cast<unsigned int>(color);
}

