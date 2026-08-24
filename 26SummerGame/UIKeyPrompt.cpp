#include "UIKeyPrompt.h"
#include "ResourceManager.h"
#include <DxLib.h>
#include "DrawText.h"


UIKeyPrompt::UIKeyPrompt(const Vector& position, const Vector& size, GraphicId resourceId, const std::string& text) :
	Base_UIElement(position, size),
	m_resourceManager(ResourceManager::GetInstance()),
	m_resourceId(resourceId),
	m_text(text)
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
		0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 100);

	text::DrawRightCenterFormatStringToHandleWithEdge(
		static_cast<int>(m_position.m_x + halfWidth - m_innerOffset),
		static_cast<int>(m_position.m_y),
		0x000000, 0xffffff,
		m_resourceManager.GetHandle(FontId::kFactoryUIText),
		"{}", m_text);

	DrawRotaGraph(
		static_cast<int>(m_position.m_x - halfWidth + 15 + m_innerOffset),
		static_cast<int>(m_position.m_y - halfHeight + 15 + m_innerOffset),
		0.3, 0, m_resourceManager.GetHandle(m_resourceId), true);
}

bool UIKeyPrompt::OnClick(const Vector& screenPos)
{
	if (m_onClick) m_onClick();
	return true;
}

