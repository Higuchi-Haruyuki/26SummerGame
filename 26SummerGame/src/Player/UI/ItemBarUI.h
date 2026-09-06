#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "Component.h"
#include "ItemType.h"

class PlayerItem;
class UIManager;
class UIPanel;
class UIItemBox;
class UIImage;
class UIText;

// アイテムバーと取得通知を管理する。表示切り替えはPlayerUIが行う。
class ItemBarUI : public Component
{
public:
	explicit ItemBarUI(std::weak_ptr<Object> parentObject);
	~ItemBarUI() override = default;

	void Init() override;
	void Update() override;
	void Finalize() override;

	std::shared_ptr<UIPanel> GetPanel() const { return m_itemBarUIPanel; }

private:
	void InitItemGetUI();
	void InitItemBarItemSlot();
	Vector CalcItemBarItemSlotPos(int index) const;

	UIManager& m_uiManager;
	std::weak_ptr<PlayerItem> m_playerItem;
	std::shared_ptr<UIPanel> m_itemBarUIPanel;
	std::vector<std::shared_ptr<UIItemBox>> m_itemBarBoxes;
	std::pair<std::weak_ptr<UIImage>, std::weak_ptr<UIText>> m_getItemUI;
	std::pair<Item, int> m_lastGetItem{ Item::kNone, 0 };
};
