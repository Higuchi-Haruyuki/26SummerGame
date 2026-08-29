#include "AssemblingMachine.h"
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
#include "FuelSystem.h"
#include "Timer.h"
#include "ManufacturingSystem.h"
#include "ItemType.h"
#include "UIItemBox.h"
#include "ItemSlot.h"
#include "UIPanel.h"
#include "ItemManager.h"

namespace
{

	//アイテムの名前
	const Item kItemType = Item::kAssemblingMachine;

	const Vector kSize = { 200,200,200 };

	constexpr unsigned int kColor = 0xd3d3d3;

	constexpr int kMaxInputItemSlot = 1;
	constexpr int kMaxOutputItemSlot = 1;

	//一秒間当たりどのくらいのアイテムを処理するか
	constexpr int kProcessItemPerSecond = 1;

	constexpr Second kMainProcessDuration = 1.0f;

	//燃料として使用するアイテム
	const Item kFuelItemType = Item::kCoalOre;

	//UIの透明度
	constexpr int kUIAlpha = 180;

	const Vector kInputUIPos = { Game::kDisplaySize.m_x / 3 , Game::kDisplaySize.m_y / 2 };

	const Vector kOutputUIPos = { Game::kDisplaySize.m_x / 3 * 2, Game::kDisplaySize.m_y / 2 };

	const Vector kFuelUIPos = { Game::kDisplaySize.m_x * 0.5f , Game::kDisplaySize.m_y * 0.6f };

	const Vector kUISize = { Game::kDisplaySize.m_x / 20, Game::kDisplaySize.m_x / 20 };
}

AssemblingMachine::AssemblingMachine(std::weak_ptr<Object> parentObject):
	FactoryComponent(parentObject)
{}

AssemblingMachine::~AssemblingMachine()
{}

void AssemblingMachine::Init()
{
	FactoryComponent::Init();

	m_shape = GetComponent<Square3D>();
	if (!m_shape.lock())
		m_shape = AddComponent<Square3D>();

	m_fuelSystem = AddComponent<FuelSystem>();

	m_manufacturingSystem = AddComponent<ManufacturingSystem>();

	m_manufacturingSystem.lock()->SetAllowRecipeType(RecipeType::kAssemblingMachine);

	const auto& square3D = std::static_pointer_cast<Square3D>(m_shape.lock());
	square3D->SetUVScrollTexHandle(GraphicId::kAssemblingMachineIcon);
	square3D->SetUVScrollOffset(1);

	SetSizeAndColorAndMaxSlot(kSize, kColor, kMaxInputItemSlot, kMaxOutputItemSlot);
	SetItemName(kItemType);
	SetTimerDuration(kMainProcessDuration);
	m_fuelSystem.lock()->SetFuelItem(kFuelItemType);
}

void AssemblingMachine::Update()
{
	FactoryComponent::Update();

	if (!m_isEnable) return;

	if (m_isPreviewMode) return;

	if (!m_fuelSystem.lock()->GetFuelRemain())
	{
		m_timer->SetEnable(false);
		return;
	}

	m_timer->SetEnable(true);

	if (!m_timer->IsTimeOver())
	{
		//アイテムスタックからアイテムタイプを取得する。
		m_manufacturingSystem.lock()->SetRecipe(m_inputSlot.get());
		return;
	}

	//溶鉱炉の処理をする。
	bool result = m_manufacturingSystem.lock()->Manufacture(m_inputSlot.get(), m_outputSlot.get());

	if (result) m_fuelSystem.lock()->UseFuel();

	m_timer->ResetStartTime();
}

void AssemblingMachine::GetAllItemOwnership(FactoryComponent::ItemContainer* result)
{
	FactoryComponent::GetAllItemOwnership(result);
	m_fuelSystem.lock()->GetAllItemOwnership(result);
}

void AssemblingMachine::UpdateUIPanel()
{
	const auto input = GetInputItemStack(0);

	if (input)
	{
		m_inputItemUI->SetGraphicID(input->GetItemIconGraphicID());
		m_inputItemUI->SetText("x{}", input->GetItemCount());
	}
	else
	{
		m_inputItemUI->SetGraphicID(GraphicId::kNone);
		m_inputItemUI->SetText("");
	}

	const auto output = GetOutputItemStack(0);

	if (output)
	{
		m_outputItemUI->SetGraphicID(output->GetItemIconGraphicID());
		m_outputItemUI->SetText("x{}", output->GetItemCount());
	}
	else
	{
		m_outputItemUI->SetGraphicID(GraphicId::kNone);
		m_outputItemUI->SetText("");
	}

	const auto& fuel = m_fuelSystem.lock()->GetFuelSlot().lock()->GetItem(0);

	if (fuel)
	{
		m_fuelItemUI->SetGraphicID(fuel->GetItemIconGraphicID());
		m_fuelItemUI->SetImageAlpha(255);
		m_fuelItemUI->SetText("x{}", fuel->GetItemCount());
	}
	else
	{
		m_fuelItemUI->SetGraphicID(ItemTable::GetGraphicID(kFuelItemType));
		m_fuelItemUI->SetImageAlpha(100);
		m_fuelItemUI->SetText("");
	}
}

void AssemblingMachine::BuildUIPanel()
{
	m_inputItemUI = std::make_shared<UIItemBox>(m_uiPanel, kInputUIPos, kUISize);
	m_inputItemUI->SetOnSelectItem(GetInputItemSlot(), 0);
	m_inputItemUI->SetOnMoveItem(GetInputItemSlot(), 0);

	m_inputItemUI->SetOnSelectHalfItem(GetInputItemSlot(), 0);
	m_inputItemUI->SetOnMoveHalfItem(GetInputItemSlot(), 0);

	m_outputItemUI = std::make_shared<UIItemBox>(m_uiPanel, kOutputUIPos, kUISize);
	m_outputItemUI->SetOnSelectItem(GetOutputItemSlot(), 0);

	m_outputItemUI->SetOnSelectHalfItem(GetOutputItemSlot(), 0);

	m_fuelItemUI = std::make_shared<UIItemBox>(m_uiPanel, kFuelUIPos, kUISize);
	m_fuelItemUI->SetEnableFilterItem(true);
	m_fuelItemUI->SetFilterItem(kFuelItemType);
	m_fuelItemUI->SetOnSelectItem(m_fuelSystem.lock()->GetFuelSlot(), 0);
	m_fuelItemUI->SetOnMoveItem(m_fuelSystem.lock()->GetFuelSlot(), 0);

	m_fuelItemUI->SetOnSelectHalfItem(m_fuelSystem.lock()->GetFuelSlot(), 0);
	m_fuelItemUI->SetOnMoveHalfItem(m_fuelSystem.lock()->GetFuelSlot(), 0);

	m_uiPanel->AddChild(m_manufacturingSystem.lock()->GetOrBuidUIPanel());

}
