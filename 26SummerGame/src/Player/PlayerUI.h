#pragma once
#include "Component.h"
#include <memory>
#include <vector>
#include <map>
#include "ItemType.h"

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
class UIText;
class UIProgressBar;

/// <summary>
/// 主に状態に応じて見せるUIPanelを変更する
/// </summary>
class PlayerUI :
    public Component
{
public:
    PlayerUI(std::weak_ptr<Object> parentObject);
	virtual ~PlayerUI() override {};

	void Init() override;
	void Update() override;
	/*外部から呼び出して更新してもらう*/

	void UpdateMiningProgressBar(float progress);
	void SetVisibleMiningProgressBar(bool visible);

	void UpdateResourceUIOnMouseCursor(Item item, const Vector& cursorPos);
	void SetVisibleResourceUIOnMouseCursor(bool visible);

private:

	/// <summary>
	/// コンポーネントへの参照を取得する。
	/// </summary>
	void GetComponentReference();

	/// <summary>
	/// パネルを生成し、メンバ変数に格納する。
	/// </summary>
	void GetPanel();

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

	//コンポーネントのポインタ
	std::weak_ptr<Collider> m_collider;
	std::weak_ptr<PlayerController> m_playerController;
	std::weak_ptr<CharactorStateManager> m_state;

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

	/*パネル内のUI要素の変数*/
	std::weak_ptr<UIKeyPrompt> m_destroyUIKeyPrompt;
	
	std::weak_ptr<UIText> m_factoryUITitleText;
	
	//工業オブジェクト特有のUIパネルのポインタ
	std::weak_ptr<UIPanel> m_originalFactoryUIPanel;

	std::map<CharactorState, std::vector<std::shared_ptr<UIPanel>>> m_activePanelByState;

	//資源採掘ゲージ
	std::weak_ptr<UIProgressBar> m_miningProgressBar;

	//マウスカーソルの近くに表示される資源アイテムのUI
	std::weak_ptr<UIKeyPrompt> m_resourceMiningUI;

};





