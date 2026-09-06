#include "Camera.h"
#include "CharactorStateManager.h"
#include "Collider.h"
#include "Component.h"
#include "Debug.h"
#include "DrawText.h"
#include "FactoryComponent.h"
#include "Game.h"
#include "Object.h"
#include "PlayerController.h"
#include "PlayerInput.h"
#include "PlayerUI.h"
#include "RayCastResult.h"
#include "ResourceManager.h"
#include "ResourceType.h"
#include "UIFactory.h"
#include "UIImage.h"
#include "UIKeyPrompt.h"
#include "UIManager.h"
#include "UIPanel.h"
#include "UISquare.h"
#include "UIText.h"
#include "UI/QuestUI.h"
#include "UI/ItemBarUI.h"
#include "UI/InventoryUI.h"
#include "vector.h"
#include <algorithm>
#include <DxLib.h>
#include <format>
#include <memory>
#include <string>
#include "Color.h"
#include "UIProgressBar.h"
#include "MouseCursorPoint.h"
#include "ItemManager.h"

namespace
{
	/*操作キーのUIの定数*/
	//大きさ
	const Vector kKeyPromptSize = { 250,50 };
	//下に表示するUIの左の方の位置
	const Vector kKeyPromptDownLeftPos = { Game::kDisplaySize.m_x / 5 * 4 - 200,Game::kDisplaySize.m_y - 100 };
	//下に表示するUIの右の方の位置
	const Vector kKeyPromptDownRightPos = kKeyPromptDownLeftPos + Vector{ 285, 0 };
	//工業オブジェクトの名前と操作が出るUIの位置
	const Vector kKeyPromptFactoryUIPos = Game::kDisplaySize / 2 + Vector{ 100,100 };

	/*設置モード中の定数*/
	const Vector kRotationKeyPromptUIPos = Game::kDisplaySize / 2 + Vector{ 100,150 };

	/*資源採掘UI*/
	const Vector kMiningProgressBarSize = Vector{ 120,25 };
	constexpr unsigned int kMiningProgressBarColor = static_cast<unsigned int>(Color::kMainAccentColor);
	const Vector kMiningProgressBarOffset = Vector{20,20};


}

PlayerUI::PlayerUI(std::weak_ptr<Object> parentObject) :
	Component(parentObject), m_input(PlayerInput::GetInstance()),
	m_uiManager(UIManager::GetInstance()),
	m_resourceManager(ResourceManager::GetInstance())
{}

void PlayerUI::Init()
{
	GetComponentReference();

	GetPanel();

	const auto questUI = AddComponent<QuestUI>().lock();
	const auto itemBarUI = AddComponent<ItemBarUI>().lock();
	const auto inventoryUI = AddComponent<InventoryUI>().lock();

	//ステートと有効なパネルの配列を初期化する。
	m_activePanelByState =
	{
		{CharactorState::IDLE, {m_idleKeyPanel, itemBarUI->GetPanel(), questUI->GetPanel()} },
		{CharactorState::INSTALLATION,{m_installationKeyPanel, itemBarUI->GetPanel()}},
		{CharactorState::DESTROY,{m_destroyKeyPanel, itemBarUI->GetPanel()}},
		{CharactorState::OPEN_FACTORYUI,{m_factoryUIPanel, itemBarUI->GetPanel()}},
		{CharactorState::OPEN_INVENTORYUI,{itemBarUI->GetPanel(),inventoryUI->GetPanel()}}
	};

	//ステートの変更イベントを購読
	m_state.lock()->OnChangeState = [this](const CharactorState& beforeState, const CharactorState& nowState)
		{
			ChangeCharactorState(beforeState, nowState);
			ApplyUIVisibility(nowState);
		};

	//UIパネルの初期設定
	InitUIPanel();

	//現在のステートのUIが見えるようにする
	ApplyUIVisibility(m_state.lock()->GetCurrentState());

}

void PlayerUI::Update()
{
	GetComponentReference();

	SetChooseFactoryComponent(m_input.GetActiveDeviceType());

	UpdateUIPanel();

}

