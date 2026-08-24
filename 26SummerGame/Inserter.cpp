#include "Inserter.h"
#include "TransportSystem.h"
#include "Object.h"
#include "Square3D.h"
#include "Timer.h"
#include "ItemType.h"
#include "UIItemBox.h"
#include "UIText.h"
#include "Game.h"
#include "ItemStack.h"

namespace
{
	//オブジェクトにつけるタグ
	const std::string kObjectTag = "Inserter";

	//アイテムの名前
	const Item kItemType = Item::kInserter;

	const Vector kSize = { 200,100,100 };

	constexpr unsigned int kColor = 0xFFA500;

	constexpr int kMaxInputItemSlot = 1;

	constexpr int kMaxOutputItemSlot = 1;

	constexpr Second kMainProcessDuration = 1.0f;

	/*UI要素の定数*/
	const Vector kInputUIPos = Game::kDisplaySize / 2;

	const Vector kUISize = { Game::kDisplaySize.m_x / 20, Game::kDisplaySize.m_x / 20 };
}

Inserter::Inserter(std::shared_ptr<Object> parentObject) :
	FactoryComponent(parentObject)
{

}

void Inserter::Init()
{
	FactoryComponent::Init();

	m_shape = GetParentObject()->GetComponent<Square3D>();
	if (!m_shape)
		m_shape = GetParentObject()->AddComponent<Square3D>();

	m_transport = GetParentObject()->AddComponent<TransportSystem>();

	const auto& square3D = std::static_pointer_cast<Square3D>(m_shape);
	square3D->SetUVScrollTexHandle(GraphicId::kInserterTop);
	square3D->SetUVScrollOffset(1);

	SetSizeAndColorAndMaxSlot(kSize, kColor, kMaxInputItemSlot, kMaxOutputItemSlot);
	SetItemName(kItemType);

	SetTimerDuration(kMainProcessDuration);
}

void Inserter::Update(float deltaTime)
{
	FactoryComponent::Update(deltaTime);

	//前の処理から指定時間まだ経過していない
	if (!m_timer->IsTimeOver()) return;

	m_transport.lock()->SetInputAndOutputObject();

	m_transport.lock()->TransportItemInputToMyself(std::static_pointer_cast<Inserter>(shared_from_this()));

	m_transport.lock()->TransportItemMyselfToOutput(std::static_pointer_cast<Inserter>(shared_from_this()));

	m_timer->ResetStartTime();
}

void Inserter::UpdateUIPanel()
{
	const auto item = GetOutputItemStack(0);

	if (!item) return;

	m_itemUI->SetGraphicID(item->GetItemIconGraphicID());
	m_itemUI->SetText("x{}", item->GetItemCount());
}

void Inserter::SetRotationAngle(Radian angle)
{
	FactoryComponent::SetRotationAngle(angle);

	const auto& safeTransport = m_transport.lock();
	if (!safeTransport) return;

	safeTransport->SetRotation(angle);
}

void Inserter::SetIsPreviewMode(bool b, int alpha)
{
	FactoryComponent::SetIsPreviewMode(b, alpha);
	m_transport.lock()->SetIsPreviewMode(b);
}

Radian Inserter::GetRotationAngle() const
{
	const auto& safeTransport = m_transport.lock();
	if (!safeTransport) return 0.0f;
	return safeTransport->GetRotation();
}

VectorInt Inserter::GetOutputDir() const
{
	const auto& safeTransport = m_transport.lock();
	if (!safeTransport) return { -1,-1,-1 };
	return safeTransport->GetOutputDir();
}

bool Inserter::TryInsert(ItemStack* item, int count)
{
	return FactoryComponent::TryInsert(m_outputSlot.get(), item, count);
}

void Inserter::BuildUIPanel()
{
	m_itemUI = std::make_shared<UIItemBox>(m_uiPanel, kInputUIPos, kUISize);

}
