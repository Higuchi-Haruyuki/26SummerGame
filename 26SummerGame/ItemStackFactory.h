#pragma once
#include <memory>
#include "ItemType.h"
#include "ItemStack.h"
#include "Furnace.h"
#include "MiningMachine.h"
#include "BeltConveyor.h"
#include "Inserter.h"
#include "AssemblingMachine.h"
#include "Chest.h"

namespace ItemStackFactory
{
	inline std::unique_ptr<ItemStack> Make(Item itemType, int count)
	{
		switch (itemType)
		{
		case Item::kMiningMachine:
			return ItemStack::MakeItemStack<MiningMachine>("MiningMachine", itemType, count);
		case Item::kBeltconveyor:
			return ItemStack::MakeItemStack<BeltConveyor>("Beltconveyor", itemType, count);
		case Item::kInserter:
			return ItemStack::MakeItemStack<Inserter>("Inserter", itemType, count);
		case Item::kFurnace:
			return ItemStack::MakeItemStack<Furnace>("Furnace",itemType, count);
		case Item::kAssemblingMachine:
			return ItemStack::MakeItemStack<AssemblingMachine>("AssemblingMachine", itemType, count);
		case Item::kRockChest:
			return ItemStack::MakeItemStack<Chest>("Chest", itemType, count);
		default:
			return ItemStack::MakeItemStack(itemType, count);
		}
	}

}