void PlayerUI::UpdateMiningProgressBar(float progress)
{
	const auto& safeBar = m_miningProgressBar.lock();
	if (!safeBar) return;

	auto pos = MouseCursorPoint::GetCurrentMousePos() + kMiningProgressBarOffset;

	safeBar->SetProgress(progress);
	safeBar->SetPosition(pos);

}

void PlayerUI::SetVisibleMiningProgressBar(bool visible)
{
	const auto& safeBar = m_miningProgressBar.lock();
	if (!safeBar) return;
	safeBar->SetVisible(visible);
}

void PlayerUI::UpdateResourceUIOnMouseCursor(Item item, const Vector& cursorPos)
{

	if (const auto& safeUI = m_resourceMiningUI.lock())
	{
		safeUI->SetText(ItemTable::ItemTypeToDisplayName(item));
		safeUI->SetPosition(cursorPos + kMiningProgressBarOffset);
	}
}

void PlayerUI::SetVisibleResourceUIOnMouseCursor(bool visible)
{

	if (const auto& safeUI = m_resourceMiningUI.lock())
	{
		safeUI->SetVisible(visible);
	}
}

void PlayerUI::GetComponentReference()
{
	if (!m_collider.lock())
		m_collider = GetComponent<Collider>();
	if (!m_playerController.lock())
		m_playerController = GetComponent<PlayerController>();
	if (!m_state.lock())
		m_state = GetComponent<CharactorStateManager>();
}

void PlayerUI::GetPanel()
{
	m_idleKeyPanel = m_uiManager.GetOrCreateScreen("idle", UILayer::Hud);
	m_installationKeyPanel = m_uiManager.GetOrCreateScreen("installaction", UILayer::Hud);
	m_destroyKeyPanel = m_uiManager.GetOrCreateScreen("destroy", UILayer::Hud);
	m_factoryUIPanel = m_uiManager.GetOrCreateScreen("factoryUI", UILayer::Hud);
}

void PlayerUI::InitUIPanel()
{
	InitIdleUIPanel();

	InitInstallationUIPanel();

	InitDestroyUIPanel();

	InitFactoryUIPanel();
}

void PlayerUI::InitIdleUIPanel()
{
	const auto& inventory = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_idleKeyPanel, kKeyPromptDownLeftPos + Vector{ -1100 }, kKeyPromptSize, GraphicId::kKeyboardTab, "インベントリ");
	inventory.lock()->SubscribeOnClick([this]() {
		m_playerController.lock()->OpenInventoryUI();
		});

	const auto& installationMode = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_idleKeyPanel, kKeyPromptDownLeftPos, kKeyPromptSize, GraphicId::kKeyboardZ, "設置モード");
	installationMode.lock()->SubscribeOnClick([this]() {
		m_playerController.lock()->EnterInstallationMode();
		});

	const auto& destroyMode = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_idleKeyPanel, kKeyPromptDownRightPos, kKeyPromptSize, GraphicId::kKeyboardX, "破壊モード");
	destroyMode.lock()->SubscribeOnClick([this]() {
		m_playerController.lock()->EnterDestroyMode();
		});

	m_factoryUIPrompt = UIFactory::MakeUIToPanel<UIKeyPrompt>(
		m_idleKeyPanel, 
		kKeyPromptFactoryUIPos, kKeyPromptSize, GraphicId::kMouseLeft, "null");


	//破壊ゲージ
	m_miningProgressBar = UIFactory::MakeUIToPanel<UIProgressBar>(
		m_idleKeyPanel,
		Vector{}, kMiningProgressBarSize, kMiningProgressBarColor, 200);
	m_miningProgressBar.lock()->SetVisible(false);

	//資源の表示
	m_resourceMiningUI = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_idleKeyPanel, Vector{},kMiningProgressBarSize, GraphicId::kMouseRight, "null");
	m_resourceMiningUI.lock()->SetFontID(FontId::kFactoryUITextMini);
	m_resourceMiningUI.lock()->SetKeyImageExRate(0.2);
	m_resourceMiningUI.lock()->SetImagePositionOffset({ 5.0f , 2.5f });



}

