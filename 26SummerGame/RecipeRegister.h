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
			ItemTypeAndCount{{Item::kIronPlate,5}},
			ItemTypeAndCount{{Item::kMiningMachine,1}},
				2)
		},
		{
			RecipeName::kMakeBeltconveyor,
			std::make_shared<Recipe>(
			RecipeName::kMakeBeltconveyor,
			ItemTypeAndCount{{Item::kIronPlate,5},{Item::kCopperPlate,3}},
			ItemTypeAndCount{{Item::kBeltconveyor,3}},
				2)
		},
		{
			RecipeName::kMakeInserter,
			std::make_shared<Recipe>(
			RecipeName::kMakeInserter,
			ItemTypeAndCount{{Item::kIronPlate,3},{Item::kCopperPlate,5}},
			ItemTypeAndCount{{Item::kInserter,1}},
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
		RecipeName::kMakeMiningMachine,
		RecipeName::kMakeBeltconveyor,
		RecipeName::kMakeFurnace,
		RecipeName::kMakeInserter,
	};

	const std::unordered_set<RecipeName> kCraftMachineRecipe =
	{
		RecipeName::kMakeMiningMachine,
		RecipeName::kMakeBeltconveyor,
		RecipeName::kMakeFurnace,
		RecipeName::kMakeInserter,
	};

}