#pragma once
#include "Base_UIElement.h"
#include <string>
#include <algorithm>
#include <functional>
#include "Event.h"


class UISquare :
    public Base_UIElement
{
public:
	UISquare(const Vector& position, const Vector& size, unsigned int color, int alpha = 255);
	virtual ~UISquare() = default;

	void Draw() const override;

	bool OnClick(const Vector& screenPos) override;
	bool OnDrop(const Vector& screenPos) override;

	bool OnDragBegin(const Vector& screenPos) override;
	void OnDrag(const Vector& screenPos, const Vector& delta) override;
	bool OnDragEnd(const Vector& screenPos) override;

	//SETTER
	void SetColor(unsigned int color) { m_color = color; }
	void SetAlpha(int alpha) 
	{ 
		m_alpha = alpha; 
		m_alpha = std::clamp(m_alpha, 0, 255);
	}

private:

	unsigned int m_color = 0xFFFFFFFF; // 白色

	int m_alpha = 255; // 透明度(0-255)

};

