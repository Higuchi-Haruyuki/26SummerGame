#include "UIManager.h"
#include "Base_UIElement.h"
#include <algorithm>
#include "UIPanel.h"
#include "ItemSlot.h"
#include "ItemStack.h"
#include "Debug.h"
#include "ItemManager.h"
#include "ItemType.h"
#include "ItemStackFactory.h"
#include "SoundManager.h"

namespace
{
	constexpr float kDragThreshould = 6.0f;
}

void UIManager::Update(float deltaTime)
{
	m_pointer.Update();

	const auto& orderedScreens = MakeOrderedScreen();

	const auto nowPos = m_pointer.GetScreenPosition();

	MouseLeftProcess(nowPos, orderedScreens);

	MouseRightProcess(nowPos, orderedScreens);

	MouseHoverCheck(nowPos,orderedScreens);

	for (const auto& [name, screen] : m_screens)
	{
		screen->Update(deltaTime);
	}

	Debug::Log(std::format("selectedIndex: {}", m_referenceIndex));

}

void UIManager::Draw() const
{

	for (const auto& [name, screen] : MakeOrderedScreen())
	{
		screen->Draw();
	}
}

Item UIManager::GetSelectedItemType() const
{
	const auto& safeItemSlot = m_referenceItemSlot.lock();
	if (!safeItemSlot) return Item::kNone;

	if (m_referenceIndex == -1) return Item::kNone;

	auto item = safeItemSlot->GetItem(m_referenceIndex);
	if (!item) return Item::kNone;

	return item->GetItemType();
}

std::shared_ptr<UIPanel> UIManager::GetOrCreateScreen(const std::string& screenName, UILayer layer)
{
	//スクリーンを取得する。
	const auto& screen = GetScreen(screenName);

	if (screen) return screen;
	//持ってないならあたらしくつくったものをかえす。
	return CreateScreen(screenName, layer);
}

void UIManager::ItemSelect(std::weak_ptr<ItemSlot> itemSlot, int idx)
{
	m_referenceItemSlot = itemSlot;
	m_referenceIndex = idx;
}

bool UIManager::MoveItem(std::weak_ptr<ItemSlot> toItemSlot, int toindex)
{
	if (!m_referenceItemSlot.lock()) return false;
	if (toindex == -1) return false;
	bool result = false;

	auto referenceItem = m_referenceItemSlot.lock()->GetItemOwnership(m_referenceIndex);
	//移動先にアイテムがないとき
	if (!toItemSlot.lock()->GetItem(toindex))
	{
		toItemSlot.lock()->AddItem(toindex, std::move(referenceItem));
		result = true;
	}
	else
	{
		auto toItemOwerShip = toItemSlot.lock()->GetItemOwnership(toindex);

		if (toItemOwerShip->GetItemType() != referenceItem->GetItemType())
		{
			std::swap(referenceItem, toItemOwerShip);

			m_referenceItemSlot.lock()->AddItem(m_referenceIndex,
				std::move(referenceItem));

			toItemSlot.lock()->AddItem(toindex,
				std::move(toItemOwerShip));

			result = true;
		}
		else
		{
			result = toItemOwerShip->MoveItemStack(referenceItem.get(), referenceItem->GetItemCount());

			if (referenceItem->GetItemCount() != 0)
			{
				m_referenceItemSlot.lock()->AddItem(m_referenceIndex,
					std::move(referenceItem));
			}

			toItemSlot.lock()->AddItem(toindex, std::move(toItemOwerShip));
		}
	}

	ResetDrag();
	return result;
}

void UIManager::SelectHalfItem(std::weak_ptr<ItemSlot> itemSlot, int idx)
{
	m_halfReferenceItemSlot = itemSlot;
	m_halfReferenceIndex = idx;
}

bool UIManager::MoveHalfItem(std::weak_ptr<ItemSlot> toItemSlot, int toindex)
{
	if (!m_halfReferenceItemSlot.lock()) return false;
	if (toindex == -1) return false;

	auto referenceItem = m_halfReferenceItemSlot.lock()->GetItem(m_halfReferenceIndex);
	auto toItem = toItemSlot.lock()->GetItem(toindex);

	auto itemCount = referenceItem->GetItemCount();

	auto moveCount = itemCount / 2;

	if (!moveCount)
	{
		ResetDrag();
		return false;
	}

	//移動先にアイテムがないとき
	if (!toItem)
	{

		toItemSlot.lock()->AddItem(toindex,
			ItemStackFactory::Make(referenceItem->GetItemType(), moveCount));

		referenceItem->MinusItemCount(moveCount);
	}
	//あるとき
	else
	{
		if (referenceItem->GetItemType() != toItem->GetItemType())
		{
			ResetDrag();
			return false;
		}
		auto successCount = toItem->AddItemCount(moveCount);

		auto remain = moveCount - successCount;

		referenceItem->MinusItemCount(moveCount - remain);
	}

	ResetDrag();
	return true;
}

void UIManager::RemoveScreen(const std::string& screenName)
{
	m_screens.erase(screenName);
	m_screenLayers.erase(screenName);
}


std::shared_ptr<UIPanel> UIManager::GetScreen(const std::string& screenName)
{
	auto it = m_screens.find(screenName);

	//もうすでにあるならそれを返す。
	if (it != m_screens.end()) return it->second;
	return nullptr;
}

std::shared_ptr<UIPanel> UIManager::CreateScreen(const std::string& screenName, UILayer layer)
{
	const auto& newScreen = std::make_shared<UIPanel>();
	m_screens.emplace(screenName, newScreen);
	m_screenLayers.emplace(screenName, layer);
	return newScreen;
}

