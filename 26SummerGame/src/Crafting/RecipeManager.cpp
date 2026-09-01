#include "RecipeManager.h"
#include <memory>
#include <string>
#include "Recipe.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "RecipeRegister.h"


std::weak_ptr<Recipe> RecipeManager::GetRecipeFromName(RecipeName recipeName) const
{

	auto it = RecipeList::kRecipeList.find(recipeName);
	
	if(it == RecipeList::kRecipeList.end()) return std::weak_ptr<Recipe>();

	return (*it).second;

}


RecipeMap RecipeManager::GetSortedRecipeList(RecipeType recipeType)
{

	const RecipeNameSet* sortTable = nullptr;
	RecipeMap result;

	switch (recipeType)
	{
	case RecipeType::kNone:
		break;
	case RecipeType::kFurnace:
		sortTable = &RecipeList::kFurnaceRecipe;
		break;
	case RecipeType::kPlayerCraft:
		sortTable = &RecipeList::kPlayerCraftRecipe;
		break;
	case RecipeType::kAssemblingMachine:
		sortTable = &RecipeList::kCraftMachineRecipe;
		break;
	default:
		return result;
		break;
	}
	if (!sortTable) return result;

	result.reserve(sortTable->size());

	for (const auto& name : *sortTable) {
		const auto it = RecipeList::kRecipeList.find(name);

		if (it == RecipeList::kRecipeList.end()) {
			continue;
		}
		result.emplace(it->first, it->second);
	}
	return result;
}

bool RecipeManager::IsContainsFurnaceRecipe(RecipeName recipeName) const
{
	return RecipeList::kFurnaceRecipe.contains(recipeName);
}

bool RecipeManager::IsContainsCraftMachineRecipe(RecipeName recipeName) const
{
	return RecipeList::kCraftMachineRecipe.contains(recipeName);
}

bool RecipeManager::IsContainsPlayerCraftRecipe(RecipeName recipeName) const
{
	return RecipeList::kPlayerCraftRecipe.contains(recipeName);
}

std::weak_ptr<Recipe> RecipeManager::GetRecipeFromItemType(RecipeType recipeType, Item itemType) const
{
	RecipeName recipeName = GetRecipeNameFromItemType(itemType);

	switch (recipeType)
	{
	case RecipeType::kNone:
		break;
	case RecipeType::kFurnace:
		if (!IsContainsFurnaceRecipe(recipeName)) return std::weak_ptr<Recipe>();
			break;
	case RecipeType::kPlayerCraft:
		if (!IsContainsPlayerCraftRecipe(recipeName)) return std::weak_ptr<Recipe>();
		break;
	case RecipeType::kAssemblingMachine:
		if (!IsContainsCraftMachineRecipe(recipeName)) return std::weak_ptr<Recipe>();
		break;
	default:
		assert(false && "到達してはいけないswitch分岐です。");
		break;
	}

	return GetRecipeFromName(recipeName);
}

RecipeName RecipeManager::GetRecipeNameFromItemType(Item itemType) const
{
	for (const auto& [name, recipe] : RecipeList::kRecipeList)
	{
		for (const auto& input : recipe->GetRecipeInput())
		{
			if (input.first == itemType) return name;
		}
	}
	return RecipeName::kNone;
}

