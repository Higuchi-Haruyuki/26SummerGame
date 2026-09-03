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

	constexpr int kMaxInputItemSlot = 2;
	constexpr int kMaxOutputItemSlot = 1;

	//一秒間当たりどのくらいのアイテムを処理するか
	constexpr int kProcessItemPerSecond = 1;

	constexpr Second kMainProcessDuration = 1.0f;

	//燃料として使用するアイテム
	const Item kFuelItemType = Item::kCoalOre;

	//UIの透明度
	constexpr int kUIAlpha = 180;

	const Vector kInputUIStartPos = { Game::kDisplaySize.m_x / 3 , Game::kDisplaySize.m_y / 2 };

	const Vector kInputUIOffset = { Game::kDisplaySize.m_x / 20 };

	constexpr int kInputUICount = 2;

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
	for (const auto& input : m_inputUIs)
	{
		input->SetLabelVisible(false);
	}
	m_outputItemUI->SetLabelVisible(false);
	m_fuelItemUI->SetLabelVisible(false);

	const auto recipe = m_manufacturingSystem.lock()->GetCurrentRecipe().lock();

	for (int i = 0; i < m_inputUIs.size(); i++)
	{
		const auto input = GetInputItemStack(i);

		//入力にアイテムが入っているとき
		if (input)
		{
			m_inputUIs.at(i)->SetGraphicID(input->GetItemIconGraphicID());
			m_inputUIs.at(i)->SetText("x{}", input->GetItemCount());
			m_inputUIs.at(i)->SetImageAlpha(255);
			m_inputUIs.at(i)->SetLabelText(ItemTable::ItemTypeToDisplayName(input->GetItemType()));

		}
		//レシピがセットされているときで、インデックスがレシピの入力個数以下(out of range防止)のとき
		else if (recipe && i < recipe->GetRecipeInput().size())
		{
			auto currentRecipe = recipe->GetRecipeInput().at(i);
			auto graphicId = ItemTable::GetGraphicID(currentRecipe.first);
			m_inputUIs.at(i)->SetGraphicID(graphicId);
			m_inputUIs.at(i)->SetText("");
			m_inputUIs.at(i)->SetImageAlpha(50);
			m_inputUIs.at(i)->SetLabelText(ItemTable::ItemTypeToDisplayName(currentRecipe.first) + "が必要");
		}
		else
		{
			m_inputUIs.at(i)->SetGraphicID(GraphicId::kNone);
			m_inputUIs.at(i)->SetText("");
			m_inputUIs.at(i)->SetLabelText("入力");
		}

	}

	const auto output = GetOutputItemStack(0);
	if (output)
	{
		m_outputItemUI->SetGraphicID(output->GetItemIconGraphicID());
		m_outputItemUI->SetText("x{}", output->GetItemCount());
		m_outputItemUI->SetImageAlpha(255);
		m_outputItemUI->SetLabelText(ItemTable::ItemTypeToDisplayName(output->GetItemType()));
	}
	else if (recipe)
	{
		auto currentRecipe = recipe->GetRecipeOutput().at(0);
		auto graphicId = ItemTable::GetGraphicID(currentRecipe.first);
		m_outputItemUI->SetGraphicID(graphicId);
		m_outputItemUI->SetText("");
		m_outputItemUI->SetImageAlpha(50);
		m_outputItemUI->SetLabelText(ItemTable::ItemTypeToDisplayName(currentRecipe.first) + "ができる");
	}
	else
	{
		m_outputItemUI->SetGraphicID(GraphicId::kNone);
		m_outputItemUI->SetText("");
		m_outputItemUI->SetLabelText("出力");
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
		m_fuelItemUI->SetLabelText(ItemTable::ItemTypeToDisplayName(kFuelItemType) + "が必要");
	}
}

bool AssemblingMachine::TryInsert(ItemStack* item, int count)
{
	if (item->GetItemType() == kFuelItemType)
	{
		const auto fuelSlot = m_fuelSystem.lock()->GetFuelSlot().lock();
		return FactoryComponent::TryInsert(fuelSlot.get(), item, count);
	}
	return FactoryComponent::TryInsert(m_inputSlot.get(), item, count);
}

void AssemblingMachine::BuildUIPanel()
{
	auto pos = kInputUIStartPos;

	for (int i = 0; i < kInputUICount; i++)
	{
		const auto& inputUI = std::make_shared<UIItemBox>(m_uiPanel, pos, kUISize);
		inputUI->SetOnSelectItem(GetInputItemSlot(), i);
		inputUI->SetOnMoveItem(GetInputItemSlot(), i);

		inputUI->SetOnSelectHalfItem(GetInputItemSlot(), i);
		inputUI->SetOnMoveHalfItem(GetInputItemSlot(), i);
		inputUI->SetNotVisibleWhenNoGraphic(false);

		m_inputUIs.push_back(inputUI);

		pos += Vector{ kUISize.m_x * 0.5f } + kInputUIOffset;
	}

	m_outputItemUI = std::make_shared<UIItemBox>(m_uiPanel, kOutputUIPos, kUISize);
	m_outputItemUI->SetOnSelectItem(GetOutputItemSlot(), 0);

	m_outputItemUI->SetOnSelectHalfItem(GetOutputItemSlot(), 0);
	m_outputItemUI->SetNotVisibleWhenNoGraphic(false);

	m_fuelItemUI = std::make_shared<UIItemBox>(m_uiPanel, kFuelUIPos, kUISize);
	m_fuelItemUI->SetEnableFilterItem(true);
	m_fuelItemUI->SetFilterItem(kFuelItemType);
	m_fuelItemUI->SetOnSelectItem(m_fuelSystem.lock()->GetFuelSlot(), 0);
	m_fuelItemUI->SetOnMoveItem(m_fuelSystem.lock()->GetFuelSlot(), 0);

	m_fuelItemUI->SetOnSelectHalfItem(m_fuelSystem.lock()->GetFuelSlot(), 0);
	m_fuelItemUI->SetOnMoveHalfItem(m_fuelSystem.lock()->GetFuelSlot(), 0);
	m_fuelItemUI->SetNotVisibleWhenNoGraphic(false);

	m_uiPanel->AddChild(m_manufacturingSystem.lock()->GetOrBuidUIPanel());

}
