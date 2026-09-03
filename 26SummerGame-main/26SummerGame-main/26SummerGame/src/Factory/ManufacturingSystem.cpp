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
#include "Color.h"
#include "ItemStackFactory.h"

namespace
{
	/*UI*/
	//windowの定数
	const Vector kWindowPos = { Game::kDisplaySize.m_x * 0.87f , Game::kDisplaySize.m_y * 0.5f };
	const Vector kWindowSize = { 400,800,0 };
	constexpr unsigned int kWindowColor = static_cast<unsigned int>(Color::kMainColor);
	constexpr int kWindowAlpha = 200;

	//1レシピあたりの四角
	const Vector kRecipeSize = { 380,70,0 };
	constexpr unsigned int kRecipeColor = static_cast<unsigned int>(Color::kDarkSubColor);
	constexpr unsigned int kRecipeAccentColor = static_cast<unsigned int>(Color::kMainAccentColor);
	constexpr int kRecipeAlpha = 255;

	//レシピ間のオフセット
	const Vector kRecipeOffset = { 0,10 };

	//アイテムボックスの定数
	const Vector kItemBoxSize = { 50,50,0 };
	constexpr int kItemBoxAlpha = 255;

	//アイテムボックス間のオフセット
	const Vector kItemBoxOffset = { 100,0 };

	//完成品のアイテムボックス
	const Vector kCompletionItemBoxSize = { 80,80 };
}

ManufacturingSystem::ManufacturingSystem(std::weak_ptr<Object> parentObject):
	Component(parentObject),
	m_recipeManager(RecipeManager::GetInstance()),
	m_factoryManager(FactoryManager::GetInstance())
{}

void ManufacturingSystem::Init()
{
	Component::Init();
	m_recipeList = m_recipeManager.GetSortedRecipeList(m_allowRecipeType);
}

void ManufacturingSystem::Update()
{
	Component::Update();
	if (!m_isEnable) return;
	if(m_recipeList.empty())
		m_recipeList = m_recipeManager.GetSortedRecipeList(m_allowRecipeType);
	UpdateUIPanel();
}

void ManufacturingSystem::Finalize()
{
	Component::Finalize();
}

std::shared_ptr<UIPanel> ManufacturingSystem::GetOrBuidUIPanel()
{
	if (!m_uiPanel)
	{
		m_uiPanel = std::make_shared<UIPanel>();
		BuildUIPanel();
	}
	return m_uiPanel;
}

void ManufacturingSystem::SetRecipe(const ItemSlot* items)
{
	if (!items->GetSlotCount()) return;

	const auto& item = items->GetItem(0);

	if (!item) return;

	//アイテムスタックからアイテムタイプを取得する。
	auto currentItemType = item->GetItemType();

	//取得したアイテムタイプから作成可能なレシピを取得する。
	m_currentRecipe = m_recipeManager.GetRecipeFromItemType(m_allowRecipeType, currentItemType);
}

bool ManufacturingSystem::HasRequiedItem(std::shared_ptr<Recipe> recipe, const ItemSlot* itemSlot) const
{
	if (!recipe) return false;
	
	const auto sumItemCount = itemSlot->SumItemCount();

	//そのレシピの必要素材と数を取得
	const auto& input = recipe->GetRecipeInput();

	for (const auto& [item, count] : input)
	{
		//所有アイテムから必要アイテムを探す
		auto it = sumItemCount.find(item);
		//見つからないとき
		if (it == sumItemCount.end()) return false;

		//所有個数が必要数より小さい時
		if (it->second < count) return false;
	}

	return true;
}

bool ManufacturingSystem::CanStoreOutput(std::shared_ptr<Recipe> recipe, const ItemSlot* outputSlot) const
{
	if (!recipe) return false;

	const auto& recipeOutput = recipe->GetRecipeOutput();

	const auto sumItemCount = outputSlot->SumItemCount();

	for (const auto& [recipeItem, recipeCount] : recipeOutput)
	{
		if (!outputSlot->CanAddItem(recipeItem, recipeCount)) return false;
	}

	return true;
}

void ManufacturingSystem::ConsumeRequiredItems(std::shared_ptr<Recipe> recipe, ItemSlot* itemSlot)
{
	for (const auto& [needItem, needCount] : recipe->GetRecipeInput())
	{
		int remainCount = needCount;
			if (remainCount <= 0) break;


			for (int i = 0; i < itemSlot->GetSlotCount(); i++)
			{
				if (remainCount <= 0) break;

				auto* itemStack = itemSlot->GetItem(i);
				if (!itemStack) continue;
				if (itemStack->GetItemType() != needItem) continue;

				remainCount -= itemStack->MinusItemCount(remainCount);

				if (itemStack->GetItemCount() <= 0) itemSlot->RemoveItem(i);
			}
		
	}

}

void ManufacturingSystem::StoreOutput(std::shared_ptr<Recipe> recipe, ItemSlot* outputSlot)
{
	for (const auto& [outItem, outCount] : recipe->GetRecipeOutput())
	{
		auto item = ItemStackFactory::Make(outItem, outCount);

		if (outputSlot->CanAddItem(outItem, outCount))
		{
			outputSlot->AddItem(std::move(item), outCount);
			m_factoryManager.OnMakeItem(outItem, outCount);
		}

	}

}

