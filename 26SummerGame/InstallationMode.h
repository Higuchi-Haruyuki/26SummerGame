#pragma once
#include "Component.h"
#include <cassert>

class ItemStack;
class PlayerItem;
class CharactorStateManager;
class PlayerInput;
class SceneManager;
class RayCastResult;
class PlayerUI;
class UIManager;

using Radian = float;

    /// <summary>
    /// 設置モードでの処理を行う
    /// </summary>
    class InstallationMode :
        public Component
    {
    public:
		InstallationMode(std::weak_ptr<Object> parentObject);
		~InstallationMode() override = default;
		void Init() override;
		void Update() override;

	private:

		/// <summary>
		/// コンポーネントへの参照を取得する。
		/// </summary>
		void GetComponentReference();

		/// <summary>
		/// PlayerItemコンポーネントから選択中のアイテムへの弱参照を取得する。
		/// </summary>
		ItemStack* GetSelectedItemReference();

		/// <summary>
		/// PlayerItemコンポーネントから選択中のアイテムを取得し、m_selectedItemにセットする。
		/// </summary>
		void SetSelectedItem();

		/// <summary>
		/// プレビューを表示する。選択中のアイテムが存在しない場合は何もしない。
		/// </summary>
		/// <param name="previewPos">プレビューを表示する位置</param>
		/// <returns>プレビューオブジェクトの弱参照。プレビューオブジェクトが存在しない場合は空の弱参照を返す。</returns>
		std::weak_ptr<Object> ShowPreview() const;

		/// <summary>
		/// プレビューオブジェクトの位置を合わせる。プレビューオブジェクトが存在しない場合は何もしない。
		/// </summary>
		void SetPositionPreviewObject(const Vector& position);

		/// <summary>
		/// プレビューオブジェクトを回転させる。プレビューオブジェクトが存在しない場合は何もしない。
		/// </summary>
		/// <param name="rotationValue"></param>
		void RotatePreviewObject(Radian rotationValue);

		/// <summary>
		/// プレビューされているアイテムを指定座標に設置する。
		/// </summary>
		/// <param name="position"></param>
		void Installation(const Vector& position);

		void ResetItemAndPreviewObject();

		void ResetItem();

		void ResetPreviewObject();

		/// <summary>
		/// 入力処理
		/// </summary>
		/// <param name="installationPos">設置する場所</param>
		void InputAction(const Vector& installationPos);
		
		/// <summary>
		/// 現在、設置モードである
		/// </summary>
		/// <returns></returns>
		bool IsInstallationState() const;

		/// <summary>
		/// マウスの位置からレイキャストを行う。
		/// </summary>
		RayCastResult RayCastFromMousePoint() const;

	private:
		
		//シングルトンへの参照
		PlayerInput& m_input;
		SceneManager& m_sceneManager;
		UIManager& m_uiManager;

		//プレイヤーのアイテムコンポーネントへの弱参照
		std::weak_ptr<PlayerItem> m_playerItem;

		//プレイヤーのステートを管理するコンポーネントへの弱参照
		std::weak_ptr<CharactorStateManager> m_state;

		//UIを表示するコンポーネントへの弱参照
		std::weak_ptr<PlayerUI> m_playerUI;
		
		//選択中のアイテムへのポインタ
		ItemStack* m_selectedItem = nullptr;

		//プレビューオブジェクトへの弱参照
		std::weak_ptr<Object> m_previewObject;

		Radian m_previewRotationAngle = 0.0f;

    };

