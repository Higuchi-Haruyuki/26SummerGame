#include "Base_Quest.h"
#include "Camera.h"
#include "CharactorStateManager.h"
#include "Collider.h"
#include "Component.h"
#include "Debug.h"
#include "DrawText.h"
#include "FactoryComponent.h"
#include "Game.h"
#include "ItemStack.h"
#include "Object.h"
#include "PlayerController.h"
#include "PlayerInput.h"
#include "PlayerItem.h"
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
#include "QuestManager.h"
#include "vector.h"
#include <algorithm>
#include <DxLib.h>
#include <format>
#include <memory>
#include <string>
#include "ItemManager.h"
#include "UIItemBox.h"
#include "PlayerCraft.h"
#include "ItemSlot.h"

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

	/*アイテムバーの定数*/
	const Vector kItemBarPos =
	{
		Game::kDisplaySize.m_x / 2,
		Game::kDisplaySize.m_y / 8 * 7 - 50
	};
	const Vector kItemBarSize = { 800,80 };
	constexpr unsigned int kItemBarColor = 0xd3d3d3;
	constexpr int kItemBarAlpha = 100;

	/*アイテムバーのアイテム表示に関する定数*/
	constexpr int kItemBarItemCount = 10;
	constexpr int kItemBarSlotOffsetX = 10;
	const Vector kItemBarItemSlotSize = { (kItemBarSize.m_x - 100) / kItemBarItemCount , kItemBarSize.m_y - 10 };
	constexpr unsigned int kItemBarItemSlotColor = 0xffffff;
	constexpr int kItemBarItemSlotAlpha = 255;

	/*設置モード中の定数*/
	const Vector kRotationKeyPromptUIPos = Game::kDisplaySize / 2 + Vector{ 100,150 };

	/*クエスト表示UIの定数*/
	const Vector kQuestUIPos = { 150.0f, Game::kDisplaySize.m_y / 2 - 100 };
	const Vector kQuestUISize = { 300.0f, 100.0f };

	/*インベントリUIの定数*/
	const Vector kInventoryPos =
	{
		Game::kDisplaySize.m_x / 4,
		Game::kDisplaySize.m_y / 2.5f
	};
	const Vector kInventorySize = { 850,810 };
	constexpr unsigned int kInventoryColor = 0xd3d3d3;
	constexpr int kInventoryAlpha = 130;

	constexpr int kInventoryItemCountWidth = 10;
	constexpr int kInventoryItemCountHeight = 10;
	constexpr int kInventorySlotOffsetX = 10;
	constexpr int kInventorySlotOffsetY = 10;
}

PlayerUI::PlayerUI(std::shared_ptr<Object> parentObject) :
	Component(parentObject), m_input(PlayerInput::GetInstance()),
	m_uiManager(UIManager::GetInstance()),
	m_resourceManager(ResourceManager::GetInstance()),
	m_questManager(QuestManager::GetInstance())
{}

void PlayerUI::Init()
{
	GetComponentReference();

	//クラフト可能なレシピタイプを設定する。
	m_playerCraft.lock()->SetAllowRecipeType(RecipeType::kPlayerCraft);

	CreatePanel();

	//ステートと有効なパネルの配列を初期化する。
	m_activePanelByState =
	{
		{CharactorState::IDLE, {m_idleKeyPanel, m_itemBarUIPanel, m_questUIPanel} },
		{CharactorState::INSTALLATION,{m_installationKeyPanel, m_itemBarUIPanel}},
		{CharactorState::DESTROY,{m_destroyKeyPanel, m_itemBarUIPanel}},
		{CharactorState::OPEN_FACTORYUI,{m_factoryUIPanel, m_itemBarUIPanel}},
		{CharactorState::OPEN_INVENTORYUI,{m_itemBarUIPanel,m_inventoryUIPanel}}
	};

	//ステートの変更イベントを購読
	m_state->OnChangeState = [this](const CharactorState& beforeState, const CharactorState& nowState)
		{
			ChangeCharactorState(beforeState, nowState);
			ApplyUIVisibility(nowState);
		};

	//UIパネルの初期設定
	InitUIPanel();

	//現在のステートのUIが見えるようにする
	ApplyUIVisibility(m_state->GetCurrentState());

}

