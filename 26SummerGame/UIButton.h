#pragma once
#include "Base_UIElement.h"
#include <functional>


class UIButton : public Base_UIElement
{
public:
	UIButton(const Vector& position, const Vector& size, const std::string& label);
	virtual ~UIButton() = default;

	void Draw() const override;

	bool OnPointerDown(const Vector& screenPos) override;
	bool OnPointerUp(const Vector& screenPos) override;
	void OnPointerEnter() override { m_isHovered = true; }
	void OnPointerExit() override { m_isHovered = false; }

	//SETTER
	void SetLabel(const std::string& label) { m_label = label; }
	void SetOnClick(const std::function<void()>& onClick) { m_onClick = onClick; }

private:
	std::string m_label;
	std::function<void()> m_onClick;

	bool m_isHovered = false;  // カーソルが乗っているか(見た目のハイライト用)
	bool m_isPressed = false;  // このボタンの上で押下されたか(離した瞬間の判定に使う)

};

