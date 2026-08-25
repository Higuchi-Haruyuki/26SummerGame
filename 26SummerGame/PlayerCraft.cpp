#include "PlayerCraft.h"
#include "Object.h"
#include "RecipeManager.h"
#include "UIFactory.h"
#include "UIPanel.h"
#include "UIImage.h"
#include "UIItemBox.h"
#include "UISquare.h"
#include "Game.h"
#include "ResourceType.h"
#include "ItemManager.h"
#include "ItemSlot.h"
#include "UIManager.h"
#include "StringUtil.h"
#include "Timer.h"
#include <numeric>

namespace
{
	/*UI*/
	//windowの定数
	const Vector kWindowPos = { Game::kDisplaySize.m_x * 0.75f , Game::kDisplaySize.m_y * 0.4f };
	const Vector kWindowSize = { 600,800,0 };
	constexpr unsigned int kWindowColor = 0xd3d3d3;
	constexpr int kWindowAlpha = 200;

	//1レシピあたりの四角
	const Vector kRecipeSize = { 580,100,0 };
	constexpr unsigned int kRecipeColor = 0xffffff;
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

PlayerCraft::PlayerCraft(std::shared_ptr<Object> parent) :
	Component(parent),
	m_recipeManager(RecipeManager::GetInstance()),
	m_uiManager(UIManager::GetInstance())
{
}

PlayerCraft::~PlayerCraft()
{
}

void PlayerCraft::Init()
{
	Component::Init();
	if (!m_isEnable) return;
	m_recipeList = m_recipeManager.GetSortedRecipeList(m_allowRecipeType);
	m_completionItemSlot = std::make_shared<ItemSlot>(1);

	m_craftTimer.SubscribeOnFinished([this]()
		{
			if (HasRequiredItems(m_craftConsumeSlots)) 
			{
				Craft();
			}
			m_craftTimer.ResetStartTime();
		});

}

void PlayerCraft::Update(float deltaTime)
{
	Component::Update(deltaTime);
	if (!m_isEnable) return;
	if (m_recipeList.empty())
		m_recipeList = m_recipeManager.GetSortedRecipeList(m_allowRecipeType);

	m_craftTimer.IsTimeOver();

	UpdateUIPanel();
}

void PlayerCraft::Finalize()
{
	Component::Finalize();
	if (!m_isEnable) return;
}

std::shared_ptr<UIPanel> PlayerCraft::GetOrBuidUIPanel()
{
	if (!m_uiPanel)
	{
		m_uiPanel = std::make_shared<UIPanel>();
		BuildUIPanel();
	}
	return m_uiPanel;
}

bool PlayerCraft::HasRequiredItems(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots) const
{

	if (!m_currentRecipe.lock()) return false;

	for (const auto& [needItem, needCount] : m_currentRecipe.lock()->GetRecipeInput())
	{
		if (needCount < CountItem(itemSlots, needItem)) return false;
	}

	return true;
}

int PlayerCraft::CountItem(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots, Item item) const
{
	int count = 0;

	for (const auto& itemSlot : itemSlots)
	{
		const auto& safeItemSlot = itemSlot.lock();

		if (!safeItemSlot) continue;

		const auto findItem = safeItemSlot->GetItem(item);

		if (!findItem) continue;
		count += findItem->GetItemCount();
	}

	return count;
}

bool PlayerCraft::CanStoreOutput() const
{
	const auto& safeRecipe = m_currentRecipe.lock();
	if (!safeRecipe) return false;

	for (const auto& [outItem, outCount] : safeRecipe->GetRecipeOutput())
	{
		const auto* stored = m_completionItemSlot->GetItem(0);

		if (stored)
		{
			if (stored->CheckAddItemCount(outCount) < outCount) return false;
			continue;
		}

		if (m_completionItemSlot->FindEmptyItemSlot() == -1) return false;
	}

	return true;
}

void PlayerCraft::ConsumeRequiredItems(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots)
{
	const auto& safeRecipe = m_currentRecipe.lock();
	if (!safeRecipe) return;

	for (const auto& [needItem, needCount] : safeRecipe->GetRecipeInput())
	{
		int remainCount = needCount;

		for (const auto& itemSlot : itemSlots)
		{
			if (remainCount <= 0) break;

			const auto& safeItemSlot = itemSlot.lock();
			if (!safeItemSlot) continue;

			for (int i = 0; i < safeItemSlot->GetSlotCount(); i++)
			{
				if (remainCount <= 0) break;

				auto* itemStack = safeItemSlot->GetItem(i);
				if (!itemStack) continue;
				if (itemStack->GetItemType() != needItem) continue;

				remainCount -= itemStack->MinusItemCount(remainCount);

				if (itemStack->GetItemCount() <= 0) safeItemSlot->RemoveItem(i);
			}
		}
	}
}

void PlayerCraft::BuildUIPanel()
{
	//Windowの生成
	UIFactory::MakeUIToPanel<UISquare>(m_uiPanel, kWindowPos, kWindowSize, kWindowColor, kWindowAlpha);

	const auto closeSize = Vector{ 30,30 };
	const auto closeOffset = Vector{ -10,10 };
	const auto closePos = kWindowPos + Vector{ kWindowSize.m_x * 0.5f , -kWindowSize.m_y * 0.5f } + Vector{ -closeSize.m_x * 0.5f, closeSize.m_y * 0.5f } + closeOffset;

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
			[this, &recipe]()
			{
				m_currentRecipe = recipe.second;
			});