void PlayerUI::InitInstallationUIPanel()
{
	//設置モード
	const auto& installationMode = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_installationKeyPanel, kKeyPromptDownLeftPos, kKeyPromptSize, 
		GraphicId::kKeyboardC, "設置モード終了");

	installationMode.lock()->SubscribeOnClick([this]() {
		m_playerController.lock()->ExitInstallationMode();
		});

	const auto& destroyMode = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_installationKeyPanel, kKeyPromptDownRightPos, kKeyPromptSize, 
		GraphicId::kKeyboardX, "破壊モード");

	destroyMode.lock()->SubscribeOnClick([this]() {
		m_playerController.lock()->EnterDestroyMode();
		});

	const auto& installationUI = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_installationKeyPanel, kKeyPromptFactoryUIPos, kKeyPromptSize, GraphicId::kMouseLeft, "設置");
	installationUI.lock()->SetIsHitTarget(false);

	const auto& rotationUI = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_installationKeyPanel, kRotationKeyPromptUIPos, kKeyPromptSize, GraphicId::kKeyboardR, "回転");
	rotationUI.lock()->SetIsHitTarget(false);
}

void PlayerUI::InitDestroyUIPanel()
{
	//破壊モード
	const auto& installationMode = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_destroyKeyPanel, kKeyPromptDownLeftPos, kKeyPromptSize, 
		GraphicId::kKeyboardZ, "設置モード");

	installationMode.lock()->SubscribeOnClick([this]() {
		m_playerController.lock()->EnterInstallationMode();
		});

	const auto& destroyMode = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_destroyKeyPanel, kKeyPromptDownRightPos, kKeyPromptSize, 
		GraphicId::kKeyboardC, "破壊モード終了");

	destroyMode.lock()->SubscribeOnClick([this]() {
		m_playerController.lock()->ExitDestroyMode();
		});

	m_destroyUIKeyPrompt = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_destroyKeyPanel, kKeyPromptFactoryUIPos, kKeyPromptSize, GraphicId::kMouseLeft, "破壊");
}

void PlayerUI::InitFactoryUIPanel()
{

	auto mainPos = Game::kDisplaySize * 0.5f;

	auto mainSize = Game::kDisplaySize * 0.5f;

	//一番下に描画する視覚上のパネル
	UIFactory::MakeUIToPanel<UISquare>(m_factoryUIPanel, 
		mainPos, mainSize, 
		static_cast<unsigned int>(Color::kMainColor), 200);

	//工業オブジェクトのなまえ
	m_factoryUITitleText = UIFactory::MakeUIToPanel<UIText>(m_factoryUIPanel, TextArgs{
				{ Game::kDisplaySize.m_x * 0.5f, Game::kDisplaySize.m_y * 0.3f},
				TextPivot::Center,
				FontId::kFactoryUITitle,
				"",
				static_cast<unsigned int>(Color::kMainCharColor),
				static_cast<unsigned int>(Color::kSubCharColor),
				255
		});

	const auto basepos = mainPos + Vector{ mainSize.m_x * 0.5f,-mainSize.m_y * 0.5f };
	const auto size = Vector{ 30,30 };

	const auto pos = basepos + Vector{ -size.m_x * 0.5f,size.m_y * 0.5f } + Vector{ -10,10 };

	const auto& close = UIFactory::MakeUIToPanel<UIImage>(m_factoryUIPanel, pos, size, GraphicId::kUIClose);
	close.lock()->SubscribeOnClick([this]()
		{
			m_playerController.lock()->CloseFactoryComponentUI();
		}
	);

}