void PlayerUI::Update(float deltaTime)
{
	GetComponentReference();

	SetChooseFactoryComponent(m_input.GetActiveDeviceType());

	UpdateUIPanel();

}

void PlayerUI::GetComponentReference()
{
	if (!m_collider)
		m_collider = GetParentObject()->GetComponent<Collider>();
	if (!m_playerController)
		m_playerController = GetParentObject()->GetComponent<PlayerController>();
	if (!m_playerItem)
		m_playerItem = GetParentObject()->GetComponent<PlayerItem>();
	if (!m_state)
		m_state = GetParentObject()->GetComponent<CharactorStateManager>();
	if (!m_playerCraft.lock())
		m_playerCraft = GetParentObject()->GetComponent<PlayerCraft>();
}

void PlayerUI::CreatePanel()
{
	m_idleKeyPanel = m_uiManager.GetOrCreateScreen("idle", UILayer::Hud);
	m_installationKeyPanel = m_uiManager.GetOrCreateScreen("installaction", UILayer::Hud);
	m_destroyKeyPanel = m_uiManager.GetOrCreateScreen("destroy", UILayer::Hud);
	m_factoryUIPanel = m_uiManager.GetOrCreateScreen("factoryUI", UILayer::Hud);
	m_itemBarUIPanel = m_uiManager.GetOrCreateScreen("itemBar", UILayer::Hud);
	m_questUIPanel = m_uiManager.GetOrCreateScreen("quest", UILayer::Hud);
	m_inventoryUIPanel = m_uiManager.GetOrCreateScreen("inventory", UILayer::Hud);
}

void PlayerUI::InitUIPanel()
{
	InitIdleUIPanel();

	InitInstallationUIPanel();

	InitDestroyUIPanel();

	InitFactoryUIPanel();

	InitItemBarUIPanel();

	InitQuestUIPanel();

	InitInventoryItemPanel();
}

void PlayerUI::InitIdleUIPanel()
{
	const auto& installationMode = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_idleKeyPanel, kKeyPromptDownLeftPos, kKeyPromptSize, GraphicId::kKeyboardZ, "設置モード");
	installationMode.lock()->SetOnClick([this]() {
		m_playerController->EnterInstallationMode(); 
		});

	const auto& destroyMode = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_idleKeyPanel, kKeyPromptDownRightPos, kKeyPromptSize, GraphicId::kKeyboardX, "破壊モード");
	destroyMode.lock()->SetOnClick([this]() {
		m_playerController->EnterDestroyMode();  
		});

	m_factoryUIPrompt = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_idleKeyPanel, kKeyPromptFactoryUIPos, kKeyPromptSize, GraphicId::kMouseLeft, "null");
}

void PlayerUI::InitInstallationUIPanel()
{
	//設置モード
	const auto& installationMode = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_installationKeyPanel, kKeyPromptDownLeftPos, kKeyPromptSize, 
		GraphicId::kKeyboardC, "設置モード終了");

	installationMode.lock()->SetOnClick([this]() {
		m_playerController->ExitInstallationMode();
		});

	const auto& destroyMode = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_installationKeyPanel, kKeyPromptDownRightPos, kKeyPromptSize, 
		GraphicId::kKeyboardX, "破壊モード");

	destroyMode.lock()->SetOnClick([this]() {
		m_playerController->EnterDestroyMode();
		});

	UIFactory::MakeUIToPanel<UIKeyPrompt>(m_installationKeyPanel, kKeyPromptFactoryUIPos, kKeyPromptSize, GraphicId::kMouseLeft, "設置");

	UIFactory::MakeUIToPanel<UIKeyPrompt>(m_installationKeyPanel, kRotationKeyPromptUIPos, kKeyPromptSize, GraphicId::kKeyboardR, "回転");
}

