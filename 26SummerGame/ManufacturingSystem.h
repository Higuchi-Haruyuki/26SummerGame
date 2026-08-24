#pragma once
#include "Component.h"
#include <vector>
#include "Recipe.h"
#include <unordered_map>

class Recipe;
class RecipeManager;
class FactoryManager;
class ItemManager;
class ItemSlot;
class UIPanel;

/// <summary>
/// 工業オブジェクトのアイテムの加工をするコンポーネント
/// </summary>
class ManufacturingSystem :
    public Component
{
public:
    ManufacturingSystem(std::shared_ptr<Object> parentObject);
    virtual ~ManufacturingSystem() {};

    void Init() override;
    void Update(float deltaTime) override;
    void Finalize() override;

    std::shared_ptr<UIPanel> GetOrBuidUIPanel();

    /// <summary>
    /// 引数のアイテムの配列からレシピを設定する。
    /// </summary>
    /// <param name="items"></param>
    void SetRecipe(const ItemSlot* items);

    void SetAllowRecipeType(RecipeType recipeType) { m_allowRecipeType = recipeType; }

    bool Manufacture(ItemSlot* inputItems, ItemSlot* outputItems);

private:

    int CalcNeedItemCount(const ItemSlot* items) const;

    void BuildUIPanel();

    void UpdateUIPanel();

    void BuildRecipeUI(int index, std::weak_ptr<Recipe> recipe);

private:
    //シングルトンへの参照
    RecipeManager& m_recipeManager;
    FactoryManager& m_factoryManager;

    //現在設定されているレシピ
    std::weak_ptr<Recipe> m_currentRecipe;

    RecipeType m_allowRecipeType = RecipeType::kNone;

    //レシピ設定用のパネル
    std::shared_ptr<UIPanel> m_uiPanel;

    std::unordered_map<RecipeName, std::shared_ptr<Recipe>> m_recipeList;
};

