#pragma once
#include "FactoryComponent.h"
#include <vector>
#include <functional>
#include <memory>

class Object;
class Square3D;
class PlayerInput;
class ItemStack;
class Recipe;
class FuelSystem;
class ManufacturingSystem;
class UIItemBox;

class Furnace :
    public FactoryComponent
{
public:
    Furnace(std::weak_ptr<Object> parentObject);
    virtual ~Furnace() {};

    void Init() override;
    void Update() override;

    bool TryInsert(ItemStack* item, int count) override;

    /// <summary>
    /// アイテムのすべての所有権を引数で受け取った配列に移動する。
    /// </summary>
    /// <param name="result"></param>
    void GetAllItemOwnership(std::vector<std::pair<std::shared_ptr<ItemStack>, int>>* result) override;

    void UpdateUIPanel() override;

protected:

    void BuildUIPanel() override;

private:

    std::shared_ptr<UIItemBox> m_inputItemUI;
    std::shared_ptr<UIItemBox> m_outputItemUI;
    std::shared_ptr<UIItemBox> m_fuelItemUI;

    std::weak_ptr<FuelSystem> m_fuelSystem;
    std::weak_ptr<ManufacturingSystem> m_manufacturingSystem;
};

