#pragma once
#include "Base_UIElement.h"
#include "ResourceType.h"
#include <string>
#include <algorithm>
#include "Color.h"

class ResourceManager;

/// <summary>
/// テキストボックスの基準点設定の列挙体
/// </summary>
enum class TextPivot
{
	Center = 0,
	CenterBottom,
	CenterTop,
	RightBottom,
	RightCenter,
};

/// <summary>
/// UITextのコンストラクタの引数用の構造体
/// </summary>
struct TextArgs
{
	//描画する座標
	Vector Position;
	//文字列の座標上の基準点
	TextPivot Pivot = TextPivot::Center;
	//使用するフォント
	FontId FontHandleName = FontId::kFactoryUIText;
	//テキスト
	std::string Text = "";
	//文字列の色
	unsigned int MainColor = static_cast<unsigned int>(Color::kMainCharColor);
	//文字列の縁色
	unsigned int EdgeColor = static_cast<unsigned int>(Color::kSubCharColor);
	//透明度
	unsigned int Alpha = 255;

};

class UIText :
	public Base_UIElement
{
public:
	UIText(const TextArgs& textArgs);

	virtual ~UIText() = default;

	void Draw() const override;

	//SETTER
	void SetText(const std::string& text) { m_text = text; }

	template <typename... Args>
	void SetText(const std::format_string<Args...>& format, Args&&... args)
	{
		std::string text = std::vformat(format.get(), std::make_format_args(args...));
		SetText(text);
	}

	void SetPivot(TextPivot pivot) { m_pivot = pivot; }


	void SetMainColor(unsigned int color) { m_mainColor = color; }

	void SetEdgeColor(unsigned int color) { m_edgeColor = color; }

	void SetAlpha(int alpha)
	{
		m_alpha = alpha;
		m_alpha = std::clamp(m_alpha, 0, 255);
	}

private:

	void DrawCenter(int x, int y, int handle) const;

	void DrawCenterTop(int x, int y, int handle) const;

	void DrawCenterBottom(int x, int y, int handle) const;

	void DrawRightBottom(int x, int y, int handle) const;

	void DrawRightCenter(int x, int y, int handle) const;
private:

	ResourceManager& m_resourceManager;

	TextPivot m_pivot;

	FontId m_fontHandleName;

	std::string m_text;

	unsigned int m_mainColor;

	//縁の色
	unsigned int m_edgeColor;

	int m_alpha; // 透明度(0-255)



};

