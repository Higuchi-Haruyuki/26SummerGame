#include "Quest_MakeItem.h"
#include "FactoryManager.h"

void Quest_MakeItem::Init()
{
	//最初の個数を保存
	m_startCount = m_factoryManager.GetItemMakeCount(m_itemType);
}

void Quest_MakeItem::ClearCheck()
{
	//現在の個数を取得
	int currentCount = m_factoryManager.GetItemMakeCount(m_itemType);

	//目標個数に到達していない
	if (currentCount - m_startCount < m_targetCount) return;

	//クリアイベントの発火
	if (m_onClear) m_onClear();

}