void UIManager::ResetDrag()
{
	m_referenceItemSlot.reset();

	m_referenceIndex = -1;

	m_capturedElement.reset();

	m_isDragging = false;
}

void UIManager::ResetRightDrag()
{
	m_referenceItemSlot.reset();

	m_referenceIndex = -1;

	m_rightClickCapturedElement.reset();

	m_isRightDragging = false;
}

void UIManager::MouseHoverCheck(const Vector& nowPos, const std::vector<std::pair<std::string, std::shared_ptr<UIPanel>>>& orderedScreens)
{
	const auto& hitElement = FindHitElement(orderedScreens, nowPos);

	if (hitElement)
	{
		hitElement->OnHoverUI();
		m_isPointerOverUI = true;
	}
	else 
	{

		m_isPointerOverUI = false;
	}
}

void UIManager::MouseLeftProcess(const Vector& nowPos,
	const std::vector<std::pair<std::string, std::shared_ptr<UIPanel>>>& orderedScreens)
{
	//押した瞬間に最前面をキャプチャする。
	if (m_pointer.IsLeftButtonTrigger())
	{
		ResetDrag();
		m_pressScreenPos = nowPos;
		m_lastScreenPos = nowPos;

		const auto& hitElement = FindHitElement(orderedScreens, nowPos);

		if (hitElement)
		{
			m_capturedElement = hitElement;
		}

	}

	if (m_pointer.IsLeftButtonPressed())
	{
		SetIsDraggingByThreshould(nowPos);

		if (auto captured = m_capturedElement.lock())
		{
			if (m_isDragging)
			{
				captured->OnDrag(nowPos, nowPos - m_lastScreenPos);
			}

			m_lastScreenPos = nowPos;
		}
	}

	if (m_pointer.IsLeftButtonReleased())
	{
		if (auto captured = m_capturedElement.lock())
		{
			if (m_isDragging)
			{
				const auto& hitElement = FindHitElement(orderedScreens, nowPos);

				if (hitElement) hitElement->OnDrop(nowPos);

				captured->OnDragEnd(nowPos);
			}
			else
			{
				captured->OnClick(nowPos);
				SoundManager::GetInstance().Play(SoundId::kClickSe);
			}
		}

		ResetDrag();

	}
}

void UIManager::MouseRightProcess(const Vector& nowPos,
	const std::vector<std::pair<std::string, std::shared_ptr<UIPanel>>>& orderedScreens)
{
	const auto& hitElement = FindHitElement(orderedScreens, nowPos);

	//押した瞬間に最前面をキャプチャする。
	if (m_pointer.IsRightButtonTrigger())
	{
		ResetRightDrag();
		m_rightPressScreenPos = nowPos;
		m_rightLastScreenPos = nowPos;

		if (hitElement)
		{
			m_rightClickCapturedElement = hitElement;
		}

	}

	if (m_pointer.IsRightButtonPressed())
	{
		SetIsRightClickDraggingByThreshould(nowPos);

		if (auto captured = m_rightClickCapturedElement.lock())
		{
			if (m_isRightDragging)
			{
				captured->OnRightClickDrag(nowPos, nowPos - m_rightLastScreenPos);
			}

			m_rightLastScreenPos = nowPos;
		}
	}

	if (m_pointer.IsRightButtonReleased())
	{
		if (auto captured = m_rightClickCapturedElement.lock())
		{
			if (m_isRightDragging)
			{
				if (hitElement) hitElement->OnRightClickDrop(nowPos);

				captured->OnRightClickDragEnd(nowPos);
			}
			else
			{
				captured->OnRightClick(nowPos);
			}
		}

		ResetRightDrag();
	}
}

std::vector<std::pair<std::string, std::shared_ptr<UIPanel>>> UIManager::MakeOrderedScreen() const
{
	std::vector<std::pair<std::string, std::shared_ptr<UIPanel>>> orderedScreens(
		m_screens.begin(), m_screens.end());

	std::sort(orderedScreens.begin(), orderedScreens.end(),
		[this](const auto& lhs, const auto& rhs)
		{
			//名前からレイヤーを検索して並び替える。
			return m_screenLayers.at(lhs.first) > m_screenLayers.at(rhs.first);
		});

	return orderedScreens;
}

std::shared_ptr<Base_UIElement> UIManager::FindHitElement(
	const std::vector<std::pair<std::string, std::shared_ptr<UIPanel>>>& orderedScreens,
	const Vector& nowPos) const
{
	for (const auto& [name, screen] : orderedScreens)
	{
		if (auto hit = screen->HitTestTopMost(nowPos)) return hit;
	}

	return nullptr;
}

void UIManager::SetIsDraggingByThreshould(const Vector& nowPos)
{
	const auto safeCaptured = m_capturedElement.lock();

	if (!safeCaptured) return;

	if (m_isDragging) return;

	const Vector diff = nowPos - m_pressScreenPos;
	const float sqLen = diff.SqLength();

	if (sqLen > kDragThreshould * kDragThreshould)
	{
		m_isDragging = safeCaptured->OnDragBegin(nowPos);
	}
}

void UIManager::SetIsRightClickDraggingByThreshould(const Vector& nowPos)
{
	const auto safeCaptured = m_rightClickCapturedElement.lock();

	if (!safeCaptured) return;

	if (m_isRightDragging) return;

	const Vector diff = nowPos - m_rightPressScreenPos;
	const float sqLen = diff.SqLength();

	if (sqLen > kDragThreshould * kDragThreshould)
	{
		m_isRightDragging = safeCaptured->OnRightClickDragBegin(nowPos);
	}
}
