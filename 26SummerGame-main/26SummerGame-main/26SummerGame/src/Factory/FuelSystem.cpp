#include "FuelSystem.h"
#include "Object.h"
#include "ItemSlot.h"

namespace
{
	constexpr int kIndex = 0;
	constexpr int kItemCount = 1;
	constexpr int kConsumeItemPerUse = 3;
}

FuelSystem::FuelSystem(std::shared_ptr<Object> parentObject) :
	Component(parentObject)
{

}

void FuelSystem::Init()
{
	m_fuelSlot = std::make_shared<ItemSlot>(kItemCount);
}

void FuelSystem::Update(float deltaTime)
{

}

bool FuelSystem::IsEqualFuelType(Item itemType) const
{
	if (m_usableFuelType == itemType) return true;
	return false;
}

bool FuelSystem::IsNeedAddNew() const
{
	//燃料スロットに何も入ってないとき
	bool isFuelSlotIsNull = !m_fuelSlot;
	if (isFuelSlotIsNull) return true;

	//使用可能なアイテムと燃料スロットのアイテムタイプが異なるとき
	bool isNotEqualFuelSlotAndUsableFuel = (m_fuelSlot->GetItem(kIndex)->GetItemType() != m_usableFuelType);
	if (isNotEqualFuelSlotAndUsableFuel) return true;

	return false;
}

bool FuelSystem::AddNewFuel(std::shared_ptr<ItemStack> item)
{
	m_fuelSlot->AddItem(std::move(item));
	return true;
}

bool FuelSystem::AddFuelCount(const std::shared_ptr<ItemStack>& item)
{
	if (!m_fuelSlot) return false;

	//燃料アイテムの取得
	const auto& fuelItem = m_fuelSlot->GetItem(kIndex);

	//すでに入っているアイテムと引数のアイテムが異なるとき
	bool isEqualItemType = fuelItem->GetItemType() == item->GetItemType();
	if (!isEqualItemType) return false;
	
	int addCount = fuelItem->AddItemCount(item->GetItemCount());
	item->MinusItemCount(addCount);

	return true;
}

bool FuelSystem::AddFuel(std::shared_ptr<ItemStack> item)
{
	//追加するアイテムが使用可能な燃料でないとき
	if (!IsEqualFuelType(item->GetItemType())) return false;

	//新規追加が必要なとき
	if (IsNeedAddNew()) return AddNewFuel(std::move(item));

	return AddFuelCount(std::move(item));
}

bool FuelSystem::UseFuel()
{
	if (!IsEqualFuelType(m_fuelSlot->GetItem(0)->GetItemType())) return false;
	
	if (!GetFuelRemain()) return false;

	m_useCount++;
	if (m_useCount % kConsumeItemPerUse) return false;

	m_useCount = 0;

	//燃料アイテムの取得
	const auto& fuelItem = m_fuelSlot->GetItem(kIndex);
	fuelItem->MinusItemCount(1);

	if (!fuelItem->GetItemCount()) m_fuelSlot->RemoveItem(kIndex);

	return true;
}

int FuelSystem::GetFuelRemain() const
{
	if (!m_fuelSlot) return 0 ;
	//燃料アイテムの取得
	const auto& fuelItem = m_fuelSlot->GetItem(kIndex);
	if (!fuelItem) return 0;

	return fuelItem->GetItemCount();
}

void FuelSystem::GetAllItemOwnership(std::vector<std::pair<std::shared_ptr<ItemStack>, int>>* result)
{
	if (!m_fuelSlot) return;

	for (int i = 0; i < m_fuelSlot->GetSlotCount(); i++)
	{
		auto item = m_fuelSlot->GetItemOwnership(i);
		if (!item) continue;
		auto itemCount = item->GetItemCount();
		result->push_back(std::make_pair(std::move(item), itemCount));
	}
}
