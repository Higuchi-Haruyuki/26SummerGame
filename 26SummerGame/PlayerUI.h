#pragma once
#include "Component.h"
#include <memory>
#include <vector>
#include <functional>
#include <string>
#include "Object.h"
#include "Base_UIElement.h"
#include "UIPanel.h"
#include <map>
#include <concepts>
#include <type_traits>

class FactoryComponent;
class UIManager;
class Collider;
class PlayerInput;
enum class CharactorState;
enum class DeviceType;
class ResourceManager;
class PlayerController;
class CharactorStateManager;
class Vector;
class UIKeyPrompt;
class UIPanel;
class PlayerItem;
class UISquare;
class UIImage;
class UIText;
class UIButton;
class QuestManager;
class UIItemBox;
class PlayerCraft;

/// <summary>
/// UI表示を行う。
/// </summary>
class PlayerUI :
    public Component
{
public:
    PlayerUI(std::weak_ptr<Object> parentObject);
	virtual ~PlayerUI() override {};

	void Init() override;
	void Update() override;

private:

	/// <summary>
	/// コンポーネントへの参照を取得する。
	/// </summary>
	void GetComponentReference();

	/// <summary>
	/// パネルを生成し、メンバ変数に格納する。
	/// </summary>
	void CreatePanel();

	/// <summary>
	/// すべてのパネルの初期設定をする
	/// </summary>
	void InitUIPanel();

	/// <summary>
	/// Idleパネルの初期設定をする
	/// </summary>
	void InitIdleUIPanel();

	/// <summary>
	/// Installationパネルの初期設定をする
	/// </summary>
	void InitInstallationUIPanel();

	/// <summary>
	/// Destroyパネルの初期設定をする
	/// </summary>
	void InitDestroyUIPanel();

	/// <summary>
	/// FactoryUI(工業オブジェクトをクリック時に出るUI)の初期設定をする。
	/// </summary>
	void InitFactoryUIPanel();

	/// <summary>
	/// ItemBarパネルの初期設定をする
	/// </summary>
	void InitItemBarUIPanel();

	/// <summary>
	/// QuestUIパネルの初期設定をする
	/// </summary>
	void InitQuestUIPanel();

	/// <summary>
	/// アイテムバーUIのアイテムスロットの初期化処理
	/// </summary>
	void InitItemBarItemSlot();

	/// <summary>
	/// インベントリUIのアイテムスロットの初期化処理
	/// </summary>
	void InitInventoryItemPanel();

	/// <summary>
	/// 指定番目のアイテムバーのアイテムスロットの位置を計算する。
	/// </summary>
	/// <param name="index">index番目</param>
	/// <returns>位置</returns>
	Vector CalcItemBarItemSlotPos(int index) const;

	Vector CalcInventoryItemSlotPos(int index) const;

	/// <summary>
	/// 現在有効なデバイスタイプに応じてm_chooseFacComを設定する。
	/// </summary>
	/// <param name="currentDevice"></param>
	void SetChooseFactoryComponent(DeviceType currentDevice);

	/// <summary>
	/// すべてのパネルの更新処理をする
	/// </summary>
	void UpdateUIPanel();

	/// <summary>
	/// Destroyパネルの更新処理をする
	/// </summary>
	void UpdateDestroyUIPanel();

	/// <summary>
	/// FactoryComponentの操作キーの表示UIの更新処理
	/// </summary>
	void UpdateFactoryUIKeyPrompt();

	/// <summary>
	/// FactoryUI(工業オブジェクトをクリック時に出るUI)の更新処理をする。
	/// </summary>
	void UpdateFactoryUIPanel();

	/// <summary>
	/// アイテムバーUIの更新処理
	/// </summary>
	void UpdateItemBar();

	/// <summary>
	/// インベントリUIの更新処理
	/// </summary>
	void UpdateInventory();

	/// <summary>
	/// クエストUIの更新処理
	/// </summary>
	void UpdateQuestUI();

	/// <summary>
	/// 指定したステートのUIのみを見えるようにする。
	/// </summary>
	/// <param name="state"></param>
	void ApplyUIVisibility(const CharactorState& state);

	/// <summary>
	/// すべてのステートのUIの可視状態を設定する。
	/// </summary>
	/// <param name="visible"></param>
	void SetAllUIVisible(bool visible);

	/// <summary>
	/// 指定したステートのUIの可視状態を設定する。
	/// </summary>
	/// <param name="state"></param>
	/// <param name="visible"></param>
	void SetUIVisible(const CharactorState& state, bool visible);
	
	/// <summary>
	/// カメラ操作の有効状態を設定する。
	/// </summary>
	/// <param name="enable"></param>
	void IsEnableCameraControll(bool enable) const;

	/// <summary>
	/// 現在のステートならtrueを返し、そうでないならfalseを返す。
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	bool IsCurrentState(CharactorState state) const;

	/// <summary>
	/// 現在、m_chooseFacComを設定可能かを返す。
	/// </summary>
	/// <returns></returns>
	bool IsEnableSetChooseFactoryComponent()const;

	void ChangeCharactorState(const CharactorState& lastState, const CharactorState& nowState);

private:
	//インスタンスへの参照
	PlayerInput& m_input;
	UIManager& m_uiManager;
	ResourceManager& m_resourceManager;
	QuestManager& m_questManager;

	//コンポーネントのポインタ
	std::weak_ptr<Collider> m_collider;
	std::weak_ptr<PlayerController> m_playerController;
	std::weak_ptr<CharactorStateManager> m_state;
	std::weak_ptr<PlayerItem> m_playerItem;
	std::weak_ptr<PlayerCraft> m_playerCraft;

	//現在選んでいるUIのFactoryComponent
	std::weak_ptr<FactoryComponent> m_chooseFactory;
	//一つ前のフレームで選択されていたFactoryComponent
	std::weak_ptr<FactoryComponent> m_lastOpenedFactory;

	//キー表示
	std::weak_ptr<UIKeyPrompt> m_factoryUIPrompt;


	/*パネルの変数*/

	//通常状態のUIの基本のパネル
	std::shared_ptr<UIPanel> m_idleKeyPanel;
	//設置モードのUIの基本のパネル
	std::shared_ptr<UIPanel> m_installationKeyPanel;
	//破壊モードのUIの基本のパネル
	std::shared_ptr<UIPanel> m_destroyKeyPanel;
	//工業オブジェクトをクリック時にでるUIの基本のパネル
	std::shared_ptr<UIPanel> m_factoryUIPanel;
	//アイテムバーのUIのパネル
	std::shared_ptr<UIPanel> m_itemBarUIPanel;
	//クエストのUIパネル
	std::shared_ptr<UIPanel> m_questUIPanel;
	//インベントリのUIのパネル
	std::shared_ptr<UIPanel> m_inventoryUIPanel;

	/*パネル内のUI要素の変数*/
	std::weak_ptr<UIKeyPrompt> m_destroyUIKeyPrompt;
	
	std::weak_ptr<UIText> m_factoryUITitleText;
	
	std::weak_ptr<UIText> m_questText;
	
	//工業オブジェクト特有のUIパネルのポインタ
	std::weak_ptr<UIPanel> m_originalFactoryUIPanel;

	std::vector<std::shared_ptr<UIItemBox>> m_itemBarBoxes;

	std::vector<std::shared_ptr<UIItemBox>> m_inventoryBoxes;

	std::map<CharactorState, std::vector<std::shared_ptr<UIPanel>>> m_activePanelByState;
};





