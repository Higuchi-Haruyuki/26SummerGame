#pragma once
#include <functional>
#include <vector>
#include <map>
#include <memory>
#include <string>

class VectorInt;
class Object;
class FactoryComponent;

enum class Item;

class FactoryManager
{
public:
	// シングルトンのインスタンスを返します
	static FactoryManager& GetInstance();

	void AddToFactoryComponents(const std::shared_ptr<FactoryComponent>& f);

	std::shared_ptr<FactoryComponent> GetComponentAtGridPos(const VectorInt& gridPos);

	/// <summary>
	/// 引数で渡されたコンポーネントが管理下が管理下にあるかを返す。
	/// </summary>
	bool IsManageFactoryComponent(const std::shared_ptr<FactoryComponent>& f);

	bool IsReservedRemove(const std::shared_ptr<FactoryComponent>& f);

	void AddToRemoveFactoryComponents(const std::shared_ptr<FactoryComponent>& f);
	
	void RemoveFactoryComponents();

	/// <summary>
	/// アイテムを作成したときに呼び出す
	/// </summary>
	/// <param name="itemName"></param>
	void OnMakeItem(Item itemName, int count);

	/// <summary>
	/// アイテムの制作個数を保存している配列を返す。
	/// </summary>
	/// <returns></returns>
	std::map<Item, unsigned int> GetItemMakeCountMap() const { return m_itemMakeCount; }

	/// <summary>
	/// 指定したアイテム名が何個作られているか返す。
	/// </summary>
	/// <param name="itemName"></param>
	/// <returns></returns>
	unsigned int GetItemMakeCount(Item itemName) const
	{
		int itemCount = 0;
		auto it = m_itemMakeCount.find(itemName);
		if (it != m_itemMakeCount.end())
			itemCount = it->second;
		return itemCount;
	}

	int GetFactoryComponentCount() const
	{
		return m_factoryComponents.size();
	}

private:
	FactoryManager() = default;
	~FactoryManager() = default;
	FactoryManager(const FactoryManager&) = delete;
	FactoryManager& operator=(const FactoryManager&) = delete;
	FactoryManager(FactoryManager&&) = delete;
	FactoryManager& operator=(FactoryManager&&) = delete;

private:
	std::vector<std::shared_ptr<FactoryComponent>> m_factoryComponents;

	std::vector<std::shared_ptr<FactoryComponent>> m_removeFactoryComponents;

	std::map<Item, unsigned int> m_itemMakeCount;
};