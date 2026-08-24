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

void FactoryManager::AddToFactoryComponents(const std::shared_ptr<FactoryComponent>& f)
{
	//親オブジェクトがシーンの管理下におかれていないなら追加しない。
	if (!SceneManager::GetInstance().GetCurrentScene()->HasSceneObjects(f->GetParentObject())) return;

	//すでにもっているときは処理をしない
	for (const auto& factoryComponent : m_factoryComponents)
	{
		if (factoryComponent == f) return;
	}
	m_factoryComponents.push_back(f);
}

/// <summary>
/// 与えられたグリッド座標にあるFactoryComponentを取得する。
/// ないときはnullptrを返す。
/// </summary>
std::shared_ptr<FactoryComponent> FactoryManager::GetComponentAtGridPos(const VectorInt& gridPos)
{
	for (const auto& com : m_factoryComponents)
	{
		if (IsReservedRemove(com)) continue;
		if (!com->GetParentObject()) continue;
		if (com->GetParentObject()->GetGridPosition() == gridPos) return com;
	}
	return nullptr;
}

bool FactoryManager::IsManageFactoryComponent(const std::shared_ptr<FactoryComponent>& f)
{
	for (const auto& com : m_factoryComponents)
	{
		if (IsReservedRemove(com)) continue;
		if (com == f) return true;
	}
	return false;
}

bool FactoryManager::IsReservedRemove(const std::shared_ptr<FactoryComponent>& f)
{
	for (const auto& com : m_removeFactoryComponents)
	{
		if (com == f) return true;
	}
	return false;
}

void FactoryManager::AddToRemoveFactoryComponents(const std::shared_ptr<FactoryComponent>& f)
{
	if (!IsManageFactoryComponent(f)) return;
	m_removeFactoryComponents.push_back(f);
}

void FactoryManager::RemoveFactoryComponents()
{
	for (const auto& com : m_removeFactoryComponents)
	{
		m_factoryComponents.erase(std::find(m_factoryComponents.begin(), m_factoryComponents.end(), com));
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
