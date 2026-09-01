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
#include "Debug.h"
#include "StringUtil.h"
#include "Timer.h"
#include <numeric>
#include <format>
#include "ItemStackFactory.h"
#include "UIProgressBar.h"
#include "PlayerItem.h"
#include "UIText.h"
#include "ContainerUtil.h"
#include "Color.h"
#include "FactoryManager.h"

namespace
{

	/*UI*/
	//windowの定数
	const Vector kWindowPos = { Game::kDisplaySize.m_x * 0.75f , Game::kDisplaySize.m_y * 0.4f };
	const Vector kWindowSize = { 600,800,0 };
	constexpr unsigned int kWindowColor = static_cast<unsigned int>(Color::kMainColor);
	constexpr int kWindowAlpha = 200;

	inline Vector GetWindowRightUp() { return kWindowPos + Vector{ +kWindowSize.m_x * 0.5f,-kWindowSize.m_y * 0.5f }; }
	inline Vector GetWindowLeftDown() { return kWindowPos + Vector{ -kWindowSize.m_x * 0.5f,kWindowSize.m_y * 0.5f}; }

	//ウィンドウを消すボタンのUI
	const Vector kCloseSize = { 30,30 };
	const Vector kCloseOffset = { -10,10 };
	const Vector kClosePos = GetWindowRightUp() + Vector{-kCloseSize.m_x * 0.5f, kCloseSize.m_y * 0.5f} + kCloseOffset;

	//1レシピあたりの四角
	const Vector kRecipeSize = { 580,70,0 };
	constexpr unsigned int kRecipeDisableColor = static_cast<unsigned int>(Color::kDarkSubColor);
	constexpr unsigned int kRecipeEnableColor = static_cast<unsigned int>(Color::kMainAccentColor);
	constexpr int kRecipeAlpha = 255;

	//レシピ間のオフセット
	const Vector kRecipeOffset = { 0,10 };

	//レシピリスト表示位置
	const Vector kRecipeListStartOffset = { 10,5 };
	const Vector kRecipeListFirstpos = kWindowPos - kWindowSize * 0.5f + kRecipeSize * 0.5f + kRecipeOffset + kRecipeListStartOffset;

	//アイテムボックスの定数
	const Vector kItemBoxSize = { 50,50,0 };
	constexpr int kItemBoxAlpha = 255;

	//アイテムボックス間のオフセット
	const Vector kItemBoxOffset = { 100,0 };

	//レシピキューの最大表示数
	constexpr int kMaxDisplayCraftQueue = 8;

	//レシピキューの表示位置
	const Vector kCraftQueueOffset = { 10, 0 };
	const Vector kCraftQueueFirstPos = GetWindowLeftDown() + Vector{ kItemBoxSize.m_x, -kItemBoxSize.m_y } + kCraftQueueOffset;

	//クラフト進捗のUI
	const auto kProgressBarSize = Vector{ kItemBoxSize.m_x,10 };
	const auto kProgressBarOffset = Vector{ 0,10 };
	const auto kProgressBarPos = kCraftQueueFirstPos + Vector{ 0,kItemBoxSize.m_y * 0.5f } + kProgressBarOffset;
	const auto kProgressBarColor = static_cast<unsigned int>(Color::kSubAccentColor);
}

PlayerCraft::PlayerCraft(std::weak_ptr<Object> parent) :
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

	m_playerItem = GetComponent<PlayerItem>();

	m_craftQueueUI.resize(kMaxDisplayCraftQueue);

	m_recipeList = m_recipeManager.GetSortedRecipeList(m_allowRecipeType);

	m_craftTimer.SetEnable(false);   //レシピが選ばれるまで動かさない
	m_craftTimer.SubscribeOnFinished([this]() { Craft();});

}

