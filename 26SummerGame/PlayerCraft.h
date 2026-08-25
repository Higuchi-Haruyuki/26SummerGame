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

enum class RecipeType;
enum class RecipeName;

class PlayerCraft :
    public Component
{
public:
    PlayerCraft(std::shared_ptr<Object> parent);
    ~PlayerCraft();

    void Init() override;
    void Update(float deltaTime) override;
    void Finalize() override;

    std::shared_ptr<UIPanel> GetOrBuidUIPanel();

    void SetAllowRecipeType(RecipeType recipeType) { m_allowRecipeType = recipeType; }

    void AddCraftConsumeSlot(std::weak_ptr<ItemSlot> slot) { m_craftConsumeSlots.push_back(slot); }

    bool HasRequiredItems(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots) const;

private:
    //指定アイテムがスロット群に合計何個あるか
    int CountItem(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots, Item item) const;

    bool CanStoreOutput() const;

    void StoreOutput();

    void ConsumeRequiredItems(const std::vector<std::weak_ptr<ItemSlot>>& itemSlots);

    void BuildUIPanel();

    void UpdateUIPanel();

    void BuildCraftQueueUI();

    void BuildRecipeUI(Vector leftUpDrawPos, std::weak_ptr<Recipe> recipe);

    void Craft();

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

    std::deque<std::weak_ptr<Recipe>> m_craftQueue;

    //UI要素
    std::weak_ptr<UIProgressBar> m_craftProgressBar;

    std::vector<std::shared_ptr<UIItemBox>> m_craftQueueUI;

    std::weak_ptr<UIText> m_craftProgressText;

};

