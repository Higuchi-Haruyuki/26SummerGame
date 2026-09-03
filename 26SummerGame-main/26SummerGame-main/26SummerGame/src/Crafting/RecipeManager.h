#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

class Recipe;

enum class Item;
enum class RecipeName;
enum class RecipeType;

using RecipeMap = std::unordered_map<RecipeName, std::shared_ptr<Recipe>>;
using RecipeNameSet = std::unordered_set<RecipeName>;

//レシピを保存しておくシングルトンなクラス
class RecipeManager
{
public:
	static RecipeManager& GetInstance()
	{
		static RecipeManager instance;
		return instance;
	}

	std::weak_ptr<Recipe> GetRecipeFromName(RecipeName recipeName) const;

	RecipeMap GetSortedRecipeList(RecipeType recipeType);

	bool IsContainsFurnaceRecipe(RecipeName recipeName) const;

	bool IsContainsCraftMachineRecipe(RecipeName recipeName) const;

	bool IsContainsPlayerCraftRecipe(RecipeName recipeName) const;


	std::weak_ptr<Recipe> GetRecipeFromItemType(RecipeType recipeType, Item itemType) const;

private:

	RecipeManager() = default;
	~RecipeManager() = default;

	RecipeManager(const RecipeManager&) = delete;
	RecipeManager& operator=(const RecipeManager&) = delete;

	RecipeManager(const RecipeManager&&) = delete;
	RecipeManager& operator=(const RecipeManager&&) = delete;

private:
	RecipeName GetRecipeNameFromItemType(Item itemType) const;

};

