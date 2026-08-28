#pragma once
#include "Base_UIElement.h"
#include "ResourceType.h"
#include <functional>

class ResourceManager;

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

private:

	ResourceManager& m_resourceManager;

	GraphicId m_resourceId; // 表示するキーアイコン
	std::string m_text;         // 説明テキスト
	int m_innerOffset = 10;     // 内側の余白(旧DrawMiniUIのoffset相当)
};