		BuildRecipeUI(leftUpPos, recipe.second);
		index++;
	}

	const auto leftDown = kWindowPos + Vector{ -kWindowSize.m_x * 0.5f , kWindowSize.m_y * 0.5f };

	const auto completionOffset = Vector{ 0,10 };

	const auto completionPos = Vector{ kWindowPos.m_x,leftDown.m_y } - Vector{ 0,kCompletionItemBoxSize.m_y * 0.5f } - completionOffset;

	m_completionItemBox = std::make_shared<UIItemBox>(
		m_uiPanel, completionPos, kCompletionItemBoxSize);
	m_completionItemBox->SetImageAlpha(100);
	m_completionItemBox->SetOnDragBeginEvent([this]()
		{
			m_uiManager.ItemSelect(m_completionItemSlot, 0);
		});
}

void PlayerCraft::UpdateUIPanel()
{
	const auto& safeRecipe = m_currentRecipe.lock();
	if (!safeRecipe) return;

	const auto& outputs = safeRecipe->GetRecipeOutput();

	const auto& output = outputs.at((0));

	m_completionItemBox->SetGraphicID(ItemTable::GetGraphicID(output.first));
}

void PlayerCraft::BuildRecipeUI(Vector leftUpDrawPos, std::weak_ptr<Recipe> recipe)
{
	const auto& safeRecipe = recipe.lock();
	const auto& outputs = safeRecipe->GetRecipeOutput();
	const auto& inputs = safeRecipe->GetRecipeInput();

	auto startOffset = Vector{ 10,kRecipeSize.m_y * 0.25f };

	auto startPos = leftUpDrawPos + kItemBoxSize * 0.5f + startOffset;

	auto pos = Vector{};

	for (int i = 0; i < outputs.size(); i++)
	{
		pos = startPos + (Vector{ kItemBoxSize.m_x ,0 } + kItemBoxOffset) * i;

		const auto& itemBox = std::make_shared<UIItemBox>(
			m_uiPanel, pos, kItemBoxSize);
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
		m_uiPanel, arrowPos, kItemBoxSize * 1.5f, GraphicId::kArrow, kRecipeAlpha
	);

	for (int i = 0; i < inputs.size(); i++)
	{
		pos = startPos + (Vector{ kItemBoxSize.m_x ,0 } + kItemBoxOffset) * (i + outputs.size());

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

void PlayerCraft::Craft()
{
	const auto& safeRecipe = m_currentRecipe.lock();
	if (!safeRecipe) return;

	//出力用スロットを取得
	const auto& outputItem = m_completionItemSlot->GetItem(0);

	//入力用スロットを取得
	const auto& inputSlots = m_craftConsumeSlots;

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
		if (!isOk) return;

		inputSlots->MinusItemCount(needItemCount);
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

		inputSlots->MinusItemCount(needItemCount);
		outputItemSlot->AddItemStack(0, std::make_unique<ItemStack>(recipeOutputType, recipeOutputCount));
	}

	for (const auto& outputItem : output)
	{
		m_factoryManager.OnMakeItem(outputItem.first, outputItem.second);
	}

	if (!inputSlots->GetItemCount()) inputItems->RemoveItem(0);

	return true;
}
