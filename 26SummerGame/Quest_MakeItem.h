#pragma once
#include "Base_Quest.h"
#include "ItemType.h"

/// <summary>
/// アイテムの制作目標のクエスト
/// </summary>
class Quest_MakeItem :
    public Base_Quest
{
public:
    Quest_MakeItem(const std::string& name, const std::string& description, 
        Item itemType, int targetCount):
        Base_Quest(name,description),
        m_itemType(itemType),
        m_targetCount(targetCount)
    { }

    void Init() override;

    void ClearCheck() override;

private:

    Item m_itemType;

    int m_targetCount;

    int m_startCount = 0;
};

