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


ItemStack* ItemSlot::GetItem(Item itemType) const
{
	for (const auto& itemStack : m_items)
	{
		if (!itemStack) continue;
		if (itemStack->GetItemType() == itemType) return itemStack.get();
	}
	return nullptr;
}

std::unique_ptr<ItemStack> ItemSlot::GetItemOwnership(int index)
{
	if (!m_items.size()) return nullptr;

	return std::move(m_items.at(index));

}

std::unique_ptr<ItemStack> ItemSlot::AddItemStack(std::unique_ptr<ItemStack> item, int count)
{
	if (!item) return nullptr;
	const auto itemType = item->GetItemType();

	//そのアイテムを持っているなら
	if (const auto& itemStack = GetItem(itemType))
	{
		itemStack->MoveItemStack(item.get(), count);
		
		int currentItemCount = item->GetItemCount();
		if (currentItemCount) return AddItemStack(std::move(item), currentItemCount);
		return item;
	}
	//もっていないなら
	else
	{
		if (!item->GetItemCount()) return nullptr;
		return AddItemStack(std::move(item));
	}
}

std::unique_ptr<ItemStack> ItemSlot::AddItemStack(std::unique_ptr<ItemStack> item)
{

	const int index = FindEmptyItemSlot();

	//見つからなかったらfalseを返す。
	if (index == -1) return item;

	//見つかった場所にアイテムを追加
	m_items.at(index) = std::move(item);

	return nullptr;
}

void ItemSlot::AddItemStack(int index, std::unique_ptr<ItemStack> item)
{
	m_items.at(index) = std::move(item);
}