void PlayerUI::InitDestroyUIPanel()
{
	//破壊モード
	const auto& installationMode = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_destroyKeyPanel, kKeyPromptDownLeftPos, kKeyPromptSize, 
		GraphicId::kKeyboardZ, "設置モード");

	installationMode.lock()->SetOnClick([this]() {
		m_playerController->EnterInstallationMode();
		});

	const auto& destroyMode = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_destroyKeyPanel, kKeyPromptDownRightPos, kKeyPromptSize, 
		GraphicId::kKeyboardC, "破壊モード終了");

	destroyMode.lock()->SetOnClick([this]() {
		m_playerController->ExitDestroyMode();
		});

	m_destroyUIKeyPrompt = UIFactory::MakeUIToPanel<UIKeyPrompt>(m_destroyKeyPanel, kKeyPromptFactoryUIPos, kKeyPromptSize, GraphicId::kMouseLeft, "破壊");
}

void PlayerUI::InitFactoryUIPanel()
{

	auto mainPos = Game::kDisplaySize * 0.5f;

	auto mainSize = Game::kDisplaySize * 0.5f;

	//一番下に描画する視覚上のパネル
	UIFactory::MakeUIToPanel<UISquare>(m_factoryUIPanel, mainPos, mainSize, 0xd3d3d3, 200);

	//工業オブジェクトのなまえ
	m_factoryUITitleText = UIFactory::MakeUIToPanel<UIText>(m_factoryUIPanel, TextArgs{
				{ Game::kDisplaySize.m_x * 0.5f, Game::kDisplaySize.m_y * 0.3f},
				TextPivot::Center,
				FontId::kFactoryUITitle,
				"",
				0x000000,
				0xffffff,
				255
		});

	const auto basepos = mainPos + Vector{ mainSize.m_x * 0.5f,-mainSize.m_y * 0.5f };
	const auto size = Vector{ 30,30 };

	const auto pos = basepos + Vector{ -size.m_x * 0.5f,size.m_y * 0.5f } + Vector{ -10,10 };

	const auto& close = UIFactory::MakeUIToPanel<UIImage>(m_factoryUIPanel, pos, size, GraphicId::kUIClose);
	close.lock()->SubscribeOnClick([this]()
		{
			m_playerController->CloseFactoryComponentUI();
		}
	);

}

void PlayerUI::InitItemBarUIPanel()
{
	//アイテムバーの背景部分
	UIFactory::MakeUIToPanel<UISquare>(m_itemBarUIPanel, kItemBarPos, kItemBarSize, kItemBarColor, kItemBarAlpha);

	/*アイテムバーのアイテムスロットの部分*/
	InitItemBarItemSlot();
}

void PlayerUI::InitQuestUIPanel()
{
	UIFactory::MakeUIToPanel<UISquare>(m_questUIPanel, kQuestUIPos, kQuestUISize, 0xd3d3d3, 200);

	m_questText = UIFactory::MakeUIToPanel<UIText>(m_questUIPanel, TextArgs{
				kQuestUIPos + Vector{0,kQuestUISize.m_y * 0.4f},
				TextPivot::CenterBottom,
				FontId::kQuestUIText,
				"",
				0x000000,
				0xffffff,
				255
		});
	UIFactory::MakeUIToPanel<UIText>(m_questUIPanel, TextArgs{
				kQuestUIPos - Vector{0,kQuestUISize.m_y * 0.4f},
				TextPivot::CenterTop,
				FontId::kFactoryUIText,
				"<クエスト名>",
				0xffffff,
				0x000000,
				255
		});

}

void PlayerUI::InitItemBarItemSlot()
{
	for (int i = 0; i < kItemBarItemCount; i++)
	{
		const auto& itemBox = std::make_shared<UIItemBox>(m_itemBarUIPanel, CalcItemBarItemSlotPos(i), kItemBarItemSlotSize);
		int index = i;

		itemBox->SetOnClickEvent(
			[this, index]
			{
				m_playerItem->SetItemBarChoiceIndex(index);
			}
		);
		itemBox->SetOnDragBeginEvent(
			[this, index]()
			{
				m_uiManager.ItemSelect(m_playerItem->GetItemBar().lock(), index);
			}
		);
		itemBox->SetOnDropEvent(
			[this, index]
			{
				m_uiManager.MoveItem(m_playerItem->GetItemBar(),index);
			}
		);
		
		//配列に追加
		m_itemBarBoxes.push_back(itemBox);
	}
}

