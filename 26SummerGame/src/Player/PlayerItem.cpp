#include "BeltConveyor.h"
#include "CharactorStateManager.h"
#include "Collider.h"
#include "Component.h"
#include "Debug.h"
#include "DrawText.h"
#include "FactoryComponent.h"
#include "Furnace.h"
#include "Game.h"
#include "ItemStack.h"
#include "MiningMachine.h"
#include "Object.h"
#include "ObjectFactory.h"
#include "PlayerInput.h"
#include "PlayerItem.h"
#include "ResourceManager.h"
#include "ResourceName.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Shape.h"
#include "UIManager.h"
#include "vector.h"
#include "VectorInt.h"
#include <DxLib.h>
#include <format>
#include <memory>
#include "Inserter.h"
#include "RayCastResult.h"
#include <string>
#include <vector>

#include "ItemType.h"
#include "ItemSlot.h"

#include "AssemblingMachine.h"
#include "ItemStackFactory.h"

namespace
{
	//アイテムバーのアイテム数
	constexpr int kItemBarItemCount = 10;

	//アイテムインベントリの定数
	constexpr int kInventoryWidthCount = 10;
	constexpr int kInventoryHeightCount = 10;

}

PlayerItem::PlayerItem(std::weak_ptr<Object> parentObject)
	:Component(parentObject), m_input(PlayerInput::GetInstance()),
	m_uiManager(UIManager::GetInstance())
{
}

void PlayerItem::Init()
{
	Component::Init();

	m_state = GetComponent<CharactorStateManager>();
	m_collider = GetComponent<Collider>();

	m_itemSlot = std::make_shared<ItemSlot>(kItemBarItemCount);

	m_inventory = std::make_shared<ItemSlot>(kInventoryWidthCount * kInventoryHeightCount);

	m_itemSlot->AddItem(0, ItemStackFactory::Make(Item::kMiningMachine,3));
	m_itemSlot->AddItem(1, ItemStackFactory::Make(Item::kBeltconveyor, 30));
	m_itemSlot->AddItem(2, ItemStackFactory::Make(Item::kFurnace, 3));
	m_itemSlot->AddItem(3, ItemStackFactory::Make(Item::kInserter, 3));
	m_itemSlot->AddItem(4, ItemStackFactory::Make(Item::kCopperPlate, 50));
	m_itemSlot->AddItem(5, ItemStackFactory::Make(Item::kIronPlate, 50));
	m_itemSlot->AddItem(6, ItemStackFactory::Make(Item::kRockOre, 50));
	m_itemSlot->AddItem(7, ItemStackFactory::Make(Item::kIronOre, 50));
	m_itemSlot->AddItem(8, ItemStackFactory::Make(Item::kCoalOre, 100));
	m_itemSlot->AddItem(9, ItemStackFactory::Make(Item::kCoalOre, 100));
}

void PlayerItem::Update()
{
	Component::Update();

	ChoiceItemSlot();
	ChoiceInventorySlot();
}

void PlayerItem::SetItemBarChoiceIndex(int index)
{
	bool isLower = index < 0;
	bool isUpper = index > m_itemSlot->GetSlotCount();

	bool isOutOfRange = isLower || isUpper;

	if (isOutOfRange) return;

	m_itemBarChoiceIndex = index;
}

std::shared_ptr<ItemStack> PlayerItem::AddItem(std::shared_ptr<ItemStack> item, int count)
{
	auto itemType = item->GetItemType();
	auto remainItem = AddItemToItemBar(std::move(item), count);
	if (!remainItem) 
	{
		m_onAddItem.Invoke(itemType, count);
		return nullptr;
	}

	//アイテムバーに入りきらなかった残りだけをインベントリへ
	const int remainCount = remainItem->GetItemCount();

	auto remainItemAfterInventory = AddItemToInventory(std::move(remainItem), remainCount);
	if (!remainItemAfterInventory)
	{
		m_onAddItem.Invoke(itemType, count);
		return nullptr;
	}

	const auto remainCountAfterInventory = remainItemAfterInventory->GetItemCount();

	m_onAddItem.Invoke(itemType,count - remainCountAfterInventory);
	return remainItemAfterInventory;

}

bool PlayerItem::CanAddItem(Item itemType, int count) const
{
	if (CanAddToItemBar(itemType,count)) return true;
	if (CanAddToInventory(itemType, count)) return true;
	return false;
}

std::shared_ptr<ItemStack> PlayerItem::AddItemToItemBar(std::shared_ptr<ItemStack> item, int count)
{
	return m_itemSlot->AddItem(std::move(item), count);
}

bool PlayerItem::CanAddToItemBar(Item itemType, int count) const
{
	return CanAddToItemSlot(m_itemSlot, itemType, count);
}

ItemStack* PlayerItem::GetItemFromInventory(int idx) const
{
	return m_inventory->GetItem(idx);
}

void PlayerItem::SetInventoryChoiceIndex(int index)
{
	bool isLower = index < 0;
	bool isUpper = index > m_inventory->GetSlotCount();

	bool isOutOfRange = isLower || isUpper;

	if (isOutOfRange) return;

	m_inventoryChoiceIndex = index;
}

ItemStack* PlayerItem::GetItemFromItemBar(int idx) const
{
	bool isLower = idx < 0;
	bool isUpper = idx > m_itemSlot->GetSlotCount();
	bool isOutOfRange = isLower || isUpper;

	if (isOutOfRange) return nullptr;

	return m_itemSlot->GetItem(idx);
}

std::shared_ptr<ItemStack> PlayerItem::AddItemToInventory(std::shared_ptr<ItemStack> item, int count)
{
	return m_inventory->AddItem(std::move(item), count);
}

bool PlayerItem::CanAddToInventory(Item itemType, int count) const
{
	return CanAddToItemSlot(m_inventory, itemType, count);
}

void PlayerItem::ChoiceItemSlot()
{
	m_selectedItemBar = m_itemSlot->GetItemReference(m_itemBarChoiceIndex);

	Debug::Log(std::format("選択中のアイテムバーのインデックス: {}", m_itemBarChoiceIndex));
}

void PlayerItem::ChoiceInventorySlot()
{
	m_selectedInventoryItem = m_inventory->GetItemReference(m_inventoryChoiceIndex);

	Debug::Log(std::format("選択中のインベントリのインデックス: {}", m_inventoryChoiceIndex));
}

bool PlayerItem::CanAddToItemSlot(const std::weak_ptr<ItemSlot> itemSlot, Item itemType, int count) const
{
	const auto& safeItemSlot = itemSlot.lock();
	if (!safeItemSlot) return false;

	//空いているスロットがあるならtrueを返す。
	if (safeItemSlot->FindEmptyItemSlot() != -1) return true;

	int remainCount = count;

	for (int i = 0; i < safeItemSlot->GetSlotCount(); i++)
	{
		const auto* item = safeItemSlot->GetItem(i);
		if (!item) continue;

		if (item->GetItemType() != itemType) continue;

		auto canAddCount = item->CheckAddItemCount(remainCount);
		remainCount -= canAddCount;

		if (remainCount <= 0) break;
	}

	return remainCount <= 0;
}
