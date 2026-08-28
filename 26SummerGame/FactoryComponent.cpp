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

	/*m_collider = GetComponent<SquareCollider3D>();
	if (!m_collider.lock())
		m_collider = AddComponent<SquareCollider3D>();*/
}

void FactoryComponent::Update()
{
	Component::Update();

#if false
	if (auto item = GetInputItemStack(0))
	{
		Debug::Log(std::format("tag: {}, type: {}, count: {}",
			GetParentObject()->GetTag(),
			ItemTable::ItemTypeToDisplayName(item->GetItemType()),
			item->GetItemCount()));
	}

	if (auto item = GetOutputItemStack(0))
	{
		Debug::Log(std::format("tag: {}, type: {}, count: {}",
			GetParentObject()->GetTag(),
			ItemTable::ItemTypeToDisplayName(item->GetItemType()),
			item->GetItemCount()));
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

std::unique_ptr<ItemStack> FactoryComponent::MakeItemStackFromThisComponent()
{
	return nullptr;
}

ItemStack* FactoryComponent::GetInputItemStack(int index) const
{
	if (!m_inputSlot) return nullptr;
	
	return m_inputSlot->GetItem(index);
}

ItemStack* FactoryComponent::GetOutputItemStack(int index) const
{
	if (!m_outputSlot) return nullptr;

	return m_outputSlot->GetItem(index);
}

void FactoryComponent::SetSizeAndColorAndMaxSlot(const Vector& siz, unsigned int col, int maxInputSlot, int maxOutputSlot)
{
	m_shape.lock()->SetSize(siz);
	m_shape.lock()->SetColor(col);
	/*m_collider.lock()->SetSize(siz);
	m_collider.lock()->IsVisible(true);
	m_collider.lock()->SetMass(0);*/

	m_inputSlot = std::make_shared<ItemSlot>(maxInputSlot);
	m_outputSlot = std::make_shared<ItemSlot>(maxOutputSlot);
}

void FactoryComponent::SetTimerDuration(Second sec)
{
	m_timer = std::make_unique<Timer>(sec);
}

bool FactoryComponent::TryInsert(ItemSlot* itemSlot, std::unique_ptr<ItemStack> item)
{
	auto myItemStack = itemSlot->GetItem(item->GetItemType());

	if (!myItemStack)
	{
		//アイテム追加に失敗したときに処理を終了する。
		if (itemSlot->AddItemStack(std::move(item))) return false;

	}
	else
	{
		myItemStack->AddItemCount(item->GetItemCount());
	}
	return true;
}

bool FactoryComponent::TryInsert(ItemSlot* itemSlot, ItemStack* item, int count)
{
	auto myItemStack = itemSlot->GetItem(item->GetItemType());

	if (!myItemStack)
	{
		//新しくアイテムスタックを作成して追加する。
		//渡されたアイテムの個数か指定された個数のうち小さい値で作成する。
		auto newItemStack = std::make_unique<ItemStack>(item->GetItemType(), min(item->GetItemCount(), count));

		//アイテム追加に失敗したときに処理を終了する。
		if (itemSlot->AddItemStack(std::move(newItemStack))) return false;

		//呼び出し元のアイテムの数をカウント分減少させる。
		item->MinusItemCount(count);

		if (item->GetItemCount()) itemSlot->RemoveItem(item);

		return true;
	}
	else
	{
		//このコンポーネントのアイテム数がいくつ増加するかをチェックする
		int checkAddCount = myItemStack->CheckAddItemCount(count);

		//呼び出し元のアイテム数がいくつ減少するかをチェックする。
		int checkMinusCount = item->CheckMinusItemCount(count);

		//小さいほうの値を取得
		int smallerValue = min(checkAddCount, checkMinusCount);

		myItemStack->AddItemCount(smallerValue);

		item->MinusItemCount(smallerValue);

		if (item->GetItemCount()) itemSlot->RemoveItem(item);

		return true;
	}
	return false;
}
