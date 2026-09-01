#include "FactoryComponent.h"
#include "ItemStack.h"
#include "FactoryManager.h"
#include "Object.h"
#include "Timer.h"
#include "VectorInt.h"
#include <memory>
#include <DxLib.h>
#include <utility>
#include "Component.h"
#include <string>
#include "vector.h"
#include "RecipeManager.h"
#include "ResourceManager.h"
#include "SquareCollider3D.h"
#include "Square3D.h"
#include "PlayerInput.h"
#include "UIManager.h"
#include "ItemSlot.h"
#include <cmath>
#include "ItemManager.h"
#include "UIPanel.h"

namespace
{
	constexpr int kDebugArrowOffsetY = 50;
	constexpr int kDebugArrowSize = 50;
}


FactoryComponent::FactoryComponent(std::weak_ptr<Object> parentObject) :
	Component(parentObject),
	m_factoryManager(FactoryManager::GetInstance()),
	m_recipeManager(RecipeManager::GetInstance())
{
}

FactoryComponent::~FactoryComponent() = default;


void FactoryComponent::Init()
{
	Component::Init();

	m_factoryManager.AddToFactoryComponents(std::dynamic_pointer_cast<FactoryComponent>(shared_from_this()));

	m_collider = GetComponent<SquareCollider3D>();
	if (!m_collider.lock())
		m_collider = AddComponent<SquareCollider3D>();
	m_collider.lock()->SetTrigger(true);
}

void FactoryComponent::Update()
{
	Component::Update();

#if false
	if (auto toItem = GetInputItemStack(0))
	{
		Debug::Log(std::format("tag: {}, type: {}, count: {}",
			GetParentObject()->GetTag(),
			ItemTable::ItemTypeToDisplayName(toItem->GetItemType()),
			toItem->GetItemCount()));
	}

	if (auto toItem = GetOutputItemStack(0))
	{
		Debug::Log(std::format("tag: {}, type: {}, count: {}",
			GetParentObject()->GetTag(),
			ItemTable::ItemTypeToDisplayName(toItem->GetItemType()),
			toItem->GetItemCount()));
	}
#endif

	if (m_isPreviewMode) return;
}

void FactoryComponent::Finalize()
{
	Component::Finalize();
	FactoryManager::GetInstance().AddToRemoveFactoryComponents
	(std::dynamic_pointer_cast<FactoryComponent>(shared_from_this()));
}

bool FactoryComponent::TryInsert(ItemStack* item, int count)
{
	return TryInsert(m_inputSlot.get(), item, count);
}

std::shared_ptr<UIPanel> FactoryComponent::GetOrBuildUIPanel()
{
	if (!m_uiPanel)
	{
		m_uiPanel = std::make_shared<UIPanel>();
		BuildUIPanel();
	}
	return m_uiPanel;
}

void FactoryComponent::GetAllItemOwnership(ItemContainer* result)
{
	const auto& inputItemSlot = GetInputItemSlot().lock();

	if (!inputItemSlot) return;

	for (int i = 0; i < inputItemSlot->GetSlotCount(); i++)
	{
		auto item = inputItemSlot->GetItemOwnership(i);
		if (!item) continue;
		auto itemCount = item->GetItemCount();
		result->push_back(std::make_pair(std::move(item), itemCount));
	}

	const auto& outputItemSlot = GetOutputItemSlot().lock();

	for (int i = 0; i < outputItemSlot->GetSlotCount(); i++)
	{
		auto item = outputItemSlot->GetItemOwnership(i);
		if (!item) continue;
		auto itemCount = item->GetItemCount();
		result->push_back(std::make_pair(std::move(item), itemCount));
	}
}

void FactoryComponent::SetRotationAngle(Radian angle)
{
	m_shape.lock()->SetRotationAngle(-angle);
	//m_collider.lock()->SetRotationAngle(angle);
}

void FactoryComponent::SetIsPreviewMode(bool b,int alpha)
{
	//m_collider.lock()->IsEnable(!b);
	m_shape.lock()->SetAlpha(alpha);
	m_isPreviewMode = true;
}

ItemStack* FactoryComponent::GetInputItemStack(int index) const
{
	if (!m_inputSlot) return nullptr;
	
	if (index < 0 || index >= m_inputSlot->GetSlotCount()) return nullptr;

	return m_inputSlot->GetItem(index);
}

ItemStack* FactoryComponent::GetOutputItemStack(int index) const
{
	if (!m_outputSlot) return nullptr;

	if (index < 0 || index >= m_outputSlot->GetSlotCount()) return nullptr;

	return m_outputSlot->GetItem(index);
}

void FactoryComponent::SetSizeAndColorAndMaxSlot(const Vector& siz, unsigned int col, int maxInputSlot, int maxOutputSlot)
{
	m_shape.lock()->SetSize(siz);
	m_shape.lock()->SetColor(col);
	m_collider.lock()->SetSize(siz);
	m_collider.lock()->IsVisible(true);
	m_collider.lock()->SetMass(0);

	m_inputSlot = std::make_shared<ItemSlot>(maxInputSlot);
	m_outputSlot = std::make_shared<ItemSlot>(maxOutputSlot);
}

void FactoryComponent::SetTimerDuration(Second sec)
{
	m_timer = std::make_unique<Timer>(sec);
}

bool FactoryComponent::TryInsert(ItemSlot* itemSlot, std::shared_ptr<ItemStack> item)
{
	auto myItemStack = itemSlot->GetItem(item->GetItemType());

	if (!myItemStack)
	{
		//アイテム追加に失敗したときに処理を終了する。
		if (itemSlot->AddItem(std::move(item))) return false;

	}
	else
	{
		myItemStack->AddItemCount(item->GetItemCount());
	}
	return true;
}

bool FactoryComponent::TryInsert(ItemSlot* toItemSlot, ItemStack* fromItem, int count)
{
	auto myItemStacks = toItemSlot->GetItems(fromItem->GetItemType());

	if (!myItemStacks.size())
	{
		//新しくアイテムスタックを作成して追加する。
		//渡されたアイテムの個数か指定された個数のうち小さい値で作成する。
		auto newItemStack = std::make_shared<ItemStack>(fromItem->GetItemType(), min(fromItem->GetItemCount(), count));

		//アイテム追加に失敗したときに処理を終了する。
		if (toItemSlot->AddItem(std::move(newItemStack))) return false;

		//呼び出し元のアイテムの数をカウント分減少させる。
		fromItem->MinusItemCount(count);

		if (fromItem->GetItemCount()) toItemSlot->RemoveItem(fromItem);

		return true;
	}
	else
	{

		auto remainCount = count;
		for (auto toItem : myItemStacks)
		{
			if (remainCount <= 0) break;

			//このコンポーネントのアイテム数がいくつ増加するかをチェックする
			auto checkAddCount = toItem->CheckAddItemCount(remainCount);

			//呼び出し元のアイテム数がいくつ減少するかをチェックする。
			auto checkMinusCount = fromItem->CheckMinusItemCount(remainCount);

			//小さいほうの値を取得
			auto smallerValue = min(checkAddCount, checkMinusCount);

			toItem->AddItemCount(smallerValue);

			fromItem->MinusItemCount(smallerValue);

			remainCount -= smallerValue;

			if (remainCount <= 0) break;
		}

		if (fromItem->GetItemCount()) toItemSlot->RemoveItem(fromItem);

		return true;
	}
	return false;
}
