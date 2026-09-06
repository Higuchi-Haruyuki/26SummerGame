#include "ItemBarUI.h"

#include "Color.h"
#include "Game.h"
#include "ItemSlot.h"
#include "ItemStack.h"
#include "ItemType.h"
#include "ItemManager.h"
#include "PlayerItem.h"
#include "ResourceType.h"
#include "UIFactory.h"
#include "UIImage.h"
#include "UIItemBox.h"
#include "UIManager.h"
#include "UIPanel.h"
#include "UISquare.h"
#include "UIText.h"
#include "vector.h"
#include "Timer.h"

namespace
{
	/*アイテムバーの定数*/
	const Vector kItemBarPos =
	{
		Game::kDisplaySize.m_x / 2,
		Game::kDisplaySize.m_y / 8 * 7 - 50
	};
	const Vector kItemBarSize = { 800,80 };
	constexpr unsigned int kItemBarColor = static_cast<unsigned int>(Color::kMainColor);
	constexpr int kItemBarAlpha = 100;

	/*アイテムバーのアイテム表示に関する定数*/
	constexpr int kItemBarItemCount = 10;
	constexpr int kItemBarSlotOffsetX = 10;
	const Vector kItemBarItemSlotSize = { (kItemBarSize.m_x - 100) / kItemBarItemCount , kItemBarSize.m_y - 10 };

	constexpr unsigned int kItemBarBoxHighlight = static_cast<unsigned int>(Color::kMainAccentColor);
	//アイテム取得UI
	const Vector kItemGetUIPos = Vector{ 25,Game::kDisplaySize.m_y * 0.5f };
	constexpr Second kItemGetUIExistTime = 3.0f;
	const Vector kItemGetUIImageSize = Vector{ 50,50 };
	const Vector kItemGetUITextOffset = Vector{ 30.0f };
}

ItemBarUI::ItemBarUI(std::weak_ptr<Object> parentObject) :
	Component(parentObject),
	m_uiManager(UIManager::GetInstance())
{}

void ItemBarUI::Init()
{
	Component::Init();
	m_playerItem = GetComponent<PlayerItem>();
	m_itemBarUIPanel = m_uiManager.GetOrCreateScreen("itemBar", UILayer::Hud);

	//アイテムバーの背景部分
	UIFactory::MakeUIToPanel<UISquare>(m_itemBarUIPanel, kItemBarPos, kItemBarSize, kItemBarColor, kItemBarAlpha);

	/*アイテムバーのアイテムスロットの部分*/
	InitItemBarItemSlot();

	/*アイテム取得のUI*/
	InitItemGetUI();
}

void ItemBarUI::InitItemGetUI()
{
	auto& [image, text] = m_getItemUI;

	image = UIFactory::MakeUIToPanel<UIImage>(m_itemBarUIPanel,
		kItemGetUIPos, kItemGetUIImageSize, GraphicId::kNone);

	text = UIFactory::MakeUIToPanel<UIText>(m_itemBarUIPanel,
		TextArgs{
			kItemGetUIPos + kItemGetUITextOffset,
			TextPivot::LeftCenter
		});

	// 購読先がUIより長く生存しても、破棄済みのUIを参照しない。
	const std::weak_ptr<ItemBarUI> weakSelf =
		std::static_pointer_cast<ItemBarUI>(shared_from_this());
	image.lock()->SetTimerDuration(kItemGetUIExistTime);
	image.lock()->SubscribeOnTimerFinished([weakSelf]()
		{
			const auto self = weakSelf.lock();
			if (!self || !self->m_isEnable) return;
			auto& [image, text] = self->m_getItemUI;
			self->m_lastGetItem = std::make_pair(Item::kNone,0);
			image.lock()->SetVisible(false);
			text.lock()->SetVisible(false);
		});
	m_playerItem.lock()->SubscribeOnAddItem([weakSelf](Item item, int count)
		{
			const auto self = weakSelf.lock();
			if (!self || !self->m_isEnable) return;
			auto& [image, text] = self->m_getItemUI;
			auto& [lastItem, lastCount] = self->m_lastGetItem;

			if (lastItem != item)
			{
				lastCount = count;
				lastItem = item;
			}
			else
			{
				lastCount += count;
			}

			image.lock()->ResetTimer();
			image.lock()->SetVisible(true);
			image.lock()->SetGraphicID(ItemTable::GetGraphicID(item));
			text.lock()->SetVisible(true);
			text.lock()->SetText("{}: x{}", ItemTable::ItemTypeToDisplayName(item), lastCount);
		}
	);

}

