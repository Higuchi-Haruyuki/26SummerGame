#include "ItemSlot.h"
#include "ItemStack.h"

ItemSlot::ItemSlot(int itemCount)
{
	m_items.resize(itemCount);
}

ItemStack* ItemSlot::GetItem(int index) const
{
	if (!m_items.size()) return nullptr;

	return m_items.at(index).get();
}

std::weak_ptr<ItemStack> ItemSlot::GetItemReference(int index) const
{
	if (index < 0 || index >= static_cast<int>(m_items.size())) return {};
	return m_items.at(index);
}


ItemStack* ItemSlot::GetItem(Item itemType) const
{
	for (const auto& itemStack : m_items)
	{
		if (!itemStack) continue;
		if (itemStack->GetItemType() == itemType) return itemStack.get();
	}
	return nullptr;
}

std::vector<ItemStack*> ItemSlot::GetItems(Item itemType) const
{
	std::vector<ItemStack*> result;

	for (const auto& itemStack : m_items)
	{
		if (!itemStack) continue;
		if (itemStack->GetItemType() == itemType) result.push_back(itemStack.get());
	}

	return result;
}

std::shared_ptr<ItemStack> ItemSlot::GetItemOwnership(int index)
{
	if (!m_items.size()) return nullptr;

	return std::move(m_items.at(index));

}

std::shared_ptr<ItemStack> ItemSlot::AddItem(std::shared_ptr<ItemStack> item, int count)
{
	if (!item) return nullptr;
	if (count <= 0) return item;

	const auto itemType = item->GetItemType();

	int remainMoveCount = count;

	//同じ種類のスタックに入るだけ入れていく
	for (const auto& itemStack : m_items)
	{
		if (remainMoveCount <= 0) break;
		if (item->GetItemCount() <= 0) break;

		if (!itemStack) continue;
		if (itemStack->GetItemType() != itemType) continue;

		const int beforeCount = item->GetItemCount();
		itemStack->MoveItemStack(item.get(), remainMoveCount);

		//満杯のスタックは0個しか移動しないので、そのまま次のスタックへ進む
		remainMoveCount -= (beforeCount - item->GetItemCount());
	}

	//すべて移動できた
	if (item->GetItemCount() <= 0) return nullptr;

	//余った分は空きスロットへ
	return AddItem(std::move(item));
}

std::shared_ptr<ItemStack> ItemSlot::AddItem(std::shared_ptr<ItemStack> item)
{

	const int index = FindEmptyItemSlot();

	//見つからなかったらfalseを返す。
	if (index == -1) return item;

	//見つかった場所にアイテムを追加
	m_items.at(index) = std::move(item);

	return nullptr;
}

void ItemSlot::AddItem(int index, std::shared_ptr<ItemStack> item)
{
	m_items.at(index) = std::move(item);
}

bool ItemSlot::CanAddItem(Item checkItem, int checkCount) const
{
	for (int i = 0; i < GetSlotCount(); i++)
	{
		const auto* item = GetItem(i);
		if (!item) return true;

		if (checkItem == item->GetItemType())
		{
			auto successCount = item->CheckAddItemCount(checkCount);

			if (successCount < checkCount) return false;

			return true;
		}

	}
	return false;
}

std::map<Item, int> ItemSlot::SumItemCount() const
{
	std::map<Item, int> result;

	for (const auto& item : m_items)
	{
		if (!item) continue;

		//すでにそのアイテムがキーとしてあるか
		auto it = result.find(item->GetItemType());

		//ないなら新規追加
		if (it == result.end())
		{
			result.emplace(item->GetItemType(), item->GetItemCount());
			continue;
		}

		//あるときは個数追加
		(*it).second += item->GetItemCount();
	}

	return result;
}

