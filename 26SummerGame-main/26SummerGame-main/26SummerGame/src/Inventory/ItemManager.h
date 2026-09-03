#pragma once
#include <string>
#include <array>
#include "ItemType.h"
#include <cassert>
#include "ResourceType.h"
#include <unordered_map>

enum class ItemType
{
	kNone = 0,
	kOre,
	kNormal,
};

struct ItemInfo
{
	Item Item;
	ItemType Type;
	const char* ItemName;
	const char* ItemDisplayName;
	GraphicId Icon;
	ModelId Model;
};



namespace ItemTable
{
	constexpr std::array<ItemInfo, static_cast<size_t>(Item::kItemTypeCount)> kItemTypeTable = {{
		{Item::kNone,ItemType::kNone,"none","なし"},
		{Item::kIronOre,ItemType::kOre,"iron_ore","鉄",GraphicId::kIronOreIcon,ModelId::kIronOre},
		{Item::kCoalOre,ItemType::kOre,"coal_ore","石炭",GraphicId::kCoalOreIcon,ModelId::kIronOre},
		{Item::kRockOre,ItemType::kOre,"rock_ore","石",GraphicId::kRockIcon,ModelId::kIronOre},
		{Item::kCopperOre,ItemType::kOre,"copper_ore","銅",GraphicId::kCopperOreIcon,ModelId::kIronOre},
		{Item::kIronPlate,ItemType::kNormal,"iron_plate","鉄板",GraphicId::kIronPlateIcon,ModelId::kIronPlate},
		{Item::kCopperPlate,ItemType::kNormal,"copper_plate","銅板",GraphicId::kCopperPlateIcon,ModelId::kCopperPlate},
		//新規分
		{Item::kCopperCable,ItemType::kNormal,"copper_cable","銅線",GraphicId::kCopperCableIcon,ModelId::kCopperPlate},
		{Item::kElectronicCircuit,ItemType::kNormal,"electonic_circuit","電子基盤",GraphicId::kElectonicCircuit,ModelId::kCopperPlate},
		
		{Item::kMiningMachine,ItemType::kNormal,"miningMachine","採掘機",GraphicId::kMiningMachineIcon,ModelId::kNone},
		{Item::kBeltconveyor,ItemType::kNormal,"beltconveyor","ベルトコンベア",GraphicId::kBeltconveyorIcon,ModelId::kBeltConveyor},
		{Item::kInserter,ItemType::kNormal,"inserter","インサータ",GraphicId::kInserterIcon,ModelId::kNone},
		{Item::kFurnace,ItemType::kNormal,"furnace","溶鉱炉",GraphicId::kFurnaceIcon,ModelId::kNone},
		{Item::kRockChest,ItemType::kNormal,"rock_chest","鉄製チェスト",GraphicId::kRockChestIcon,ModelId::kNone},
		{Item::kAssemblingMachine,ItemType::kNormal,"assemblingMachine","組立機",GraphicId::kAssemblingMachineIcon,ModelId::kNone},
		}};

	static_assert(kItemTypeTable.size() == static_cast<size_t>(Item::kItemTypeCount),
		"kItemTypeTable の大きさが Item::kItemTypeCount と異なります。");


	inline std::array<ItemInfo, static_cast<size_t>(Item::kItemTypeCount)>::const_iterator FindItemType(Item itemType)
	{
		const auto it = std::find_if(ItemTable::kItemTypeTable.begin(), ItemTable::kItemTypeTable.end(),
			[itemType](ItemInfo itemtypeInfo)
			{
				return itemtypeInfo.Item == itemType;
			}
		);
		return it;
	}

	inline std::string ItemTypeToDisplayName(Item itemType)
	{
		return FindItemType(itemType)->ItemDisplayName;
	}

	inline std::string ItemTypeToItemName(Item itemType)
	{
		return FindItemType(itemType)->ItemName;
	}

	inline GraphicId GetGraphicID(Item itemType)
	{
		return FindItemType(itemType)->Icon;
	}

}

class ItemManager
{
public:
	static ItemManager& GetInstance()
	{
		static bool isFirst = true;
		if (isFirst) 
		{
			ValidateItemTypeTable();
			isFirst = false;
		} 

		static ItemManager instance;
		return instance;
	}


private:
	ItemManager() = default;
	~ItemManager() = default;

	ItemManager(const ItemManager&) = delete;
	ItemManager& operator=(const ItemManager&) = delete;
	ItemManager(ItemManager&&) = delete;
	ItemManager& operator=(ItemManager&&) = delete;


private:
	
	// テーブルのi番目が本当にenum値iと一致しているかを起動時に1回だけ確認する
	static void ValidateItemTypeTable()
	{
		for (size_t i = 0; i < ItemTable::kItemTypeTable.size(); ++i) {
			assert(ItemTable::kItemTypeTable[i].Item == static_cast<Item>(i) &&
				"kItemTypeTable の並びが Item enum と異なります。");
		}
	}
};
