#pragma once
#include "ResourceType.h"
#include "vector.h"
#include <string>

class UISquare;
class UIText;
class UIPanel;

/// <summary>
/// テキストラベルのUIをまとめて管理するクラス
/// </summary>
class UITextLabel
{
public:
	UITextLabel(std::weak_ptr<UIPanel> addPanel, const Vector& position, const std::string& text);
	virtual ~UITextLabel() = default;

	void SetVisible(bool isVisible);
	void SetPosition(const Vector& position);

	void SetText(const std::string& text);

	template<typename... Args>
	void SetText(const std::format_string<Args...>& fmt, Args&&... args)
	{
		const auto text = std::vformat(fmt.get(), std::make_format_args(std::forward<Args>(args)...));
		SetText(text);
	}

private:
	std::weak_ptr<UISquare> m_background;
	std::weak_ptr<UIText> m_text;

};

