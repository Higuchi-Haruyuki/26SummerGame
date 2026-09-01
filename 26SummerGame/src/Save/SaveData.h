#pragma once
#include "ItemType.h"
#include "VectorInt.h"
#include <vector>

namespace save_data
{
	struct ItemStack
	{
		int SlotIndex = 0;
		Item ItemType = Item::kNone;
		int Count = 0;
	};

	struct FactoryObject
	{
		Item ObjectType = Item::kNone;
		VectorInt GridPos = {};
		float RotationAngle = 0.0f;

		std::vector<ItemStack> InputItems = {};
		std::vector<ItemStack> OutputItems = {};
		std::vector<ItemStack> FuelItems = {};
	};

	struct SaveData
	{
		int Version = 1;
		std::vector<FactoryObject> FactoryObjects = {};
	};
}