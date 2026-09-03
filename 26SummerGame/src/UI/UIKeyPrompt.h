#pragma once
#include "Base_UIElement.h"
#include "ResourceType.h"
#include <functional>

class ResourceManager;

enum class Color : unsigned int;

class UIKeyPrompt : public Base_UIElement
{
public:

    UIKeyPrompt(const Vector& position, const Vector& size, GraphicId resourceId, const std::string& text);

    virtual ~UIKeyPrompt() override = default;

    void Draw() const override;

	bool OnClick(const Vector& screenPos) override;

	// SETTER
	void SetText(const std::string & text) { m_text = text; }
	void SetResourceId(GraphicId resourceId) { m_resourceId = resourceId; }
	void SetFontID(FontId fontId) { m_fontID = fontId; }
	void SetBackGroundDefaultColor();
	void SetBackGroundColor(Color color);
	void SetKeyImageExRate(double exRate) { m_keyImageExRate = exRate; }
	void SetImagePositionOffset(Vector offset) { m_imagePositionOffset = offset; }

private:

	ResourceManager& m_resourceManager;

	GraphicId m_resourceId; // 表示するキーアイコン
	std::string m_text;         // 説明テキスト
	int m_innerOffset = 10;     // 内側の余白
	unsigned int m_color;
	FontId m_fontID;
	double m_keyImageExRate = 0.3f; //キー画像の拡大率
	Vector m_imagePositionOffset = { 15.0f,15.0f };

};

