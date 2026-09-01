#pragma once
#include "RecipeManager.h"
#include <map>
#include <string>
#include <memory>
#include "Recipe.h"
#include <vector>
#include <unordered_map>
#include "ItemType.h"

#include <unordered_set>

namespace RecipeList
{

	const std::unordered_map<RecipeName, std::shared_ptr<Recipe>> kRecipeList =
	{
		{
			RecipeName::kMakeIronPlate,
			std::make_shared<Recipe>(
			RecipeName::kMakeIronPlate,
			ItemTypeAndCount{{Item::kIronOre,1}},
			ItemTypeAndCount{{Item::kIronPlate,1}},
				2)
		},
		{
			RecipeName::kMakeCopperPlate,
			std::make_shared<Recipe>(
			RecipeName::kMakeCopperPlate,
			ItemTypeAndCount{{Item::kCopperOre,1}},
			ItemTypeAndCount{{Item::kCopperPlate,1}},
				2)
		},
		{
			RecipeName::kMakeCopperCable,
			std::make_shared<Recipe>(
			RecipeName::kMakeCopperCable,
			ItemTypeAndCount{{Item::kCopperPlate,1}},
			ItemTypeAndCount{{Item::kCopperCable,2}},
				2)
		},
		{
			RecipeName::kMakeElectronicCircuit,
			std::make_shared<Recipe>(
			RecipeName::kMakeElectronicCircuit,
			ItemTypeAndCount{{Item::kCopperCable,3},{Item::kIronPlate,1}},
			ItemTypeAndCount{{Item::kElectronicCircuit,1}},
				2)
		},
		{
			RecipeName::kMakeFurnace,
			std::make_shared<Recipe>(
			RecipeName::kMakeFurnace,
			ItemTypeAndCount{{Item::kRockOre,5}},
			ItemTypeAndCount{{Item::kFurnace,1}},
				2)
		},
		{
			RecipeName::kMakeMiningMachine,
			std::make_shared<Recipe>(
			RecipeName::kMakeMiningMachine,
			ItemTypeAndCount{{Item::kIronPlate,3},{Item::kFurnace,1}},
			ItemTypeAndCount{{Item::kMiningMachine,1}},
				2)
		},
		{
			RecipeName::kMakeBeltconveyor,
			std::make_shared<Recipe>(
			RecipeName::kMakeBeltconveyor,
			ItemTypeAndCount{{Item::kIronPlate,2},{Item::kCopperCable,2}},
			ItemTypeAndCount{{Item::kBeltconveyor,3}},
				2)
		},
		{
			RecipeName::kMakeInserter,
			std::make_shared<Recipe>(
			RecipeName::kMakeInserter,
			ItemTypeAndCount{{Item::kElectronicCircuit,1},{Item::kIronPlate,2}},
			ItemTypeAndCount{{Item::kInserter,1}},
				2)
		},
		{
			RecipeName::kMakeAssemblingMachine,
			std::make_shared<Recipe>(
			RecipeName::kMakeAssemblingMachine,
			ItemTypeAndCount{{Item::kElectronicCircuit,3},{Item::kIronPlate,20}},
			ItemTypeAndCount{{Item::kAssemblingMachine,1}},
				2)
		},
		{
			RecipeName::kMakeRockChest,
			std::make_shared<Recipe>(
			RecipeName::kMakeRockChest,
			ItemTypeAndCount{{Item::kIronPlate,1},{Item::kRockOre,6}},
			ItemTypeAndCount{{Item::kRockChest,1}},
				2)
		},
	};

	const std::unordered_set<RecipeName> kFurnaceRecipe =
	{
		RecipeName::kMakeIronPlate,
		RecipeName::kMakeCopperPlate,
	};

	const std::unordered_set<RecipeName> kPlayerCraftRecipe =
	{
		RecipeName::kMakeCopperCable,
		RecipeName::kMakeElectronicCircuit,
		RecipeName::kMakeMiningMachine,
		RecipeName::kMakeBeltconveyor,
		RecipeName::kMakeFurnace,
		RecipeName::kMakeInserter,
		RecipeName::kMakeAssemblingMachine,
		RecipeName::kMakeRockChest
	};

	const std::unordered_set<RecipeName> kCraftMachineRecipe =
	{
		RecipeName::kMakeCopperCable,
		RecipeName::kMakeElectronicCircuit,
		RecipeName::kMakeMiningMachine,
		RecipeName::kMakeBeltconveyor,
		RecipeName::kMakeFurnace,
		RecipeName::kMakeInserter,
		RecipeName::kMakeAssemblingMachine,
		RecipeName::kMakeRockChest

	};

}