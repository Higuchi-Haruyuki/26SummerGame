#include "UIText.h"
#include "DrawText.h"
#include "ResourceManager.h"
#include <cassert>


UIText::UIText(const TextArgs& textArgs):
	Base_UIElement(textArgs.Position, {0,0,0}),
	m_pivot(textArgs.Pivot),
	m_fontHandleName(textArgs.FontHandleName),
	m_text(textArgs.Text),
	m_mainColor(textArgs.MainColor),
	m_edgeColor(textArgs.EdgeColor),
	m_alpha(textArgs.Alpha),
	m_resourceManager(ResourceManager::GetInstance())
{}

void UIText::Draw() const
{
	int x = static_cast<int>(m_position.m_x);
	int y = static_cast<int>(m_position.m_y);

	int handle = m_resourceManager.GetHandle(m_fontHandleName);

	if (m_alpha < 255) SetDrawBlendMode(DX_BLENDMODE_ALPHA_X4, m_alpha);


	switch (m_pivot)
	{
	case TextPivot::Center:
		DrawCenter(x,y,handle);
		break;
	case TextPivot::CenterTop:
		DrawCenterTop(x, y, handle);
		break;
	case TextPivot::CenterBottom:
		DrawCenterBottom(x, y, handle);
		break;
	case TextPivot::RightBottom:
		DrawRightBottom(x, y, handle);
		break;
	case TextPivot::RightCenter:
		DrawRightCenter(x, y, handle);
		break;
	default:
		break;
	}

	if(m_alpha < 255) SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}

void UIText::DrawCenter(int x, int y, int handle) const
{
	if (handle == -1)
		text::DrawCenterFormatString(x, y, m_mainColor, "{}",m_text);
	else if (m_edgeColor == -1)
		text::DrawCenterFormatStringToHandle(x, y, m_mainColor, handle, "{}", m_text);
	else if (handle != -1 && m_edgeColor != -1)
		text::DrawCenterFormatStringToHandleWithEdge(x, y, m_mainColor, m_edgeColor, handle, "{}", m_text);
	else
		assert(false && "無効なUIのテキスト指定です");
}

void UIText::DrawCenterTop(int x, int y, int handle) const
{
	if (handle == -1 || m_edgeColor == -1)
	{
		assert(false && "無効なUIのテキスト指定です");
		return;
	}
	text::DrawCenterTopFormatStringToHandleWithEdge(x, y, m_mainColor, m_edgeColor, handle, "{}", m_text);
}

void UIText::DrawCenterBottom(int x, int y, int handle) const
{
	if (handle == -1 || m_edgeColor == -1)
	{
		assert(false && "無効なUIのテキスト指定です");
		return;
	}
	text::DrawCenterBottomFormatStringToHandleWithEdge(x, y, m_mainColor, m_edgeColor, handle, "{}", m_text);
}

void UIText::DrawRightBottom(int x, int y, int handle) const
{
	if (handle == -1 || m_edgeColor == -1)
	{
		assert(false && "無効なUIのテキスト指定です");
		return;
	}
	text::DrawRightBottomFormatStringToHandleWithEdge(x, y, m_mainColor, m_edgeColor, handle, "{}", m_text);

}

void UIText::DrawRightCenter(int x, int y, int handle) const
{
	if (handle == -1 || m_edgeColor == -1)
	{
		assert(false && "無効なUIのテキスト指定です");
		return;
	}
	text::DrawRightCenterFormatStringToHandleWithEdge(x, y, m_mainColor, m_edgeColor, handle, "{}", m_text);
}