void PlayerUI::SetChooseFactoryComponent(DeviceType currentDevice)
{
	if (!IsEnableSetChooseFactoryComponent()) return;
	if (m_uiManager.IsPointerHoverUI()) return;

	//コントローラーのとき
	if (currentDevice == DeviceType::kGamepad)
		//画面中心のオブジェクトを取得
		m_chooseFactory = m_playerController.lock()->GetScreenCenterFactoryObject();
	//キーボードのとき
	else if (currentDevice == DeviceType::kKeyboard)
		//マウスカーソルの位置のオブジェクトを取得
		m_chooseFactory = m_playerController.lock()->GetMousePointFactoryObject();
}

void PlayerUI::UpdateUIPanel()
{
	UpdateDestroyUIPanel();

	UpdateFactoryUIKeyPrompt();

	UpdateFactoryUIPanel();
}

void PlayerUI::UpdateDestroyUIPanel()
{
	const auto& safeFacCom = m_chooseFactory.lock();
	if (!safeFacCom)
		m_destroyUIKeyPrompt.lock()->SetVisible(false);
	else
		m_destroyUIKeyPrompt.lock()->SetVisible(true);
}

void PlayerUI::UpdateFactoryUIKeyPrompt()
{
	const auto& safeFactory = m_chooseFactory.lock();
	if (!safeFactory)
		m_factoryUIPrompt.lock()->SetVisible(false);
	else
	{
		m_factoryUIPrompt.lock()->SetVisible(true);
		m_factoryUIPrompt.lock()->SetText(ItemTable::ItemTypeToDisplayName(safeFactory->GetItemType()));
	}
}

void PlayerUI::UpdateFactoryUIPanel()
{
	//見つからなかったときは処理を終わらせる。
	const auto& safeFactory = m_chooseFactory.lock();
	if (!safeFactory) return;

	if (safeFactory != m_lastOpenedFactory.lock())
	{
		if (const auto& original = m_originalFactoryUIPanel.lock())
		{
			m_factoryUIPanel->RemoveChild(original);
		}

		//新しいパネル
		const auto& newPanel = safeFactory->GetOrBuildUIPanel();

		m_factoryUIPanel->AddChild(newPanel);

		m_originalFactoryUIPanel = newPanel;

		m_lastOpenedFactory = m_chooseFactory;
	}

	//更新処理
	safeFactory->UpdateUIPanel();

	//工業オブジェクトのUIパネルのタイトル
	m_factoryUITitleText.lock()->SetText(ItemTable::ItemTypeToDisplayName(safeFactory->GetItemType()));
}


void PlayerUI::ApplyUIVisibility(const CharactorState& state)
{
	SetAllUIVisible(false);

	SetUIVisible(state, true);
}

void PlayerUI::SetAllUIVisible(bool visible)
{
	for (auto& [s, panels] : m_activePanelByState)
	{
		std::for_each(panels.begin(), panels.end(), [visible](const std::shared_ptr<UIPanel>& panel) {
			panel->SetVisible(visible);
			});
	}
}

void PlayerUI::SetUIVisible(const CharactorState& state, bool visible)
{
	auto it = m_activePanelByState.find(state);

	if (it == m_activePanelByState.end()) return;

	const auto& panels = it->second;

	std::for_each(panels.begin(), panels.end(), [visible](const std::shared_ptr<UIPanel>& panel) {
		panel->SetVisible(visible);
		});
}

bool PlayerUI::IsCurrentState(CharactorState state) const
{
	return m_state.lock()->CheckCurrentState(state);
}

bool PlayerUI::IsEnableSetChooseFactoryComponent() const
{
	if (IsCurrentState(CharactorState::IDLE)) return true;
	if (IsCurrentState(CharactorState::INSTALLATION)) return true;
	if (IsCurrentState(CharactorState::DESTROY)) return true;
	return false;
}

void PlayerUI::ChangeCharactorState(const CharactorState& lastState, const CharactorState& nowState)
{
	switch (nowState)
	{
	case CharactorState::OPEN_FACTORYUI:

		//何もUIを開いていないとき
		if (!m_chooseFactory.lock())
		{
			//ステートをアイドルに戻す
			m_state.lock()->ChangeState(CharactorState::IDLE);
			return;
		};

		break;

	default:
		break;
	}
}

