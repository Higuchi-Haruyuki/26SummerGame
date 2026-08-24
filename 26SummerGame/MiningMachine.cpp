#include "MiningMachine.h"
#include "Debug.h"
#include "FactoryComponent.h"
#include "FactoryManager.h"
#include "Game.h"
#include "ItemStack.h"
#include "Object.h"
#include "PlayerInput.h"
#include "Square3D.h"
#include "SquareCollider3D.h"
#include "UIManager.h"
#include "vector.h"
#include "Recipe.h"
#include "RecipeManager.h"
#include "DrawText.h"
#include <DxLib.h>
#include <format>
#include <functional>
#include <memory>
#include <Windows.h>
#include <string>
#include "ResourceManager.h"
#include"ResourceName.h"
#include "ResourceGenerator.h"
#include "MapManager.h"
#include "FuelSystem.h"
#include "ItemType.h"
#include "Timer.h"
#include "UIItemBox.h"
#include "UIText.h"
#include "ItemSlot.h"

namespace
{
	//オブジェクトのタグ
	const std::string kObjectTag = "MiningMachine";

	//アイテムの名前
	constexpr Item kItemType = Item::kMiningMachine;

	const Vector kSize = { 200,200,200 };

	constexpr unsigned int kColor = 0x696969;

	constexpr int kMaxInputItemSlot = 1;
	constexpr int kMaxOutputItemSlot = 1;

	//一秒間当たりどのくらいのアイテムを処理するか
	constexpr int kProcessItemPerSecond = 1;

	constexpr Second kMainProcessDuration = 1.0f;

	//燃料として使用するアイテム
	constexpr Item kFuelItemType = Item::kCoalOre;

	/*UI要素の定数*/
	const Vector kInputUIPos = Game::kDisplaySize / 2;

	const Vector kFuelUIPos = { Game::kDisplaySize.m_x * 0.5f , Game::kDisplaySize.m_y * 0.6f };

	const Vector kUISize = { Game::kDisplaySize.m_x / 20, Game::kDisplaySize.m_x / 20 };
}

MiningMachine::MiningMachine(std::shared_ptr<Object> parentObject) :
	FactoryComponent(parentObject), m_mapManager(MapManager::GetInstance())
{}

void MiningMachine::Init()
{
	FactoryComponent::Init();

	m_shape = GetParentObject()->GetComponent<Square3D>();
	if (!m_shape)
		m_shape = GetParentObject()->AddComponent<Square3D>();
	m_collider = GetParentObject()->GetComponent<SquareCollider3D>();
	if (!m_collider)
		m_collider = GetParentObject()->AddComponent<SquareCollider3D>();

	m_fuelSystem = GetParentObject()->AddComponent<FuelSystem>();

	const auto& square3D = std::static_pointer_cast<Square3D>(m_shape);
	square3D->SetUVScrollTexHandle(GraphicId::kMiningMachineTop);
	square3D->SetUVScrollOffset(1);

	SetSizeAndColorAndMaxSlot(kSize, kColor, kMaxInputItemSlot, kMaxOutputItemSlot);
	SetItemName(kItemType);
	SetTimerDuration(kMainProcessDuration);
	m_fuelSystem.lock()->SetFuelItem(kFuelItemType);
}

void MiningMachine::Update(float deltaTime)
{
	FactoryComponent::Update(deltaTime);

	if (!m_isEnable) return;

	if (m_isPreviewMode) return;

	if (!m_fuelSystem.lock()->GetFuelRemain())
	{
		m_timer->SetEnable(false);
		return;
	}

	m_timer->SetEnable(true);

	if (!m_timer->IsTimeOver()) return;

	//資源を取得
	auto myGridPos = GetParentObject()->GetGridPosition();
	Item resource = m_mapManager.GetResourceAtGridPos(myGridPos);

	bool result = TryInsert(m_outputSlot.get(), std::make_unique<ItemStack>(resource, 1));

	if (result) 
	{
		m_fuelSystem.lock()->UseFuel();
		m_factoryManager.OnMakeItem(resource, 1);
	}

	m_timer->ResetStartTime();
}

void MiningMachine::UpdateUIPanel()
{
	const auto item = GetOutputItemStack(0);

	if (item)
	{
		m_itemUI->SetGraphicID(item->GetItemIconGraphicID());
		m_itemUI->SetText("x{}", item->GetItemCount());
	}
	else
	{
		m_itemUI->SetGraphicID(GraphicId::kNone);
		m_itemUI->SetText("");
	}


	const auto& fuel = m_fuelSystem.lock()->GetFuelSlot().lock()->GetItem(0);

	if (fuel)
	{
		m_fuelItemUI->SetGraphicID(fuel->GetItemIconGraphicID());
		m_fuelItemUI->SetText("x{}", fuel->GetItemCount());
	}
	else
	{
		m_fuelItemUI->SetGraphicID(GraphicId::kNone);
		m_fuelItemUI->SetText("");
	}

}

std::unique_ptr<ItemStack> MiningMachine::MakeItemStackFromThisComponent()
{
	return ItemStack::MakeItemStack<MiningMachine>(kObjectTag, kItemType, 1);
}

void MiningMachine::BuildUIPanel()
{
	m_itemUI = std::make_shared<UIItemBox>(m_uiPanel, kInputUIPos, kUISize);
	m_itemUI->SetOnSelectItem(GetOutputItemSlot(), 0);
	m_itemUI->SetOnMoveItem(GetOutputItemSlot(), 0);

	m_fuelItemUI = std::make_shared<UIItemBox>(m_uiPanel, kFuelUIPos, kUISize);
	m_fuelItemUI->SetEnableFilterItem(true);
	m_fuelItemUI->SetFilterItem(kFuelItemType);
	m_fuelItemUI->SetOnSelectItem(m_fuelSystem.lock()->GetFuelSlot(), 0);
	m_fuelItemUI->SetOnMoveItem(m_fuelSystem.lock()->GetFuelSlot(), 0);
}
