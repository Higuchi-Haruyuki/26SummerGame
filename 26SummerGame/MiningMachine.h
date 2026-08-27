#pragma once
#include "FactoryComponent.h"

class MapManager;
class FuelSystem;
class UIItemBox;

/// <summary>
/// 採掘機 <-地面にある資源を採掘してアイテムとして取得する機械
/// </summary>
class MiningMachine :
    public FactoryComponent
{
public:
    MiningMachine(std::weak_ptr<Object> parentObject);
    virtual ~MiningMachine() {};

    void Init() override;
    void Update() override;

    void UpdateUIPanel() override;

    std::unique_ptr<ItemStack> MakeItemStackFromThisComponent() override;

protected:

    void BuildUIPanel() override;

private:
    
    std::shared_ptr<UIItemBox> m_itemUI;
    std::shared_ptr<UIItemBox> m_fuelItemUI;

    std::weak_ptr<FuelSystem> m_fuelSystem;

    MapManager& m_mapManager;
};