void ManufacturingSystem::BuildUIPanel()
{
	//Windowの生成
	UIFactory::MakeUIToPanel<UISquare>(m_uiPanel, kWindowPos, kWindowSize, kWindowColor, kWindowAlpha);

	const auto closeSize = Vector{ 30,30 };
	const auto closeOffset = Vector{ -10,10 };
	const auto closePos = kWindowPos + Vector{ kWindowSize.m_x * 0.5f , -kWindowSize.m_y * 0.5f } + Vector{ -closeSize.m_x * 0.5f, closeSize.m_y * 0.5f} + closeOffset;

	const auto& close = UIFactory::MakeUIToPanel<UIImage>(m_uiPanel, closePos, closeSize, GraphicId::kUIClose);
	close.lock()->SubscribeOnClick([this]() {m_uiPanel->SetVisible(false); });

	auto index = 0;

	const auto startOffset = Vector{ 10,100 };

	const auto firstpos = kWindowPos - kWindowSize * 0.5f + kRecipeSize * 0.5f + kRecipeOffset + startOffset;

	if (m_recipeList.empty())
		m_recipeList = m_recipeManager.GetSortedRecipeList(m_allowRecipeType);

	for (const auto& recipe : m_recipeList)
	{
		const auto pos = firstpos + (Vector{ 0, kRecipeSize.m_y } + kRecipeOffset) * index;
		const auto leftUpPos = pos - kRecipeSize * 0.5f;

		const auto& recipeSquare = UIFactory::MakeUIToPanel<UISquare>(
			m_uiPanel, pos, kRecipeSize, kRecipeColor, kRecipeAlpha
		);
		recipeSquare.lock()->SubscribeOnClick(
			[this,&recipe]() 
			{
				m_currentRecipe = recipe.second;
			});

		m_recipeSquares.emplace(recipe.first, recipeSquare);

		BuildRecipeUI(leftUpPos, recipe.second);
		index++;
	}

}

void ManufacturingSystem::UpdateUIPanel()
{
	const auto& safeRecipe = m_currentRecipe.lock();
	if (!safeRecipe) return;

	for (const auto& [recipeName, square] : m_recipeSquares)
	{
		const auto& safeSquare = square.lock();
		if (!safeSquare) continue;

		if (recipeName == safeRecipe->GetRecipeName())
		{
			safeSquare->SetColor(kRecipeAccentColor);
		}
		else
		{
			safeSquare->SetColor(kRecipeColor);
		}
	}

}

void ManufacturingSystem::BuildRecipeUI(Vector leftUpDrawPos, std::weak_ptr<Recipe> recipe)
{
	const auto& safeRecipe = recipe.lock();
	const auto& outputs = safeRecipe->GetRecipeOutput();
	const auto& inputs = safeRecipe->GetRecipeInput();

	auto startOffset = Vector{10,(kRecipeSize.m_y - kItemBoxSize.m_y) * 0.5f };

	auto startPos = leftUpDrawPos + kItemBoxSize * 0.5f + startOffset;

	auto pos = Vector{};

	for (int i = 0; i < outputs.size(); i++)
	{
		pos = startPos + (Vector{ kItemBoxSize.m_x ,0 } + kItemBoxOffset) * i;

		const auto& itemBox = std::make_shared<UIItemBox>(
		m_uiPanel, pos,kItemBoxSize);
		auto graphicID = ItemTable::GetGraphicID(outputs.at(i).first);

		itemBox->SetGraphicID(graphicID);
		itemBox->SetText(StringUtil::IntToString(outputs.at(i).second));

		if ((i + 1) < outputs.size())
		{
			auto plusPos = pos + Vector{ kItemBoxOffset * 0.75f };
			UIFactory::MakeUIToPanel<UIImage>(
				m_uiPanel, plusPos, kItemBoxSize * 0.9f, GraphicId::kPlus, kRecipeAlpha
			);
		}
	}

	auto arrowPos = pos + Vector{ kItemBoxOffset * 0.75f };

	UIFactory::MakeUIToPanel<UIImage>(
		m_uiPanel, arrowPos, kItemBoxSize * 1.5f, GraphicId::kArrow,kRecipeAlpha
	);

	for (int i = 0; i < inputs.size(); i++)
	{
		pos = startPos + ( Vector{ kItemBoxSize.m_x ,0 } + kItemBoxOffset) * (i + outputs.size());

		const auto& itemBox = std::make_shared<UIItemBox>(
			m_uiPanel, pos, kItemBoxSize);
		auto graphicID = ItemTable::GetGraphicID(inputs.at(i).first);

		itemBox->SetGraphicID(graphicID);
		itemBox->SetText(StringUtil::IntToString(inputs.at(i).second));

		if ((i + 1) < inputs.size())
		{
			auto plusPos = pos + Vector{ kItemBoxOffset * 0.75f };
			UIFactory::MakeUIToPanel<UIImage>(
				m_uiPanel, plusPos, kItemBoxSize * 0.9f, GraphicId::kPlus, kRecipeAlpha
			);
		}
	}
}

bool ManufacturingSystem::Manufacture(ItemSlot* inputItemSlot, ItemSlot* outputItemSlot)
{
	const auto& safeRecipe = m_currentRecipe.lock();
	if (!safeRecipe) return false;

	if (!HasRequiedItem(safeRecipe, inputItemSlot)) return false;

	if (!CanStoreOutput(safeRecipe, outputItemSlot)) return false;

	ConsumeRequiredItems(safeRecipe, inputItemSlot);
	StoreOutput(safeRecipe, outputItemSlot);

	return true;
}