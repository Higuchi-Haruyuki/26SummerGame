#include "UIPanel.h"
#include <algorithm>


void UIPanel::Update(float deltaTime)
{
	if (!m_isVisible) return;
	

	for (const auto& child : m_children)
	{
		child->Update(deltaTime);
	}

}

void UIPanel::Draw() const
{
	if (!m_isVisible) return;
	for (const auto& child : m_children)
	{
		if (!child) continue;
		if (!child->GetIsVisible()) continue;
		child->Draw();
	}
}

void UIPanel::SetVisible(bool isVisible)
{
	m_isVisible = isVisible;

	for (const auto& child : m_children)
	{
		child->SetVisible(isVisible);
	}

}

bool UIPanel::HitTest(const Vector & screenPos) const
{
	if (!m_isVisible) return false;
	if (!m_isHitTarget) return false;

	for (const auto& child : m_children)
	{
		if (child->HitTest(screenPos)) return true;
	}

	return false;
}

std::shared_ptr<Base_UIElement> UIPanel::HitTestTopMost(const Vector& screenPos)
{
	if (!m_isVisible) return nullptr;

	if (!m_isHitTarget) return nullptr;

	for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
	{
		const auto& child = *it;
		if (!child) continue;
		if (!child->GetIsVisible()) continue;

		if (auto childPanel = std::dynamic_pointer_cast<UIPanel>(child))
		{
			if (auto hit = childPanel->HitTestTopMost(screenPos)) return hit;
			continue;
		}

		if (child->HitTest(screenPos))return child;
	}
	return nullptr;
}

void UIPanel::RemoveChild(const std::shared_ptr<Base_UIElement>& child)
{
	m_children.erase(std::find(m_children.begin(), m_children.end(), child));
}

bool UIPanel::DispatchPointerDown(const Vector & screenPos)
{
	if (!m_isVisible) return false;

	//手前から判定する。
	for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
	{
		const auto& child = *it;
		if (!child->HitTest(screenPos)) continue;

		if (child->OnPointerDown(screenPos)) return true;
	}

	return false;
}

bool UIPanel::DispatchPointerUp(const Vector& screenPos)
{
	if (!m_isVisible) return false;

	//手前から判定する。
	for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
	{
		const auto& child = *it;
		if (!child->HitTest(screenPos)) continue;

		if (child->OnPointerUp(screenPos)) return true;
	}

	return false;
}

bool UIPanel::DispatchDragBegin(const Vector& screenPos)
{
	if (!m_isVisible) return false;

	//手前から判定する。
	for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
	{
		const auto& child = *it;
		if (!child->HitTest(screenPos)) continue;

		if (child->OnDragBegin(screenPos)) return true;
	}

	return false;
}

bool UIPanel::DispatchDragEnd(const Vector& screenPos)
{
	if (!m_isVisible) return false;

	//手前から判定する。
	for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
	{
		const auto& child = *it;
		if (!child->HitTest(screenPos)) continue;

		if (child->OnDragEnd(screenPos)) return true;
	}

	return false;
}

