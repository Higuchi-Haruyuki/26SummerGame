#pragma once
#include "Component.h"
#include "Recipe.h"
#include "Timer.h"
#include <unordered_map>
#include <memory>
#include <deque>

class UIPanel;
class UIItemBox;
class Vector;
class RecipeManager;
class ItemSlot;
class UIManager;
class ItemStack;
class UIProgressBar;
class PlayerItem;
class UIText;
class UISquare;

enum class RecipeType;
enum class RecipeName;

class PlayerCraft :
	public Component
{
public:
	PlayerCraft(std::weak_ptr<Object> parent);
	~PlayerCraft();

	void Init() override;
	bool HasRequiredItems(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots) const;
	void Update() override;
	void Finalize() override;

	std::shared_ptr<UIPanel> GetOrBuidUIPanel();

	void AddCraftConsumeSlot(std::weak_ptr<ItemSlot> slot) { m_craftConsumeSlots.push_back(slot); }

	void SetAllowRecipeType(RecipeType recipeType) { m_allowRecipeType = recipeType; }

private:
	void BuildUIPanel();

	void BuildRecipeUI(Vector leftUpDrawPos, std::weak_ptr<Recipe> recipe);

	void BuildCraftQueueUI();

	void UpdateUIPanel();

	void Craft();

	bool CanStoreOutput() const;

	void ConsumeRequiredItems(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots);

	void StoreOutput();

	//指定アイテムがスロット群に合計何個あるか
	int CountItem(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots, Item item) const;

	void AddCraftQueue(std::weak_ptr<Recipe> newCraft);

	/// <summary>
	/// クラフトキューで使用する予定のアイテムを考慮して、追加できるかを判定する。
	/// </summary>
	/// <param name="recipeName"></param>
	/// <returns></returns>
	bool CanAddCraftQueue(RecipeName recipeName) const;

	/// <summary>
	/// 現在1番先頭にあるクラフトをスキップする
	/// </summary>
	void SkipNextCraft();

	/// <summary>
	/// クラフトキューで任意位置にあるクラフトをスキップする
	/// </summary>
	/// <param name="index"></param>
	void RemoveCraftQueue(int index);

	/// <summary>
	///指定アイテムがクラフトキューに合計何個あるか
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	int CountItemAtCraftQueue(Item item) const;

	std::pair<std::weak_ptr<Recipe>, int> GetCraftQueue(int index) const;

private:
	//シングルトンへの参照
	RecipeManager& m_recipeManager;
	UIManager& m_uiManager;

	std::weak_ptr<PlayerItem> m_playerItem;

	//制作時にアイテムを消費する場所
	std::vector<std::weak_ptr<ItemSlot>> m_craftConsumeSlots;

	RecipeType m_allowRecipeType = RecipeType::kNone;

	//レシピ設定用のパネル
	std::shared_ptr<UIPanel> m_uiPanel;

	std::unordered_map<RecipeName, std::shared_ptr<Recipe>> m_recipeList;

	Timer m_craftTimer{0};

	std::deque<std::pair<std::weak_ptr<Recipe>, int>> m_craftQueue;


	//UI要素
	std::map<RecipeName,std::weak_ptr<UISquare>> m_recipeSquares;

	std::weak_ptr<UIProgressBar> m_craftProgressBar;

	std::vector<std::shared_ptr<UIItemBox>> m_craftQueueUI;

	std::weak_ptr<UIText> m_craftProgressText;

};