bool PlayerCraft::HasRequiredItems(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots) const
{

	if (!m_craftQueue.size())return false;

	const auto& safeRecipe = m_craftQueue.at(0).first.lock();

	for (const auto& [needItem, needCount] : safeRecipe->GetRecipeInput())
	{
		if (needCount > CountItem(itemSlots, needItem)) return false;
	}

	return true;
}

void PlayerCraft::Update()
{
	Component::Update();
	if (!m_isEnable) return;

	if (m_recipeList.empty())
		m_recipeList = m_recipeManager.GetSortedRecipeList(m_allowRecipeType);

	//製作していないときだけ、開始できるか判定する
	if (m_craftQueue.size())
	{
		const auto& safeRecipe = GetCraftQueue(0).first.lock();

		const bool isTimerEnable = m_craftTimer.IsEnable();

		const bool isEnoughItem = HasRequiredItems(m_craftConsumeSlots);

		const bool canStoreOutput = CanStoreOutput();

		if (safeRecipe && !isTimerEnable
			&& isEnoughItem && canStoreOutput)
		{
			m_craftTimer.SetDuration(static_cast<Second>(safeRecipe->GetRecipeProcessTime()));
			m_craftTimer.SetEnable(true);
		}
	}

	//Timerはポーリングしないと完了コールバックが呼ばれない
	m_craftTimer.IsTimeOver();

	UpdateUIPanel();

	for (const auto& [recipe,count] : m_craftQueue)	
	{
		Debug::Log(std::format("RecipeName: {}", static_cast<int>(recipe.lock()->GetRecipeName())));
	}
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

void PlayerCraft::BuildUIPanel()
{
	//Windowの生成
	UIFactory::MakeUIToPanel<UISquare>(m_uiPanel, kWindowPos, kWindowSize, kWindowColor, kWindowAlpha);

	//const auto& close = UIFactory::MakeUIToPanel<UIImage>(m_uiPanel, kClosePos, kCloseSize, GraphicId::kUIClose);
	//close.lock()->SubscribeOnClick([this]() {m_uiPanel->SetVisible(false); });

	auto index = 0;

	if (m_recipeList.empty())
		m_recipeList = m_recipeManager.GetSortedRecipeList(m_allowRecipeType);

	for (const auto& recipe : m_recipeList)
	{
		const auto pos = kRecipeListFirstpos + (Vector{ 0, kRecipeSize.m_y } + kRecipeOffset) * index;
		const auto leftUpPos = pos - kRecipeSize * 0.5f;

		const auto& recipeSquare = UIFactory::MakeUIToPanel<UISquare>(
			m_uiPanel, pos, kRecipeSize, kRecipeDisableColor, kRecipeAlpha
		);
		recipeSquare.lock()->SubscribeOnClick(
			[this, recipe]()
			{
				if (CanAddCraftQueue(recipe.first))
					AddCraftQueue(recipe.second);
			});

		m_recipeSquares.emplace(recipe.second->GetRecipeName(), recipeSquare);

		BuildRecipeUI(leftUpPos, recipe.second);
		index++;
	}

	const auto leftDown = kWindowPos + Vector{ -kWindowSize.m_x * 0.5f , kWindowSize.m_y * 0.5f };

	//完成するまでのバーを表示


	m_craftProgressBar = UIFactory::MakeUIToPanel<UIProgressBar>
		(m_uiPanel, kProgressBarPos, kProgressBarSize, kProgressBarColor,150);
	m_craftProgressBar.lock()->SetProgress(0.0f);

	BuildCraftQueueUI();
}

void PlayerCraft::BuildRecipeUI(Vector leftUpDrawPos, std::weak_ptr<Recipe> recipe)
{
	const auto& safeRecipe = recipe.lock();
	const auto& outputs = safeRecipe->GetRecipeOutput();
	const auto& inputs = safeRecipe->GetRecipeInput();

	auto startOffset = Vector{ 10,(kRecipeSize.m_y - kItemBoxSize.m_y)*0.5f };

	auto startPos = leftUpDrawPos + kItemBoxSize* 0.5f + startOffset;

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

void PlayerCraft::BuildCraftQueueUI()
{
	auto pos = kCraftQueueFirstPos;
	for (int i = 0; i < m_craftQueueUI.size(); i++)
	{
		const auto& itemBoxUI = std::make_shared<UIItemBox>
			(m_uiPanel, pos, kItemBoxSize);
		itemBoxUI->SetVisible(false);
		
		//一番左端のUIだけ
			int idx = i;
			itemBoxUI->SetOnClickEvent([this, idx]() {RemoveCraftQueue(idx); });

		m_craftQueueUI.at(i) = itemBoxUI;

		pos += Vector{ kCraftQueueOffset.m_x + kItemBoxSize.m_x };
	}
	
	m_craftProgressText = UIFactory::MakeUIToPanel<UIText>
		(
			m_uiPanel, 
			TextArgs{
				pos,
				TextPivot::CenterTop,
			}
		);
	m_craftProgressText.lock()->SetVisible(false);

}

void PlayerCraft::UpdateUIPanel()
{
	//レシピ部分の更新処理
	//クラフトキューの更新処理

	for (const auto& [recipeName, ui] : m_recipeSquares)
	{
		if (CanAddCraftQueue(recipeName))
		{
			ui.lock()->SetColor(kRecipeEnableColor);
			ui.lock()->SetAlpha(255);
		}
		else
		{
			ui.lock()->SetColor(kRecipeDisableColor);
			ui.lock()->SetAlpha(100);
		}

	}


	//クラフトキューの更新処理
	for (const auto& ui : m_craftQueueUI)
	{
		ui->SetVisible(false);
		ui->SetGraphicID(GraphicId::kNone);
		ui->SetText("");
	}
	m_craftProgressText.lock()->SetVisible(false);
	
	if (m_craftQueue.size())
	{
		const auto& safeRecipe = GetCraftQueue(0).first.lock();
		if (!safeRecipe) return;

		const auto& outputs = safeRecipe->GetRecipeOutput();

		const auto& output = outputs.at((0));

		int overflowCount = 0;

		for (int i = 0; i < m_craftQueue.size(); i++)
		{
			if (i >= m_craftQueueUI.size())
			{
				overflowCount++;
				continue;
			}

			const auto& ui = m_craftQueueUI.at(i);
			ui->SetVisible(true);

			const auto& [craft,craftCount] = GetCraftQueue(i);
			const auto& [outputItem,outputCount] = craft.lock()->GetRecipeOutput().at(0);

			auto graphicID = ItemTable::GetGraphicID(outputItem);
			ui->SetGraphicID(graphicID);
			ui->SetText("x" + StringUtil::IntToString(craftCount * outputCount));

		}

		if (overflowCount)
		{
			m_craftProgressText.lock()->SetVisible(true);
			m_craftProgressText.lock()->SetText("...x" + StringUtil::IntToString(overflowCount));
		}
	}

	//進捗バーの更新処理
	if(m_craftTimer.IsEnable() && m_craftQueue.size())
		m_craftProgressBar.lock()->SetProgress(m_craftTimer.GetElapsedTime(),0.0f,m_craftTimer.GetDuration());
	else
		m_craftProgressBar.lock()->SetProgress(0.0f);
}

void PlayerCraft::Craft()
{
	//製作中に素材が抜かれた／完成品が埋まった可能性があるので再確認する

	bool isCraftable = HasRequiredItems(m_craftConsumeSlots) && CanStoreOutput();

	if (!isCraftable)
	{
		m_craftTimer.SetEnable(false);
		return;
	}

	ConsumeRequiredItems(m_craftConsumeSlots);
	StoreOutput();

	SkipNextCraft();

	//次の製作はUpdateが条件を見て開始する
	m_craftTimer.SetEnable(false);
}

bool PlayerCraft::CanStoreOutput() const
{
	if (!m_craftQueue.size())return false;

	const auto& safeRecipe = GetCraftQueue(0).first.lock();
	if (!safeRecipe) return false;

	for (const auto& [outItem, outCount] : safeRecipe->GetRecipeOutput())
	{
		if(!m_playerItem.lock()->CanAddItem(outItem, outCount)) return false;
	}

	return true;
}

void PlayerCraft::ConsumeRequiredItems(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots)
{
	if (!m_craftQueue.size())return;

	const auto& safeRecipe = GetCraftQueue(0).first.lock();
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

void PlayerCraft::StoreOutput()
{
	if (!m_craftQueue.size())return;

	const auto& safeRecipe = GetCraftQueue(0).first.lock();
	if (!safeRecipe) return;

	for (const auto& [outItem, outCount] : safeRecipe->GetRecipeOutput())
	{
		auto item = ItemStackFactory::Make(outItem, outCount);

		if(m_playerItem.lock()->CanAddItem(outItem, outCount))
		{
			m_playerItem.lock()->AddItem(std::move(item), outCount);
			FactoryManager::GetInstance().OnMakeItem(outItem, outCount);
		}

	}

}

int PlayerCraft::CountItem(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots, Item item) const
{
	int count = 0;

	for (const auto& itemSlot : itemSlots)
	{
		const auto& safeItemSlot = itemSlot.lock();

		if (!safeItemSlot) continue;

		for (int i = 0; i < safeItemSlot->GetSlotCount(); i++)
		{
			const auto* itemStack = safeItemSlot->GetItem(i);
			if (!itemStack) continue;
			if (itemStack->GetItemType() != item) continue;

			count += itemStack->GetItemCount();
		}

	}

	return count;
}

void PlayerCraft::AddCraftQueue(std::weak_ptr<Recipe> newCraft)
{

	if (m_craftQueue.size())
	{
		//クラフトキューの末尾を取得
		auto& [endRecipe, endCount] = m_craftQueue.at(m_craftQueue.size() - 1);

		//同じレシピならまとめる
		if (endRecipe.lock()->GetRecipeName() == newCraft.lock()->GetRecipeName())
		{
			endCount++;
			return;
		}

	}

	m_craftQueue.push_back(std::make_pair(newCraft,1));
}

bool PlayerCraft::CanAddCraftQueue(RecipeName recipeName) const
{
	const auto& recipe = m_recipeList.at(recipeName);

	for (const auto& [needItem, needCount] : recipe->GetRecipeInput())
	{
		auto craftQueueCount = CountItemAtCraftQueue(needItem);
		if (needCount + craftQueueCount > CountItem(m_craftConsumeSlots,needItem)) return false;
	}

	return true;
}

void PlayerCraft::SkipNextCraft()
{
	auto& [recipe, count] = m_craftQueue.at(0);

	//作成個数を減少させる
	count--;

	//もう作成しないときクラフトキューから削除
	if (count <= 0) m_craftQueue.pop_front();
}

void PlayerCraft::RemoveCraftQueue(int index)
{
	m_craftQueue.erase(m_craftQueue.begin() + index);
}

int PlayerCraft::CountItemAtCraftQueue(Item item) const
{
	int count = 0;

	const auto findInputItem = [](std::weak_ptr<Recipe> recipe)-> std::pair<Item, int>
		{ return recipe.lock()->GetRecipeInput().at(0); };

	for (const auto& [recipe, craftCount] : m_craftQueue)
	{
		//レシピから入力アイテムと個数を取得
		const auto& [inputItem, inputCount] = findInputItem(recipe);

		//入力するアイテムと当該アイテムが違うとき
		if (inputItem != item) continue;

		count += inputCount * craftCount;
	}

	return count;
}



std::pair<std::weak_ptr<Recipe>, int> PlayerCraft::GetCraftQueue(int index) const
{
	if (index < 0 || index >= m_craftQueue.size()) return std::make_pair(std::weak_ptr<Recipe>(), -1);
	return m_craftQueue.at(index);
}
