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

class AssemblingMachine :
    public FactoryComponent
{
public:
    AssemblingMachine(std::shared_ptr<Object> parentObject);
    virtual ~AssemblingMachine();

    void Init() override;
    void Update(float deltaTime) override;

    void UpdateUIPanel() override;

    std::unique_ptr<ItemStack> MakeItemStackFromThisComponent() override;

protected:

    void BuildUIPanel() override;

private:

    std::shared_ptr<UIItemBox> m_inputItemUI;
    std::shared_ptr<UIItemBox> m_outputItemUI;
    std::shared_ptr<UIItemBox> m_fuelItemUI;

    std::weak_ptr<FuelSystem> m_fuelSystem;
    std::weak_ptr<ManufacturingSystem> m_manufacturingSystem;
};

