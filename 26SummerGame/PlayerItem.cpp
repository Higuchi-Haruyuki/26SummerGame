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

namespace
{
	//アイテムバーのアイテム数
	constexpr int kItemBarItemCount = 10;

	//アイテムインベントリの定数
	constexpr int kInventoryWidthCount = 10;
	constexpr int kInventoryHeightCount = 10;

}

PlayerItem::PlayerItem(const std::shared_ptr<Object> parentObject)
	:Component(parentObject), m_input(PlayerInput::GetInstance()),
	m_uiManager(UIManager::GetInstance())
{
}

void PlayerItem::Init()
{
	Component::Init();

	m_state = GetParentObject()->GetComponent<CharactorStateManager>();
	m_collider = GetParentObject()->GetComponent<Collider>();

	m_itemSlot = std::make_shared<ItemSlot>(kItemBarItemCount);

	m_inventory = std::make_shared<ItemSlot>(kInventoryWidthCount * kInventoryHeightCount);

	m_itemSlot->AddItemStack(0, ItemStack::MakeItemStack<MiningMachine>("MiningMachine", Item::kMiningMachine, 100));
	m_itemSlot->AddItemStack(1, ItemStack::MakeItemStack<BeltConveyor>("BeltConveyor", Item::kBeltconveyor, 100));
	m_itemSlot->AddItemStack(2, ItemStack::MakeItemStack<Furnace>("Furnace", Item::kFurnace, 100));
	m_itemSlot->AddItemStack(3, ItemStack::MakeItemStack<Inserter>("Inserter", Item::kInserter, 100));
	m_itemSlot->AddItemStack(4, ItemStack::MakeItemStack(Item::kIronPlate, 30));
	m_itemSlot->AddItemStack(5, ItemStack::MakeItemStack(Item::kCopperPlate, 50));
	m_itemSlot->AddItemStack(6, ItemStack::MakeItemStack(Item::kIronOre, 100));
	m_itemSlot->AddItemStack(7, ItemStack::MakeItemStack(Item::kRockOre, 100));
	m_itemSlot->AddItemStack(8, ItemStack::MakeItemStack(Item::kCopperOre, 100));
	m_itemSlot->AddItemStack(9, ItemStack::MakeItemStack(Item::kCoalOre, 50));
}

void PlayerItem::Update(float deltaTime)
{
	Component::Update(deltaTime);

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

std::unique_ptr<ItemStack> PlayerItem::AddItem(std::unique_ptr<ItemStack> item, int count)
{
	auto remainItem = AddItemToItemBar(std::move(item), count);
	if (!remainItem) return nullptr;

	//アイテムバーに入りきらなかった残りだけをインベントリへ
	const int remainCount = remainItem->GetItemCount();

	return AddItemToInventory(std::move(remainItem), remainCount);
}

bool PlayerItem::CanAddItem(Item itemType, int count) const
{
	if (CanAddToItemBar(itemType,count)) return true;
	if (CanAddToInventory(itemType, count)) return true;
	return false;
}

std::unique_ptr<ItemStack> PlayerItem::AddItemToItemBar(std::unique_ptr<ItemStack> item, int count)
{
	return m_itemSlot->AddItemStack(std::move(item), count);
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

std::unique_ptr<ItemStack> PlayerItem::AddItemToInventory(std::unique_ptr<ItemStack> item, int count)
{
	return m_inventory->AddItemStack(std::move(item), count);
}

bool PlayerItem::CanAddToInventory(Item itemType, int count) const
{
	return CanAddToItemSlot(m_inventory, itemType, count);
}

void PlayerItem::ChoiceItemSlot()
{
	m_selectedItemBar = m_itemSlot->GetItem(m_itemBarChoiceIndex);

	Debug::Log(std::format("選択中のアイテムバーのインデックス: {}", m_itemBarChoiceIndex));
}

void PlayerItem::ChoiceInventorySlot()
{
	m_selectedInventoryItem = m_inventory->GetItem(m_inventoryChoiceIndex);

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
