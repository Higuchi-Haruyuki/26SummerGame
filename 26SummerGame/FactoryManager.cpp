#include "FactoryManager.h"
#include "FactoryComponent.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Object.h"
#include <algorithm>
#include "ItemType.h"


FactoryManager& FactoryManager::GetInstance()
{
	static FactoryManager instance;
	return instance;
	// TODO: return ステートメントをここに挿入します
}

void FactoryManager::AddToFactoryComponents(std::weak_ptr<FactoryComponent> f)
{
	const auto& safeFactoryComponent = f.lock();
	if (!safeFactoryComponent) return;
	//親オブジェクトがシーンの管理下におかれていないなら追加しない。
	if (!SceneManager::GetInstance().GetCurrentScene()->HasSceneObjects(safeFactoryComponent->GetParentObject())) return;

	//すでにもっているときは処理をしない
	for (const auto& factoryComponent : m_factoryComponents)
	{
		if (factoryComponent.lock() == safeFactoryComponent) return;
	}
	m_factoryComponents.push_back(f);
}

/// <summary>
/// 与えられたグリッド座標にあるFactoryComponentを取得する。
/// ないときはnullptrを返す。
/// </summary>
std::weak_ptr<FactoryComponent> FactoryManager::GetComponentAtGridPos(const VectorInt& gridPos)
{
	for (const auto& com : m_factoryComponents)
	{
		if (com.lock() == nullptr) continue;
		if (IsReservedRemove(com)) continue;
		if (!com.lock()->GetParentObject().lock()) continue;
		if (com.lock()->GetParentObject().lock()->GetGridPosition() == gridPos) return com;
	}
	return std::weak_ptr<FactoryComponent>();
}

bool FactoryManager::IsManageFactoryComponent(std::weak_ptr<FactoryComponent> f)
{
	for (const auto& com : m_factoryComponents)
	{
		if (com.lock() == nullptr) continue;
		if (IsReservedRemove(com)) continue;
		if (com.lock() == f.lock()) return true;
	}
	return false;
}

bool FactoryManager::IsReservedRemove(std::weak_ptr<FactoryComponent> f)
{
	for (const auto& com : m_removeFactoryComponents)
	{
		if (f.lock() == nullptr) continue;
		if (com.lock() == f.lock()) return true;
	}
	return false;
}

void FactoryManager::AddToRemoveFactoryComponents(std::weak_ptr<FactoryComponent> f)
{
	if (!IsManageFactoryComponent(f)) return;
	m_removeFactoryComponents.push_back(f.lock());
}

void FactoryManager::RemoveFactoryComponents()
{
	for (const auto& com : m_removeFactoryComponents)
	{
		const auto& safeCom = com.lock();
		if (!safeCom) continue;
		std::erase_if(m_factoryComponents, [&safeCom](const std::weak_ptr<FactoryComponent>& f) { return f.lock() == safeCom; });
	}
	m_removeFactoryComponents.clear();
}

void FactoryManager::OnMakeItem(Item itemName, int count)
{
	auto it = m_itemMakeCount.find(itemName);
	if (it == m_itemMakeCount.end())
		m_itemMakeCount.emplace(itemName, count);
	else
		it->second += count;
}
