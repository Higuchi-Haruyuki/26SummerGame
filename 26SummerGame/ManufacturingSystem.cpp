#include "ManufacturingSystem.h"
#include "Recipe.h"
#include "RecipeManager.h"
#include "FactoryManager.h"
#include "ItemStack.h"
#include "ItemSlot.h"
#include "Debug.h"
#include "ItemType.h"
#include "UIPanel.h"
#include "UIItemBox.h"
#include "UISquare.h"
#include "UIImage.h"
#include "UIFactory.h"
#include "ItemManager.h"
#include "StringUtil.h"

namespace
{
	/*UI*/
	//windowの定数
	const Vector kWindowPos = Game::kDisplaySize * 0.5f;
	const Vector kWindowSize = { 400,800,0 };
	constexpr unsigned int kWindowColor = 0xd3d3d3;
	constexpr int kWindowAlpha = 200;

	//1レシピあたりの四角
	const Vector kRecipePos = Game::kDisplaySize * 0.5f;
	const Vector kRecipeSize = { 100,200,0 };
	constexpr unsigned int kRecipeColor = 0xffffff;
	constexpr int kRecipeAlpha = 255;

	//アイテムボックスの定数
	const Vector kItemBoxPos = Game::kDisplaySize * 0.5f;
	const Vector kItemBoxSize = { 50,50,0 };
	constexpr int kItemBoxAlpha = 255;
}

ManufacturingSystem::ManufacturingSystem(std::shared_ptr<Object> parentObject):
	Component(parentObject),
	m_recipeManager(RecipeManager::GetInstance()),
	m_factoryManager(FactoryManager::GetInstance())
{}

void ManufacturingSystem::Init()
{
	Component::Init();
	m_recipeList = m_recipeManager.GetSortedRecipeList(m_allowRecipeType);
}

void ManufacturingSystem::Update(float deltaTime)
{
	Component::Update(deltaTime);
	if (!m_isEnable) return;
	if(m_recipeList.empty())
		m_recipeList = m_recipeManager.GetSortedRecipeList(m_allowRecipeType);
}

void ManufacturingSystem::Finalize()
{
	Component::Finalize();
}

std::shared_ptr<UIPanel> ManufacturingSystem::GetOrBuidUIPanel()
{
	if (!m_uiPanel)
		BuildUIPanel();
	return m_uiPanel;
}

void ManufacturingSystem::SetRecipe(const ItemSlot* items)
{
	if (!items->GetItemCount()) return;

	const auto& item = items->GetItem(0);

	if (!item) return;

	//アイテムスタックからアイテムタイプを取得する。
	auto currentItemType = item->GetItemType();

	//取得したアイテムタイプから作成可能なレシピを取得する。
	m_currentRecipe = m_recipeManager.GetRecipeFromItemType(m_allowRecipeType, currentItemType);
}

int ManufacturingSystem::CalcNeedItemCount(const ItemSlot* items) const
{
	if (!items->GetItemCount()) return 0;

	if (!items->GetItem(0)) return 0;

	const auto& recipe = m_currentRecipe.lock();
	if (!recipe) return 0;

	//そのレシピの必要素材と数を取得
	const auto& input = recipe->GetRecipeInput();

	for (const auto& inputItem : input)
	{
		//アイテムタイプが違うときは不可
		if (inputItem.first != items->GetItem(0)->GetItemType()) continue;
		//アイテム数が足りないときは不可
		if (inputItem.second <= items->GetItem(0)->GetItemCount()) return inputItem.second;
	}

	return 0;
}

void ManufacturingSystem::BuildUIPanel()
{
	//Windowの生成
	UIFactory::MakeUIToPanel<UISquare>(m_uiPanel, kWindowPos, kWindowSize, kWindowColor, kWindowAlpha);

	int index = 0;

	for (const auto& recipe : m_recipeList)
	{
		const auto& recipeSquare = UIFactory::MakeUIToPanel<UISquare>(
			m_uiPanel, kWindowPos, kWindowSize, kWindowColor, kWindowAlpha
		);
		recipeSquare.lock()->SetOnClick(
			[this,recipe]() 
			{
				m_currentRecipe = recipe.second;
			});

		BuildRecipeUI(index,recipe.second);
		index++;
	}

}

void ManufacturingSystem::BuildRecipeUI(int index, std::weak_ptr<Recipe> recipe)
{
	const auto& safeRecipe = recipe.lock();
	const auto& outputs = safeRecipe->GetRecipeOutput();
	for (int i = 0; i < outputs.size(); i++)
	{
		Vector pos = { 0,0,0 };

		const auto& itemBox = std::make_shared<UIItemBox>(
		m_uiPanel, pos,kItemBoxSize);
		auto graphicID = ItemTable::GetGraphicID(outputs.at(i).first);

		itemBox->SetGraphicID(graphicID);
		itemBox->SetText(StringUtil::IntToString(outputs.at(i).second));
	}
}

bool ManufacturingSystem::Manufacture(ItemSlot* inputItems, ItemSlot* outputItemSlot)
{
	const auto& safeRecipe = m_currentRecipe.lock();
	if (!safeRecipe) return false;

	int needItemCount = CalcNeedItemCount(inputItems);
	if (!needItemCount) return false;

	//出力用スロットを取得
	const auto& outputItem = outputItemSlot->GetItem(0);

	//入力用スロットを取得
	const auto& inputItem = inputItems->GetItem(0);

	//レシピの出力する素材と個数を取得
	const auto& output = safeRecipe->GetRecipeOutput();

	//出力可能かのフラグ
	bool isOk = false;

	//レシピが出力する個数の取得
	int recipeOutputCount = 0;

	//何かアイテムが入っている
	if (outputItem)
	{
		for (const auto& recipeOutputItem : output)
		{
			if (recipeOutputItem.first != outputItem->GetItemType()) continue;
			recipeOutputCount = recipeOutputItem.second;
			//出力用スロットにちゃんとレシピでついかされるぶんが追加できるかを確認する。
			if (recipeOutputCount != outputItem->CheckAddItemCount(recipeOutputCount)) continue;
			isOk = true;
		}
		if (!isOk) return false;

		inputItem->MinusItemCount(needItemCount);
		outputItem->AddItemCount(recipeOutputCount);
	}
	//出力用スロットが空の時
	else
	{
		Item recipeOutputType;
		for (const auto& outputItem : output)
		{
			recipeOutputType = outputItem.first;
			recipeOutputCount = outputItem.second;
		}

		inputItem->MinusItemCount(needItemCount);
		outputItemSlot->AddItemStack(0, std::make_unique<ItemStack>(recipeOutputType, recipeOutputCount));
	}

	for (const auto& outputItem : output)
	{
		m_factoryManager.OnMakeItem(outputItem.first, outputItem.second);
	}

	if(!inputItem->GetItemCount()) inputItems->RemoveItem(0);

	return true;
}