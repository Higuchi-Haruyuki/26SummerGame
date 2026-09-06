#pragma once

#include <memory>
#include <vector>

#include "Component.h"

class PlayerController;
class PlayerCraft;
class PlayerItem;
class UIManager;
class UIPanel;
class UIItemBox;

// インベントリの表示・操作とクラフトパネルの配置を管理する。
class InventoryUI : public Component
{
public:
	explicit InventoryUI(std::weak_ptr<Object> parentObject);
	~InventoryUI() override = default;

	void Init() override;
	void Update() override;
	void Finalize() override;

	std::shared_ptr<UIPanel> GetPanel() const { return m_inventoryUIPanel; }

private:
	Vector CalcInventoryItemSlotPos(int index) const;

	UIManager& m_uiManager;
	std::weak_ptr<PlayerController> m_playerController;
	std::weak_ptr<PlayerCraft> m_playerCraft;
	std::weak_ptr<PlayerItem> m_playerItem;
	std::shared_ptr<UIPanel> m_inventoryUIPanel;
	std::vector<std::shared_ptr<UIItemBox>> m_inventoryBoxes;
};
