#pragma once
#include "Base_UIElement.h"
#include <string>
#include <algorithm>
#include <functional>

class UISquare :
    public Base_UIElement
{
public:
	UISquare(const Vector& position, const Vector& size, unsigned int color, int alpha = 255);
	virtual ~UISquare() = default;

	void Update(float deltaTime) override;
	void Draw() const override;

	bool OnPointerDown(const Vector& screenPos) override;
	bool OnPointerUp(const Vector& screenPos) override;
	void OnPointerEnter() override { m_isHovered = true; }
	void OnPointerExit() override { m_isHovered = false; }

	bool OnClick(const Vector& screenPos) override;
	bool OnDrop(const Vector& screenPos) override;

	bool OnDragBegin(const Vector& screenPos) override;
	void OnDrag(const Vector& screenPos, const Vector& delta) override;

	//SETTER
	void SetColor(unsigned int color) { m_color = color; }
	void SetAlpha(int alpha) 
	{ 
		m_alpha = alpha; 
		m_alpha = std::clamp(m_alpha, 0, 255);
	}

	/// <summary>
	/// 押され始めたときに呼び出される
	/// </summary>
	/// <param name="onBegin"></param>
	void SetOnDragBegin(const std::function<void()>& onBegin) { m_onDragBegin = onBegin; }

	/// <summary>
	/// はなされたときによびだされる
	/// </summary>
	/// <param name="onBegin"></param>
	void SetOnDrop(const std::function<void()>& onDrop) { m_onDrop = onDrop; }

	void SetOnClick(const std::function<void()>& onClick) { m_onClick = onClick; }

private:
	bool IsPressed() const { return m_isPressed; }

	bool IsTrigger() const { return !m_isPressedLastFrame && m_isPressed; }

	bool IsReleased() const { return m_isPressedLastFrame && !m_isPressed; }

private:

	unsigned int m_color = 0xFFFFFFFF; // 白色

	int m_alpha = 255; // 透明度(0-255)

	std::function<void()> m_onDragBegin;
	std::function<void()> m_onDrop;

	std::function<void()> m_onClick;

	bool m_isHovered = false;  // カーソルが乗っているか(見た目のハイライト用)
	bool m_isPressed = false;  // このボタンの上で押下されたか(離した瞬間の判定に使う)
	bool m_isPressedLastFrame = false;
};

