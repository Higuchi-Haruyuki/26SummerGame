#pragma once
#include "FactoryComponent.h"

class Object;
class ItemStack;
class UIItemBox;

class Chest :
    public FactoryComponent
{
public:
    Chest(std::weak_ptr<Object> obj);
    ~Chest();

    void Init() override;

    void UpdateUIPanel() override;

protected:

    void BuildUIPanel() override;

private:

    std::vector<std::shared_ptr<UIItemBox>> m_itemBoxUIs;
};

