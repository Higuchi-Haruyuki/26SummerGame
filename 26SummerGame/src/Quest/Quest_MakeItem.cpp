#include "Quest_MakeItem.h"
#include "FactoryManager.h"

void Quest_MakeItem::Init()
{
	//最初の個数を保存
	m_startCount = m_factoryManager.GetItemMakeCount(m_itemType);
}

bool Quest_MakeItem::ClearCheck()
{
	//現在の個数を取得
	const int currentCount = m_factoryManager.GetItemMakeCount(m_itemType);

	return currentCount - m_startCount >= m_targetCount;
}

float Quest_MakeItem::GetProgress() const
{
	//現在の個数を取得
	auto currentCount = static_cast<float>(m_factoryManager.GetItemMakeCount(m_itemType));

	auto progressCount = static_cast<float>(currentCount - m_startCount);
	
	return progressCount / m_targetCount;
	
}
