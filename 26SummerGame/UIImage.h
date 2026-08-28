#pragma once
#include "Base_UIElement.h"
#include <algorithm>
#include <string>
#include <functional>
#include "Event.h"

class ResourceManager;

enum class GraphicId;

class UIImage :
    public Base_UIElement
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="position">位置</param>
	/// <param name="size">ここで指定した大きさに合わせて描画される。</param>
	/// <param name="imageResourceName">ResourceManagerで管理される画像リソースの名前</param>
	/// <param name="alpha">透明度 (0-255)</param>
	UIImage(const Vector& position, const Vector& size, GraphicId imageResourceName, int alpha = 255);
	virtual ~UIImage() = default;

	void Draw() const override;

	bool OnClick(const Vector& screenPos) override;
	bool OnDrop(const Vector& screenPos) override;

	bool OnDragBegin(const Vector& screenPos) override;
	void OnDrag(const Vector& screenPos, const Vector& delta) override;
	bool OnDragEnd(const Vector& screenPos) override;

	//SETTER

	void SetGraphicID(GraphicId graphicId) { m_imageGraphicId = graphicId; }

	void SetAlpha(int alpha)
	{
		m_alpha = alpha;
		m_alpha = std::clamp(m_alpha, 0, 255);
	}

private:

	ResourceManager& m_resourceManager;

	GraphicId m_imageGraphicId;

	int m_alpha = 255; // 透明度(0-255)
};

