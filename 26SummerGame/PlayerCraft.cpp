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

namespace
{

	/*UI*/
	//windowの定数
	const Vector kWindowPos = { Game::kDisplaySize.m_x * 0.75f , Game::kDisplaySize.m_y * 0.4f };
	const Vector kWindowSize = { 600,800,0 };
	constexpr unsigned int kWindowColor = 0xd3d3d3;
	constexpr int kWindowAlpha = 200;

	inline Vector GetWindowRightUp() { return kWindowPos + Vector{ +kWindowSize.m_x * 0.5f,-kWindowSize.m_y * 0.5f }; }
	inline Vector GetWindowLeftDown() { return kWindowPos + Vector{ -kWindowSize.m_x * 0.5f,kWindowSize.m_y * 0.5f}; }

	//ウィンドウを消すボタンのUI
	const Vector kCloseSize = { 30,30 };
	const Vector kCloseOffset = { -10,10 };
	const Vector kClosePos = GetWindowRightUp() + Vector{-kCloseSize.m_x * 0.5f, kCloseSize.m_y * 0.5f} + kCloseOffset;

	//1レシピあたりの四角
	const Vector kRecipeSize = { 580,100,0 };
	constexpr unsigned int kRecipeColor = 0xf5f5f5;
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

	m_playerItem = GetParentObject()->GetComponent<PlayerItem>();

	m_craftQueueUI.resize(kMaxDisplayCraftQueue);

	m_recipeList = m_recipeManager.GetSortedRecipeList(m_allowRecipeType);

	m_craftTimer.SetEnable(false);   //レシピが選ばれるまで動かさない
	m_craftTimer.SubscribeOnFinished([this]() { Craft();});

}

void PlayerCraft::Update(float deltaTime)
{
	Component::Update(deltaTime);
	if (!m_isEnable) return;

	if (m_recipeList.empty())
		m_recipeList = m_recipeManager.GetSortedRecipeList(m_allowRecipeType);

	//製作していないときだけ、開始できるか判定する
	if (m_craftQueue.size())
	{
		const auto& safeRecipe = m_craftQueue.at(0).lock();

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

	for (const auto& recipe : m_craftQueue)	
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

bool PlayerCraft::HasRequiredItems(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots) const
{

	if (!m_craftQueue.size())return false;

	const auto& safeRecipe = m_craftQueue.at(0).lock();

	for (const auto& [needItem, needCount] : safeRecipe->GetRecipeInput())
	{
		if (needCount > CountItem(itemSlots, needItem)) return false;
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

bool PlayerCraft::CanStoreOutput() const
{
	if (!m_craftQueue.size())return false;

	const auto& safeRecipe = m_craftQueue.at(0).lock();
	if (!safeRecipe) return false;

	for (const auto& [outItem, outCount] : safeRecipe->GetRecipeOutput())
	{
		if(!m_playerItem.lock()->CanAddItem(outItem, outCount)) return false;
	}

	return true;
}

void PlayerCraft::StoreOutput()
{
	if (!m_craftQueue.size())return;

	const auto& safeRecipe = m_craftQueue.at(0).lock();
	if (!safeRecipe) return;

	for (const auto& [outItem, outCount] : safeRecipe->GetRecipeOutput())
	{
		auto item = ItemStackFactory::Make(outItem, outCount);

		if(m_playerItem.lock()->CanAddItem(outItem, outCount)) 
			m_playerItem.lock()->AddItem(std::move(item), outCount);

	}

}

void PlayerCraft::ConsumeRequiredItems(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots)
{
	if (!m_craftQueue.size())return;

	const auto& safeRecipe = m_craftQueue.at(0).lock();
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
			m_uiPanel, pos, kRecipeSize, kRecipeColor, kRecipeAlpha
		);
		recipeSquare.lock()->SubscribeOnClick(
			[this, recipe]()
			{
				m_craftQueue.push_back(recipe.second);
			});

		BuildRecipeUI(leftUpPos, recipe.second);
		index++;
	}

	const auto leftDown = kWindowPos + Vector{ -kWindowSize.m_x * 0.5f , kWindowSize.m_y * 0.5f };

	//完成するまでのバーを表示


	m_craftProgressBar = UIFactory::MakeUIToPanel<UIProgressBar>(m_uiPanel, kProgressBarPos, kProgressBarSize,0xff0000,150);
	m_craftProgressBar.lock()->SetProgress(0.0f);

	BuildCraftQueueUI();
}

void PlayerCraft::UpdateUIPanel()
{
	for (const auto& ui : m_craftQueueUI)
	{
		ui->SetVisible(false);
		ui->SetGraphicID(GraphicId::kNone);
		ui->SetText("");
	}

	m_craftProgressText.lock()->SetVisible(false);

	if (m_craftQueue.size())
	{
		const auto& safeRecipe = m_craftQueue.at(0).lock();
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

			const auto& craft = m_craftQueue.at(i).lock();
			const auto& [item, count] = craft->GetRecipeOutput().at(0);

			auto graphicID = ItemTable::GetGraphicID(item);
			ui->SetGraphicID(graphicID);
			ui->SetText("x" + StringUtil::IntToString(count));

		}

		if (overflowCount)
		{
			m_craftProgressText.lock()->SetVisible(true);
			m_craftProgressText.lock()->SetText("...x" + StringUtil::IntToString(overflowCount));
		}
	}

	if(m_craftTimer.IsEnable())
		m_craftProgressBar.lock()->SetProgress(m_craftTimer.GetElapsedTime(),0.0f,m_craftTimer.GetDuration());
	else
		m_craftProgressBar.lock()->SetProgress(0.0f);
}

void PlayerCraft::BuildCraftQueueUI()
{
	auto pos = kCraftQueueFirstPos;
	for (int i = 0; i < m_craftQueueUI.size(); i++)
	{
		const auto& itemBoxUI = std::make_shared<UIItemBox>
			(m_uiPanel, pos, kItemBoxSize);
		itemBoxUI->SetVisible(false);
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
	//製作中に素材が抜かれた／完成品が埋まった可能性があるので再確認する
	if (!HasRequiredItems(m_craftConsumeSlots) || !CanStoreOutput())
	{
		m_craftTimer.SetEnable(false);
		return;
	}

	ConsumeRequiredItems(m_craftConsumeSlots);
	StoreOutput();

	m_craftQueue.pop_front();

	//次の製作はUpdateが条件を見て開始する
	m_craftTimer.SetEnable(false);
}