void ItemBarUI::InitItemBarItemSlot()
{
	for (int i = 0; i < kItemBarItemCount; i++)
	{
		const auto& itemBox = std::make_shared<UIItemBox>(m_itemBarUIPanel, CalcItemBarItemSlotPos(i), kItemBarItemSlotSize);
		int index = i;

		itemBox->SetOnClickEvent(
			[this, index]
			{
				m_playerItem.lock()->SetItemBarChoiceIndex(index);
			}
		);
		itemBox->SetOnDragBeginEvent(
			[this, index]()
			{
				m_uiManager.ItemSelect(m_playerItem.lock()->GetItemBar().lock(), index);
			}
		);
		itemBox->SetOnDropEvent(
			[this, index]
			{
				m_uiManager.MoveItem(m_playerItem.lock()->GetItemBar(),index);
			}
		);
		itemBox->SetOnSelectHalfItem(m_playerItem.lock()->GetItemBar(), index);
		itemBox->SetOnMoveHalfItem(m_playerItem.lock()->GetItemBar(), index);

		//配列に追加
		m_itemBarBoxes.push_back(itemBox);
	}
}

Vector ItemBarUI::CalcItemBarItemSlotPos(int index) const
{
	//アイテムバーの左端の座標 (yは中心)
	const Vector kItemBarLeftEdge = kItemBarPos - Vector{ kItemBarSize.m_x * 0.5f };
	//アイテムバーのアイテムスロットの1つ目の左端の座標((kItemBarLeftEdge) + (スロット間のオフセット)の値)
	const Vector kItemSlotLeftEdgeStartPos = kItemBarLeftEdge + Vector{ kItemBarSlotOffsetX };
	//i個目のアイテムバーのアイテムスロットの左端の座標
	const Vector kItemSlotLeftEdgePos = kItemSlotLeftEdgeStartPos + Vector{ (kItemBarItemSlotSize.m_x + kItemBarSlotOffsetX) * index };
	//i個目のアイテムバーのアイテムスロットの中心座標
	const Vector kItemSlotCenterPos = kItemSlotLeftEdgePos + Vector{ kItemBarItemSlotSize.m_x * 0.5f };

	return kItemSlotCenterPos;
}

void ItemBarUI::Update()
{
	if (!m_isEnable) return;
	if (!m_playerItem.lock()) return;

	int index = m_playerItem.lock()->GetItemBarChoiceIndex();

	//選択時の色変更
	for (int i = 0; i < kItemBarItemCount; i++)
	{
		if (i == index)
			m_itemBarBoxes.at(i)->SetColor(kItemBarBoxHighlight);
		else
			m_itemBarBoxes.at(i)->SetDefaultColor();
	}

	//アイテムがあるときにそのアイコンを表示する
	for (int i = 0; i < kItemBarItemCount; i++)
	{
		const auto& itemStack = m_playerItem.lock()->GetItemFromItemBar(i);

		if (!itemStack)
			m_itemBarBoxes.at(i)->SetGraphicID(GraphicId::kNone);
		else
			m_itemBarBoxes.at(i)->SetGraphicID(itemStack->GetItemIconGraphicID());
	}

	//アイテムがあるときにその数を表示する
	for (int i = 0; i < kItemBarItemCount; i++)
	{
		const auto& itemStack = m_playerItem.lock()->GetItemFromItemBar(i);

		if (!itemStack)
			m_itemBarBoxes.at(i)->SetText("");
		else
			m_itemBarBoxes.at(i)->SetText("x{}", itemStack->GetItemCount());
	}

	//アイテムラベルの表示
	for (int i = 0; i < kItemBarItemCount; i++)
	{
		const auto& itemStack = m_playerItem.lock()->GetItemFromItemBar(i);

		m_itemBarBoxes.at(i)->SetLabelVisible(false);
		if (!itemStack) m_itemBarBoxes.at(i)->SetLabelText("");
		else
			m_itemBarBoxes.at(i)->SetLabelText(ItemTable::ItemTypeToDisplayName(itemStack->GetItemType()));
	}
}

void ItemBarUI::Finalize()
{
	m_isEnable = false;
	if (m_itemBarUIPanel)
	{
		m_itemBarUIPanel->SetVisible(false);
		m_uiManager.RemoveScreen("itemBar");
		// 表示テーブルにパネルが残っていても、UI要素とコールバックを残さない。
		m_itemBarUIPanel->ClearChildren();
		m_itemBarUIPanel.reset();
	}
	m_itemBarBoxes.clear();
	m_getItemUI = {};
	m_lastGetItem = { Item::kNone, 0 };
	m_playerItem.reset();
	Component::Finalize();
}
