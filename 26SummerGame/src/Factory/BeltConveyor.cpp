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

BeltConveyor::BeltConveyor(std::weak_ptr<Object> parentObject):
	FactoryComponent(parentObject)
{
}

void BeltConveyor::Init()
{
	FactoryComponent::Init();

	m_shape = GetComponent<Model>();
	if (!m_shape.lock())
		m_shape = AddComponent<Model>();

	m_model = GetComponent<Model>();

	m_transport = AddComponent<TransportSystem>();

	if (const auto& safe = m_model.lock())
	{
		safe->SetModel(ModelId::kBeltConveyor);
		safe->SetScroll(0, -0.01f);
	}

	SetSizeAndColorAndMaxSlot(kSize, kColor, kMaxInputItemSlot, kMaxOutputItemSlot);
	SetItemName(kItemType);
	SetTimerDuration(kMainProcessDuration);

}

void BeltConveyor::Update()
{
	FactoryComponent::Update();

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
	return FactoryComponent::TryInsert(m_outputSlot.get(), item, count);
}

void BeltConveyor::UpdateUIPanel()
{
	const auto item = GetOutputItemStack(0);

	if (!item) return;
	
	m_itemUI->SetGraphicID(item->GetItemIconGraphicID());
	m_itemUI->SetText("x{}", item->GetItemCount());

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
