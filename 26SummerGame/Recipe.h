#pragma once
#include <string>
#include <map>
#include <vector>

enum class Item;

enum class RecipeName
{
	kNone = 0,
	kMakeIronPlate,
	kMakeCopperPlate,
	kMakeBeltconveyor,
	kMakeInserter,
	kMakeMiningMachine,
	kMakeFurnace,
};

enum class RecipeType
{
	kNone = 0,
	kFurnace,
	kPlayerCraft,
	kCraftMachine

};

using ItemTypeAndCount = std::vector<std::pair<Item, int>>;

class Recipe
{


public :
	Recipe(RecipeName recipeName, const ItemTypeAndCount& input,
		const ItemTypeAndCount& output,
		int time):
		m_recipeName(recipeName),
		m_inputItemTypeAndCount(input),
		m_outputItemTypeAndCount(output),
		m_processTime(time)
	{}
	Recipe():
	m_processTime(-1)
	{}

	~Recipe() {}

	//GETTER
	RecipeName GetRecipeName() const {
		return m_recipeName;
	}

	ItemTypeAndCount GetRecipeInput() const
	{
		return m_inputItemTypeAndCount;
	}

	ItemTypeAndCount GetRecipeOutput() const
	{
		return m_outputItemTypeAndCount;
	}

	int GetRecipeProcessTime() const { return m_processTime; }

	//SETTER

	void SetRecipeName(RecipeName recipeName)
	{
		m_recipeName = recipeName;
	}

	void SetRecipeInput(const ItemTypeAndCount& m)
	{
		m_inputItemTypeAndCount = m;
	}

	void SetRecipeOutput(const ItemTypeAndCount& m)
	{
		m_outputItemTypeAndCount = m;
	}

	void SetRecipeProcessTime(int time) { m_processTime = time; }

private:

	//レシピ名
	RecipeName m_recipeName = RecipeName::kNone;

	//使用するアイテムタイプと個数
	ItemTypeAndCount m_inputItemTypeAndCount;

	//生成するアイテムタイプと個数
	ItemTypeAndCount m_outputItemTypeAndCount;

	//生成にかかる時間

	int m_processTime;
};

