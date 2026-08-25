#include "ItemStack.h"
#include <DxLib.h>
#include <string>
#include "vector.h"
#include "ResourceManager.h"
#include <functional>
#include "VectorInt.h"
#include <memory>
#include "ObjectFactory.h"
#include "FactoryComponent.h"
#include <concepts>
#include "Object.h"
#include "ItemType.h"
#include "ItemManager.h"
#include "ResourceType.h"

namespace
{
	constexpr int kMaxItemStack = 100;
	constexpr int kPreviewObjectAlpha = 100;
}


ItemStack::ItemStack(Item itemType, int itemCount)
	: ItemStack(itemType,itemCount,nullptr,nullptr)
{
}

ItemStack::ItemStack(Item itemType, int itemCount,
	const std::function<std::shared_ptr<Object>(const VectorInt& gridPos, float rotationAngle)>& func, 
	const std::function<std::shared_ptr<Object>()>& previewFunc)
	: m_itemManager(ItemManager::GetInstance()),
	m_itemType(itemType), m_itemCount(itemCount),m_installationItem(func),m_generatePreviewObject(previewFunc)
{
	SetHandle(itemType);
}


void ItemStack::SetItemType(Item itemType)
{
	m_itemType = itemType;
	SetHandle(itemType);
}

void ItemStack::SetItemCount(int itemCount)
{
	if (itemCount > kMaxItemStack)
		itemCount = kMaxItemStack;
	m_itemCount = itemCount;
}

void ItemStack::SetModelHandle(ModelId modelId)
{
	int handle = ResourceManager::GetInstance().GetHandle(modelId);
	m_modelHandle = handle;
}

bool ItemStack::MoveItemStack(ItemStack* from, int count)
{
	if (!from) return false;
	if (count < 0) return false;

	int plusValue =  CheckAddItemCount(count);
	int minusValue = from->CheckMinusItemCount(count);

	int value = min(plusValue, minusValue);

	AddItemCount(value);
	from->MinusItemCount(value);

	return true;
}

int ItemStack::AddItemCount(int itemCount)
{
	if (itemCount < 0) return 0;

	int addCount = CheckAddItemCount(itemCount);
	m_itemCount += addCount;
	return addCount;

}

int ItemStack::CheckAddItemCount(int itemCount) const
{
	if (itemCount < 0) return 0;
	//アイテムを追加した結果最大数を超えるとき
	if ((m_itemCount + itemCount) > kMaxItemStack)
	{
		return kMaxItemStack - m_itemCount;
	}
	//超えないとき
		return itemCount;
}

int ItemStack::MinusItemCount(int itemCount)
{
	if (itemCount < 0) return 0;
	int minusCount = CheckMinusItemCount(itemCount);
	m_itemCount -= minusCount;
	return minusCount;
}

int ItemStack::CheckMinusItemCount(int itemCount) const
{
	if (itemCount < 0 ) return 0;
	//アイテムが減少した結果0を超えるとき
	if ((m_itemCount - itemCount) < 0)
	{
		return m_itemCount;
	}
	//超えないとき
	return itemCount;
}

void ItemStack::DrawModel(const Vector& v) const
{
	if (m_modelHandle == -1) return;
	MV1SetPosition(m_modelHandle, v.ToVECTOR());
	MV1DrawModel(m_modelHandle);
}

std::weak_ptr<Object> ItemStack::Installation(const VectorInt& gridPos, float rotationAngle)
{
	if (!m_installationItem) return std::weak_ptr<Object>();

	return m_installationItem(gridPos, rotationAngle);
}

std::weak_ptr<Object> ItemStack::GeneratePreviewObject()
{
	if (!m_generatePreviewObject) return std::weak_ptr<Object>();
	m_previewObject = m_generatePreviewObject();
	m_previewObject.lock()->GetComponent<FactoryComponent>()->SetIsPreviewMode(true, kPreviewObjectAlpha);
	return m_previewObject;
}

void ItemStack::RemovePreviewObject()
{
	const auto& safePreviewObject = m_previewObject.lock();
	if (!safePreviewObject) return;

	safePreviewObject->Destroy();

	m_previewObject.reset();
}

void ItemStack::RotationPreviewObject(Radian rotationAngle)
{
	const auto& safePreviewObject = m_previewObject.lock();
	if (!safePreviewObject) return;

	safePreviewObject->GetComponent<FactoryComponent>()->SetRotationAngle(rotationAngle);
}

void ItemStack::SetPositionPreviewObject(const Vector& pos)
{
	const auto& safePreviewObject = m_previewObject.lock();
	if (!safePreviewObject) return;

	safePreviewObject->SetPosition(Game::GridPosToWorldPos(Game::WorldPosToGridPos(pos)));
}

void ItemStack::SetHandle(Item itemType)
{
	auto it = ItemTable::FindItemType(itemType);
	SetModelHandle(it->Model);

	m_itemIconResourceName = it->Icon;
}
