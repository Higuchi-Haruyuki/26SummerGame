#include "BeltConveyor.h"
#include "Debug.h"
#include "DrawText.h"
#include "FactoryComponent.h"
#include "Game.h"
#include "ObjectFactory.h"
#include "ItemStack.h"
#include "Object.h"
#include "PlayerInput.h"
#include "Square3D.h"
#include "TransportSystem.h"
#include "UIManager.h"
#include "vector.h"
#include "VectorInt.h"
#include <DxLib.h>
#include <format>
#include <functional>
#include <memory>
#include <string>
#include <cmath>
#include <algorithm>
#include "ItemSlot.h"
#include "SquareCollider3D.h"
#include "Model.h"
#include "Timer.h"
#include "ItemType.h"
#include "UIItemBox.h"
#include "UIText.h"

namespace
{
	//オブジェクトにつけるタグ
	const std::string kObjectTag = "BeltConveyor";

	constexpr Item kItemType = Item::kBeltconveyor;

	const Vector kSize = { 200,50,150 };

	constexpr unsigned int kColor = 0xFFA500;

	constexpr int kMaxInputItemSlot = 1;

	constexpr int kMaxOutputItemSlot = 1;

	//一秒間当たりどのくらいのアイテムを処理するか
	constexpr int kProcessItemPerSecond = 1;

	constexpr Second kMainProcessDuration = 1.0f;

	constexpr int kItemCapacity = 4;

	constexpr float kItemInterval = 1.0f / kItemCapacity;

	constexpr float kBeltSpeed = 1.0f;

	//流れているオブジェクトの見た目のオフセット
	const Vector kObjectOffset = { 0,40,0 };

	/*UI要素の定数*/
	const Vector kInputUIPos = Game::kDisplaySize / 2;

	const Vector kUISize = { Game::kDisplaySize.m_x / 20, Game::kDisplaySize.m_x / 20};

}

BeltConveyor::BeltConveyor(std::shared_ptr<Object> parentObject):
	FactoryComponent(parentObject)
{
}

void BeltConveyor::Init()
{
	FactoryComponent::Init();

	m_shape = GetParentObject()->GetComponent<Model>();
	if (!m_shape)
		m_shape = GetParentObject()->AddComponent<Model>();

	m_model = GetParentObject()->GetComponent<Model>();

	m_transport = GetParentObject()->AddComponent<TransportSystem>();

	if (const auto& safe = m_model.lock())
	{
		safe->SetModel(ModelId::kBeltConveyor);
		safe->SetScroll(0, -0.01f);
	}

	SetSizeAndColorAndMaxSlot(kSize, kColor, kMaxInputItemSlot, kMaxOutputItemSlot);
	SetItemName(kItemType);
	SetTimerDuration(kMainProcessDuration);

}

void BeltConveyor::Update(float deltaTime)
{
	FactoryComponent::Update(deltaTime);

	if (m_isPreviewMode) return;
	//前の処理から指定時間まだ経過していない
	if (!m_timer->IsTimeOver()) return;

	m_transport.lock()->SetInputAndOutputObject<BeltConveyor>();

	m_transport.lock()->TransportItemMyselfToOutput(std::static_pointer_cast<BeltConveyor>(shared_from_this()));

	//AdvanceItems(deltaTime);
	//TryOutput();
	//DrawItems();

	m_timer->ResetStartTime();
}

bool BeltConveyor::TryInsert(ItemStack* item, int count)
{
	//if (!item || item->GetSlotCount() <= 0) return false;

	////入口に空きが無い（末尾アイテクがまだ入口付近にいる）
	//if (!m_beltItems.empty() &&
	//	m_beltItems.back().m_progress < kItemInterval) return false;

	//m_beltItems.push_back(
	//	{ std::make_unique<ItemStack>(item->GetItemType(), 1), 0.0f });

	//item->MinusItemCount(1);
	//return true;

	return FactoryComponent::TryInsert(m_outputSlot.get(), item, count);
}

void BeltConveyor::UpdateUIPanel()
{
	const auto item = GetOutputItemStack(0);

	if (!item) return;
	
	m_itemUI->SetGraphicID(item->GetItemIconGraphicID());
	m_itemUI->SetText("x{}", item->GetItemCount());

}

std::unique_ptr<ItemStack> BeltConveyor::MakeItemStackFromThisComponent()
{
	return ItemStack::MakeItemStack<BeltConveyor>(kObjectTag, kItemType, 1);
}

Radian BeltConveyor::GetRotationAngle() const
{
	const auto& safeTransport = m_transport.lock();
	if (!safeTransport) return 0.0f;
	return safeTransport->GetRotation();
}

VectorInt BeltConveyor::GetOutputDir() const
{
	const auto& safeTransport = m_transport.lock();
	if (!safeTransport) return { -1,-1,-1 };
	return safeTransport->GetOutputDir();
}

void BeltConveyor::BuildUIPanel()
{
	m_itemUI = std::make_shared<UIItemBox>(m_uiPanel, kInputUIPos, kUISize);

}

void BeltConveyor::AdvanceItems(float deltaTime)
{
	float limit = 1.0f;

	for (auto& beltItem : m_beltItems)
	{
		beltItem.m_progress = min(beltItem.m_progress + kBeltSpeed * deltaTime, limit);

		limit = beltItem.m_progress - kItemInterval;
	}

}

void BeltConveyor::TryOutput()
{
	if (m_beltItems.empty()) return;

	auto& head = m_beltItems.front();
	if (head.m_progress < 1.0f) return;

	const auto& safeTransport = m_transport.lock();
	if (!safeTransport) return;

	const auto& output = safeTransport->GetOutputObject().lock();
	if (!output) return;

	if (!output->TryInsert(head.m_item.get(), 1))return;
	
	m_beltItems.pop_front();

}

void BeltConveyor::DrawItems() const
{
	for (const auto& beltItem : m_beltItems)
	{
		beltItem.m_item->DrawModel(
			GetItemWorldPos(beltItem.m_progress) + kObjectOffset
		);
	}

}

Vector BeltConveyor::GetItemWorldPos(float progress) const
{
	Vector dir = { static_cast<float>(Game::kGridSize),0,0 };
	dir.RotateY(GetRotationAngle());

	return GetParentObject()->GetPosition() + dir * (progress - 0.5f);
}

void BeltConveyor::SetRotationAngle(Radian angle)
{
	FactoryComponent::SetRotationAngle(angle);

	const auto& safeTransport = m_transport.lock();
	if (!safeTransport) return;

	safeTransport->SetRotation(angle);
}

void BeltConveyor::SetIsPreviewMode(bool b, int alpha)
{
	FactoryComponent::SetIsPreviewMode(b,alpha);
	m_transport.lock()->SetIsPreviewMode(b);
}