void PlayerUI::InitInventoryItemPanel()
{
	//アイテムバーの背景部分
	UIFactory::MakeUIToPanel<UISquare>(m_inventoryUIPanel, kInventoryPos, kInventorySize, kInventoryColor, kInventoryAlpha);

	const auto& inventory = m_playerItem->GetInventory();
	for (int i = 0; i < inventory.lock()->GetSlotCount(); i++)
	{
		const auto& itemBox = std::make_shared<UIItemBox>(
			m_inventoryUIPanel, CalcInventoryItemSlotPos(i), kItemBarItemSlotSize
		);

		int index = i;

		itemBox->SetOnClickEvent(
			[this, index]
			{
				m_playerItem->SetInventoryChoiceIndex(index);
			}
		);
		itemBox->SetOnDragBeginEvent(
			[this, inventory, index]()
			{
				m_uiManager.ItemSelect(inventory, index);
			}
		);
		itemBox->SetOnDropEvent(
			[this, inventory, index]
			{
				m_uiManager.MoveItem(inventory, index);
			}
		);

		//配列に追加
		m_inventoryBoxes.push_back(itemBox);
	}

	const auto basepos = kInventoryPos + Vector{ kInventorySize.m_x * 0.5f,-kInventorySize.m_y * 0.5f };
	const auto size = Vector{ 30,30 };

	const auto pos = basepos + Vector{ -size.m_x * 0.5f,size.m_y * 0.5f } + Vector{-10,10};

	const auto& close = UIFactory::MakeUIToPanel<UIImage>(m_inventoryUIPanel, pos, size, GraphicId::kUIClose);
	close.lock()->SubscribeOnClick([this]()
		{
			m_playerController->CloseInventoryUI();
		}
	);

	m_inventoryUIPanel->AddChild(m_playerCraft.lock()->GetOrBuidUIPanel());
}

Vector PlayerUI::CalcItemBarItemSlotPos(int index) const
{
	//アイテムバーの左端の座標 (yは中心)
	const Vector kItemBarLeftEdge = kItemBarPos - Vector{ kItemBarSize.m_x * 0.5f };
	//アイテムバーのアイテムスロットの1つ目の左端の座標((kItemBarLeftEdge) + (スロット間のオフセット)の値)
	const Vector kItemSlotLeftEdgeStartPos = kItemBarLeftEdge + Vector{ kItemBarSlotOffsetX };
	//i個目のアイテムバーのアイテムスロットの左端の座標
	const Vector kItemSlotLeftEdgePos = kItemSlotLeftEdgeStartPos + Vector{ (kItemBarItemSlotSize.m_x + kItemBarSlotOffsetX) * index };
	//i個目のアイテムバーのアイテムスロットの中心座標
	const Vector kItemSlotCenterPos = kItemSlotLeftEdgePos + Vector{ kItemBarItemSlotSize.m_x * 0.5f };

	return kItemSlotCenterPos;
}

Vector PlayerUI::CalcInventoryItemSlotPos(int index) const
{
	int width = index % 10;
	int height = index / 10;

	//アイテムバーの左端の座標 (yは中心)
	const Vector kItemBarLeftEdge = kInventoryPos - Vector{ kInventorySize.m_x * 0.5f , kInventorySize.m_y * 0.5f };
	//アイテムバーのアイテムスロットの1つ目の左端の座標((kItemBarLeftEdge) + (スロット間のオフセット)の値)
	const Vector kItemSlotLeftEdgeStartPos = kItemBarLeftEdge + Vector{ kInventorySlotOffsetX,kInventorySlotOffsetY };
	//i個目のアイテムバーのアイテムスロットの左端の座標
	const Vector kItemSlotLeftEdgePos = kItemSlotLeftEdgeStartPos + 
		Vector{ 
		(kItemBarItemSlotSize.m_x + kInventorySlotOffsetX) * width,
		(kItemBarItemSlotSize.m_y + kInventorySlotOffsetY) * height 
	};
	//i個目のアイテムバーのアイテムスロットの中心座標
	const Vector kItemSlotCenterPos = kItemSlotLeftEdgePos + Vector{ kItemBarItemSlotSize.m_x * 0.5f, kItemBarItemSlotSize.m_y * 0.5f };

	return kItemSlotCenterPos;
}

