#pragma once
#include <vector>
#include <memory>
#include <string>
#include <map>

class ItemStack;
enum class Item;

namespace save_data
{
	struct ItemStack;
}

class ItemSlot
{
public:
	ItemSlot(int itemCount);
	~ItemSlot() {};

	int GetSlotCount() const { return m_items.size(); }

	ItemStack* GetItem(int index) const;
	std::weak_ptr<ItemStack> GetItemReference(int index) const;

	ItemStack* GetItem(Item itemType) const;

	std::vector<ItemStack*> GetItems(Item itemType) const;

	std::shared_ptr<ItemStack> GetItemOwnership(int index);

	void RemoveItem(int index) 
	{ 
		if (index < 0 || index >= m_items.size()) return;

		m_items.at(index).reset(); 
	}

	void RemoveItem(ItemStack* item)
	{
		int index = -1;
		for (int i = 0; i < m_items.size(); i++)
		{
			const auto& raw = m_items.at(i).get();
			if (raw == item)
			{
				index = i;
				break;
			}
		}
		if (index == -1) return;

		RemoveItem(index);
	}

	/*アイテムを追加する関数群*/

	/// <summary>
	/// アイテムスタックのshared_ptrをcount分追加する。
	/// 成功時はnullptrを返す。
	/// 失敗時と溢れたときはそのアイテムのshared_ptrを返す。
	/// </summary>
	/// <param name="itemType"></param>
	/// <param name="count"></param>
	/// <returns></returns>
	std::shared_ptr<ItemStack> AddItem(std::shared_ptr<ItemStack> itemType, int count);

	/// <summary>
	/// アイテムスタックのshared_ptrを追加する。
	/// 成功時はnullptrを返す。
	/// 失敗時はそのアイテムのshared_ptrを返す。
	/// </summary>
	/// <param name="itemType"></param>
	/// <param name="count"></param>
	/// <returns></returns>
	std::shared_ptr<ItemStack> AddItem(std::shared_ptr<ItemStack> item);

	/// <summary>
	/// アイテムスタックを指定した場所に上書きする。
	/// </summary>
	/// <param name="index"></param>
	/// <param name="item"></param>
	void AddItem(int index, std::shared_ptr<ItemStack> item);

	/// <summary>
	/// 何も入っていないアイテムスタックのインデックスを返す。全部埋まっているなら-1を返す。
	/// </summary>
	/// <returns></returns>
	int FindEmptyItemSlot() const
	{
		for (int i = 0; i < m_items.size(); i++)
		{
			if (!m_items[i]) return i;
		}
		return -1;
	}

	/// <summary>
	///アイテムとその個数を取得
	/// </summary>
	/// <returns></returns>
	std::map<Item, int> SumItemCount() const;

	/// <summary>
	/// アイテムを指定個数で追加可能かを返す
	/// </summary>
	/// <param name="checkItem"></param>
	/// <param name="checkCount"></param>
	/// <returns></returns>
	bool CanAddItem(Item checkItem, int checkCount) const;

	std::vector<save_data::ItemStack> Save() const;

private:

	save_data::ItemStack SaveItemStack(int index) const;
private:
	std::vector<std::shared_ptr<ItemStack>> m_items;
};

