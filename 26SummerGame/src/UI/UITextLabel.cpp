#include "UITextLabel.h"
#include "UIText.h"
#include "UISquare.h"
#include "Color.h"
#include "UIFactory.h"
#include "UIPanel.h"

namespace
{
	constexpr unsigned int kDefaultColor = static_cast<unsigned int>(Color::kMainColor);
	const Vector kDefaultSize = { 120,25 };
	constexpr FontId kFontId = FontId::kFactoryUITextMini;
	constexpr int kAlpha = 150;
}

UITextLabel::UITextLabel(std::weak_ptr<UIPanel> addPanel, const Vector& position, const std::string& text)
{
	m_background = UIFactory::MakeUIToPanel<UISquare>(addPanel, position, kDefaultSize, kDefaultColor, kAlpha);
	m_text = UIFactory::MakeUIToPanel<UIText>(addPanel,
		TextArgs	
		{
			position,
			TextPivot::Center,
			kFontId,
			text
		});
}

void UITextLabel::SetVisible(bool isVisible)
{
	if (auto safeBackground = m_background.lock())
	{
		safeBackground->SetVisible(isVisible);
	}

	if (auto safeText = m_text.lock())
	{
		safeText->SetVisible(isVisible);
	}
}

void UITextLabel::SetPosition(const Vector& position)
{
	if (auto safeBackground = m_background.lock())
	{
		safeBackground->SetPosition(position);
	}

	if (auto safeText = m_text.lock())
	{
		safeText->SetPosition(position);
	}
}

void UITextLabel::SetText(const std::string& text)
{
	if (auto safeText = m_text.lock())
	{
		safeText->SetText(text);
	}
}
