
#include "FactoryComponent.h"
#include "FactoryManager.h"
#include "TransportSystem.h"
#include "Object.h"
#include "Debug.h"
#include "ItemStack.h"
#include "ItemSlot.h"

TransportSystem::TransportSystem(std::weak_ptr<Object> parentObject) :
	Component(parentObject),
	m_factoryManager(FactoryManager::GetInstance())
{
}

void TransportSystem::Init()
{
}

void TransportSystem::Update()
{
	Component::Update();
	
	if (!m_isEnable) return;

#if false
	auto myGridPos = GetParentObject()->GetGridPosition();

	Debug::Log(std::format("rotationAngle: {}, inputPos: {}, outputPos: {}",
		m_rotationAngle,
		(myGridPos - GetOutputDir()).ToString(),
		(myGridPos + GetOutputDir()).ToString()
	));
#endif

	if (m_isPreviewMode) return;

	//GetInputObjectFromOutputDir();
	//GetOutputObjectFromOutputDir();
}

VectorInt TransportSystem::GetInputPos() const
{
	return GetParentObject().lock()->GetGridPosition() - GetOutputDir();
}

VectorInt TransportSystem::GetOutputPos() const
{
	return GetParentObject().lock()->GetGridPosition() + GetOutputDir();
}

bool TransportSystem::TransportItemInputToMyself(std::shared_ptr<FactoryComponent> myself)
{
	const auto& safeInput = m_input.lock();
	if (!safeInput) return false;

	TransportItem(safeInput, myself);

	return true;
}

bool TransportSystem::TransportItemMyselfToOutput(std::shared_ptr<FactoryComponent> myself)
{
	const auto& safeOutput = m_output.lock();
	if (!safeOutput) return false;

	TransportItem(myself, safeOutput);

	return true;
}

void TransportSystem::TransportItem(std::shared_ptr<FactoryComponent> input, std::shared_ptr<FactoryComponent> output)
{
	//移動するアイテムを取得
	const auto& moveItem = input->GetOutputItemStack(0);

	if (!moveItem) return;

	if (!output->TryInsert(moveItem, 1))return;

	if (moveItem->GetItemCount() <= 0)
	{
		input->GetOutputItemSlot().lock()->RemoveItem(0);
	}

}

void TransportSystem::SetOutputObject(std::shared_ptr<FactoryComponent> obj)
{
	//自分自身のポインタを取得
	const auto& self = GetComponent<FactoryComponent>().lock();
	if (!self) return;

	//このコンポーネントが管理下にないなら入出力の処理をしない
	if (!m_factoryManager.IsManageFactoryComponent(self)) return;

	//対象の工業オブジェクトがプレビューモードのとき
	if (obj->GetIsPreviewMode()) return;

	m_output = obj;

}

void TransportSystem::SetInputObject(std::shared_ptr<FactoryComponent> obj)
{
	//自分自身のポインタを取得
	const auto& self = GetComponent<FactoryComponent>().lock();
	if (!self) return;

	//このコンポーネントが管理下にないなら入出力の処理をしない
	if (!m_factoryManager.IsManageFactoryComponent(self)) return;

	//対象の工業オブジェクトがプレビューモードのとき
	if (obj->GetIsPreviewMode()) return;

	m_input = obj;
}

std::weak_ptr<FactoryComponent> TransportSystem::GetOutputObjectFromOutputDir()
{
	auto myGridPos = GetParentObject().lock()->GetGridPosition();

	//出力先コンポーネントの設定
	auto targetGridPos = myGridPos + GetOutputDir();
	const auto& outputComponent = m_factoryManager.GetComponentAtGridPos(targetGridPos);

	return outputComponent;
}

std::weak_ptr<FactoryComponent> TransportSystem::GetInputObjectFromOutputDir()
{
	auto myGridPos = GetParentObject().lock()->GetGridPosition();

	//入力元コンポーネントの設定
	auto targetGridPos = myGridPos - GetOutputDir();
	const auto& inputComponent = m_factoryManager.GetComponentAtGridPos(targetGridPos);

	return inputComponent;
}