void PlayerUI::SetChooseFactoryComponent(DeviceType currentDevice)
{
	if (!IsEnableSetChooseFactoryComponent()) return;
	if (m_uiManager.IsPointerHoverUI()) return;

	//コントローラーのとき
	if (currentDevice == DeviceType::kGamepad)
		//画面中心のオブジェクトを取得
		m_chooseFactory = m_playerController->GetScreenCenterFactoryObject();
	//キーボードのとき
	else if (currentDevice == DeviceType::kKeyboard)
		//マウスカーソルの位置のオブジェクトを取得
		m_chooseFactory = m_playerController->GetMousePointFactoryObject();
}

void PlayerUI::UpdateUIPanel()
{
	UpdateDestroyUIPanel();

	UpdateFactoryUIKeyPrompt();

	UpdateFactoryUIPanel();

	UpdateItemBar();

	UpdateQuestUI();

	UpdateInventory();
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

void PlayerUI::UpdateItemBar()
{
	if (!m_playerItem) return;

	int index = m_playerItem->GetItemBarChoiceIndex();

	//選択時の色変更
	for (int i = 0; i < kItemBarItemCount; i++)
	{
		if (i == index)
			m_itemBarBoxes.at(i)->SetColor(0xffff00);
		else
			m_itemBarBoxes.at(i)->SetDefaultColor();
	}

	//アイテムがあるときにそのアイコンを表示する
	for (int i = 0; i < kItemBarItemCount; i++)
	{
		const auto& itemStack = m_playerItem->GetItemFromItemBar(i);

		if (!itemStack)
			m_itemBarBoxes.at(i)->SetGraphicID(GraphicId::kNone);
		else
			m_itemBarBoxes.at(i)->SetGraphicID(itemStack->GetItemIconGraphicID());
	}

	//アイテムがあるときにその数を表示する
	for (int i = 0; i < kItemBarItemCount; i++)
	{
		const auto& itemStack = m_playerItem->GetItemFromItemBar(i);

		if (!itemStack)
			m_itemBarBoxes.at(i)->SetText("");
		else
			m_itemBarBoxes.at(i)->SetText("x{}", itemStack->GetItemCount());
	}

}

void PlayerUI::UpdateInventory()
{
	if (!m_playerItem) return;

	int index = m_playerItem->GetInventoryChoiceIndex();

	int inventorySize = m_playerItem->GetInventory().lock()->GetSlotCount();

	//アイテムがあるときにそのアイコンを表示する
	for (int i = 0; i < inventorySize; i++)
	{
		const auto& itemStack = m_playerItem->GetItemFromInventory(i);

		if (!itemStack)
			m_inventoryBoxes.at(i)->SetGraphicID(GraphicId::kNone);
		else
			m_inventoryBoxes.at(i)->SetGraphicID(itemStack->GetItemIconGraphicID());
	}

	//アイテムがあるときにその数を表示する
	for (int i = 0; i < inventorySize; i++)
	{
		const auto& itemStack = m_playerItem->GetItemFromInventory(i);

		if (!itemStack)
			m_inventoryBoxes.at(i)->SetText("");
		else
			m_inventoryBoxes.at(i)->SetText("x{}", itemStack->GetItemCount());
	}


}

void PlayerUI::UpdateQuestUI()
{
	const auto& safeQuestUI = m_questText.lock();
	if (!safeQuestUI) return;

	const Base_Quest* quest = m_questManager.GetCurrentQuest();
	safeQuestUI->SetText(quest->GetDescription());

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

void PlayerUI::IsEnableCameraControll(bool enable) const
{
	Camera::GetInstance().SetIsEnableMovement(enable);
	Camera::GetInstance().SetIsEnableRotation(enable);
}

bool PlayerUI::IsCurrentState(CharactorState state) const
{
	return m_state->CheckCurrentState(state);
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
			m_state->ChangeState(CharactorState::IDLE);
			return;
		};

		IsEnableCameraControll(false);

		break;

	case CharactorState::OPEN_INVENTORYUI:

		IsEnableCameraControll(false);

		break;
	case CharactorState::IDLE:

		IsEnableCameraControll(true);

		break;

	default:
		break;
	}
}

