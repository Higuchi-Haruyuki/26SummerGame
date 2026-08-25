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

	m_completionCraft = std::make_shared<ItemSlot>(1);

	m_itemSlot->AddItemStack(0, ItemStack::MakeItemStack<MiningMachine>("MiningMachine", Item::kMiningMachine, 100));
	m_itemSlot->AddItemStack(1, ItemStack::MakeItemStack<BeltConveyor>("BeltConveyor", Item::kBeltconveyor, 100));
	m_itemSlot->AddItemStack(2, ItemStack::MakeItemStack<Furnace>("Furnace", Item::kFurnace, 100));
	m_itemSlot->AddItemStack(3, ItemStack::MakeItemStack<Inserter>("Inserter", Item::kInserter, 100));
	m_itemSlot->AddItemStack(4, ItemStack::MakeItemStack(Item::kIronOre, 30));
	m_itemSlot->AddItemStack(5, ItemStack::MakeItemStack(Item::kIronOre, 50));
	m_itemSlot->AddItemStack(6, ItemStack::MakeItemStack(Item::kIronOre, 30));
	m_itemSlot->AddItemStack(7, ItemStack::MakeItemStack(Item::kIronOre, 4));
	m_itemSlot->AddItemStack(8, ItemStack::MakeItemStack(Item::kIronOre, 4));
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

	return AddItemToInventory(std::move(remainItem), count);
}

std::unique_ptr<ItemStack> PlayerItem::AddItemToItemBar(std::unique_ptr<ItemStack> item, int count)
{
	return m_itemSlot->AddItemStack(std::move(item), count);
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
