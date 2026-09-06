#pragma once

#include <memory>

#include "Component.h"

class QuestManager;
class UIManager;
class UIPanel;
class UIProgressBar;
class UIText;
class UISquare;
class PlayerInput;

// クエストパネルの生成・表示内容の更新・登録解除を担当する。
// 状態に応じたパネルの表示切り替えはPlayerUIが行う。
class QuestUI : public Component
{
public:
	explicit QuestUI(std::weak_ptr<Object> parentObject);
	~QuestUI() override = default;

	void Init() override;
	void Update() override;
	void Finalize() override;

	std::shared_ptr<UIPanel> GetPanel() const { return m_questUIPanel; }

private:
	std::shared_ptr<UIPanel> GenerateTabPanel(int index);
	void InitBaseContorollTab();
	void InitInventoryContorollTab();
	void InitInstallationTab();
	void InitDestroyTab();

	void SetAllPanelInvisible();
	void SetPanelVisible(int index, bool visible);
	void SetPanelVisible(std::weak_ptr<UIPanel> panel, bool visible);
private:
	UIManager& m_uiManager;
	QuestManager& m_questManager;
	PlayerInput& m_playerInput;

	std::shared_ptr<UIPanel> m_questUIPanel;
	std::weak_ptr<UIText> m_questText;
	std::weak_ptr<UIProgressBar> m_questProgressBar;
	std::weak_ptr<UIText> m_questProgressText;

	std::pair<std::weak_ptr<UISquare>,std::weak_ptr<UIText>> m_helpVisibleButton;
	std::weak_ptr<UIPanel> m_helpPanel;

	std::vector<std::weak_ptr<UISquare>> m_tabSquares;
	std::vector<std::weak_ptr<UIPanel>> m_tabPanels;
};
