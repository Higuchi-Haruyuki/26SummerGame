
#include "FactoryComponent.h"
#include "FactoryManager.h"
#include "TransportSystem.h"
#include "Object.h"
#include "Debug.h"

TransportSystem::TransportSystem(std::shared_ptr<Object> parentObject) :
	Component(parentObject),
	m_factoryManager(FactoryManager::GetInstance())
{
}

void TransportSystem::Init()
{
}

void TransportSystem::Update(float deltaTime)
{
	Component::Update(deltaTime);
	
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
	return GetParentObject()->GetGridPosition() - GetOutputDir();
}

VectorInt TransportSystem::GetOutputPos() const
{
	return GetParentObject()->GetGridPosition() + GetOutputDir();
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

	output->TryInsert(moveItem, 1);

}

void TransportSystem::SetOutputObject(const std::shared_ptr<FactoryComponent>& obj)
{
	//自分自身のポインタを取得
	const auto& self = GetParentObject()->GetComponent<FactoryComponent>();
	if (!self) return;

	//このコンポーネントが管理下にないなら入出力の処理をしない
	if (!m_factoryManager.IsManageFactoryComponent(self)) return;

	//対象の工業オブジェクトがプレビューモードのとき
	if (obj->GetIsPreviewMode()) return;

	m_output = obj;

}

void TransportSystem::SetInputObject(const std::shared_ptr<FactoryComponent>& obj)
{
	//自分自身のポインタを取得
	const auto& self = GetParentObject()->GetComponent<FactoryComponent>();
	if (!self) return;

	//このコンポーネントが管理下にないなら入出力の処理をしない
	if (!m_factoryManager.IsManageFactoryComponent(self)) return;

	//対象の工業オブジェクトがプレビューモードのとき
	if (obj->GetIsPreviewMode()) return;

	m_input = obj;
}

std::shared_ptr<FactoryComponent> TransportSystem::GetOutputObjectFromOutputDir()
{
	auto myGridPos = GetParentObject()->GetGridPosition();

	//出力先コンポーネントの設定
	auto targetGridPos = myGridPos + GetOutputDir();
	const auto& outputComponent = m_factoryManager.GetComponentAtGridPos(targetGridPos);

	return outputComponent;
}

std::shared_ptr<FactoryComponent> TransportSystem::GetInputObjectFromOutputDir()
{
	auto myGridPos = GetParentObject()->GetGridPosition();

	//入力元コンポーネントの設定
	auto targetGridPos = myGridPos - GetOutputDir();
	const auto& inputComponent = m_factoryManager.GetComponentAtGridPos(targetGridPos);

	return inputComponent;
}
