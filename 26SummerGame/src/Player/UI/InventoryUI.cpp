#include "InventoryUI.h"

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
#include "PlayerController.h"
#include "PlayerCraft.h"
#include "Recipe.h"
#include "UIKeyPrompt.h"

namespace
{
	/*インベントリUIの定数*/
	const Vector kInventoryPos =
	{
		Game::kDisplaySize.m_x / 4,
		Game::kDisplaySize.m_y / 2.5f
	};
	const Vector kInventorySize = { 850,810 };
	constexpr unsigned int kInventoryColor = static_cast<unsigned int>(Color::kMainColor);
	constexpr int kInventoryAlpha = 130;

	constexpr int kItemBoxCountWidth = 10;
	constexpr int kInventorySlotOffsetX = 10;
	constexpr int kInventorySlotOffsetY = 10;

	const Vector kInventoryItemSlotSize = { 70,70 };
	const Vector kKeyPromptSize = { 250,50 };
	const Vector kKeyPromptDownLeftPos = { Game::kDisplaySize.m_x / 5 * 4 - 200,Game::kDisplaySize.m_y - 100 };
}

InventoryUI::InventoryUI(std::weak_ptr<Object> parentObject) :
	Component(parentObject),
	m_uiManager(UIManager::GetInstance())
{}

void InventoryUI::Init()
{
	Component::Init();
	m_playerController = GetComponent<PlayerController>();
	m_playerItem = GetComponent<PlayerItem>();
	m_playerCraft = GetComponent<PlayerCraft>();
	m_playerCraft.lock()->SetAllowRecipeType(RecipeType::kPlayerCraft);
	m_inventoryUIPanel = m_uiManager.GetOrCreateScreen("inventory", UILayer::Hud);

	//インベントリの背景部分
	UIFactory::MakeUIToPanel<UISquare>(m_inventoryUIPanel, kInventoryPos, kInventorySize, kInventoryColor, kInventoryAlpha);

	const auto& inventoryClose = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_inventoryUIPanel, kKeyPromptDownLeftPos + Vector{ -1000 }, kKeyPromptSize, GraphicId::kKeyboardC, "閉じる");
	inventoryClose.lock()->SubscribeOnClick([this]() {
		m_playerController.lock()->CloseInventoryUI();
		});


	const auto& inventory = m_playerItem.lock()->GetInventory();
	for (int i = 0; i < inventory.lock()->GetSlotCount(); i++)
	{
		const auto& itemBox = std::make_shared<UIItemBox>(
			m_inventoryUIPanel, CalcInventoryItemSlotPos(i), kInventoryItemSlotSize
		);

		int index = i;

		itemBox->SetOnClickEvent(
			[this, index]
			{
				m_playerItem.lock()->SetInventoryChoiceIndex(index);
			}
		);
		itemBox->SetOnDragBeginEvent(
			[this, inventory, index]()
			{
				m_uiManager.ItemSelect(inventory, index);
			}
		);
		itemBox->SetOnDropEvent(
			[this, inventory, index]
			{
				m_uiManager.MoveItem(inventory, index);
			}
		);

		itemBox->SetOnSelectHalfItem(inventory, index);
		itemBox->SetOnMoveHalfItem(inventory, index);

		//配列に追加
		m_inventoryBoxes.push_back(itemBox);
	}

	const auto basepos = kInventoryPos + Vector{ kInventorySize.m_x * 0.5f,-kInventorySize.m_y * 0.5f };
	const auto size = Vector{ 30,30 };

	const auto pos = basepos + Vector{ -size.m_x * 0.5f,size.m_y * 0.5f } + Vector{-10,10};

	const auto& close = UIFactory::MakeUIToPanel<UIImage>(m_inventoryUIPanel, pos, size, GraphicId::kUIClose);
	close.lock()->SubscribeOnClick([this]()
		{
			m_playerController.lock()->CloseInventoryUI();
		}
	);

	m_inventoryUIPanel->AddChild(m_playerCraft.lock()->GetOrBuidUIPanel());
}

Vector InventoryUI::CalcInventoryItemSlotPos(int index) const
{
	int width = index % kItemBoxCountWidth;
	int height = index / kItemBoxCountWidth;

	//アイテムバーの左端の座標 (yは中心)
	const Vector kItemBarLeftEdge = kInventoryPos - Vector{ kInventorySize.m_x * 0.5f , kInventorySize.m_y * 0.5f };
	//アイテムバーのアイテムスロットの1つ目の左端の座標((kItemBarLeftEdge) + (スロット間のオフセット)の値)
	const Vector kItemSlotLeftEdgeStartPos = kItemBarLeftEdge + Vector{ kInventorySlotOffsetX,kInventorySlotOffsetY };
	//i個目のアイテムバーのアイテムスロットの左端の座標
	const Vector kItemSlotLeftEdgePos = kItemSlotLeftEdgeStartPos + 
		Vector{ 
		(kInventoryItemSlotSize.m_x + kInventorySlotOffsetX) * width,
		(kInventoryItemSlotSize.m_y + kInventorySlotOffsetY) * height 
	};
	//i個目のアイテムバーのアイテムスロットの中心座標
	const Vector kItemSlotCenterPos = kItemSlotLeftEdgePos + Vector{ kInventoryItemSlotSize.m_x * 0.5f, kInventoryItemSlotSize.m_y * 0.5f };

	return kItemSlotCenterPos;
}

void InventoryUI::Update()
{
	if (!m_isEnable) return;
	if (!m_playerItem.lock()) return;

	int inventorySize = m_playerItem.lock()->GetInventory().lock()->GetSlotCount();

	//アイテムがあるときにそのアイコンを表示する
	for (int i = 0; i < inventorySize; i++)
	{
		const auto& itemStack = m_playerItem.lock()->GetItemFromInventory(i);

		if (!itemStack)
			m_inventoryBoxes.at(i)->SetGraphicID(GraphicId::kNone);
		else
			m_inventoryBoxes.at(i)->SetGraphicID(itemStack->GetItemIconGraphicID());
	}

	//アイテムがあるときにその数を表示する
	for (int i = 0; i < inventorySize; i++)
	{
		const auto& itemStack = m_playerItem.lock()->GetItemFromInventory(i);

		if (!itemStack)
			m_inventoryBoxes.at(i)->SetText("");
		else
			m_inventoryBoxes.at(i)->SetText("x{}", itemStack->GetItemCount());
	}

	//アイテムラベルの表示
	for (int i = 0; i < inventorySize; i++)
	{
		const auto& itemStack = m_playerItem.lock()->GetItemFromInventory(i);

		m_inventoryBoxes.at(i)->SetLabelVisible(false);

		if (!itemStack)
			m_inventoryBoxes.at(i)->SetLabelText("");
		else
			m_inventoryBoxes.at(i)->SetLabelText(ItemTable::ItemTypeToDisplayName(itemStack->GetItemType()));
	}
}

void InventoryUI::Finalize()
{
	m_isEnable = false;
	if (m_inventoryUIPanel)
	{
		m_inventoryUIPanel->SetVisible(false);
		m_uiManager.RemoveScreen("inventory");
		m_inventoryUIPanel->ClearChildren();
		m_inventoryUIPanel.reset();
	}
	m_inventoryBoxes.clear();
	m_playerController.reset();
	m_playerCraft.reset();
	m_playerItem.reset();
	Component::Finalize();
}
