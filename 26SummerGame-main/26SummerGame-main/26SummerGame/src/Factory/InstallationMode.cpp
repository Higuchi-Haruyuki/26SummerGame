#include "CharactorStateManager.h"
#include "Collider.h"
#include "InstallationMode.h"
#include "ItemStack.h"
#include "UIManager.h"
#include "PlayerUI.h"
#include "PlayerInput.h"
#include "PlayerItem.h"
#include "RayCastResult.h"
#include "Scene.h"
#include "SceneManager.h"
#include "vector.h"
#include <algorithm>

namespace
{
	constexpr Radian kMinRad = 0.0f;
	constexpr Radian kMaxRad = 360.0f;
}

	InstallationMode::InstallationMode(std::weak_ptr<Object> parentObject):
		Component(parentObject),
		m_input(PlayerInput::GetInstance()),
		m_sceneManager(SceneManager::GetInstance()),
		m_uiManager(UIManager::GetInstance())
	{}

	void InstallationMode::Init()
	{
		Component::Init();

		//コンポーネントへの参照を取得する。
		GetComponentReference();

	}

	void InstallationMode::Update()
	{
		//コンポーネントへの参照を取得する。
		GetComponentReference();

		//マウスカーソル位置からレイを飛ばし、レイの衝突位置を取得する。
		const Vector hitPos = RayCastFromMousePoint().GetHitPoint();

		//PlayerItemコンポーネントから選択中のアイテムを取得する。
		SetSelectedItem();

		//プレビュー中のオブジェクトの位置を設定する。
		SetPositionPreviewObject(hitPos);

		//入力処理
		InputAction(hitPos);

		Debug::Log(std::format("回転角度Rad: {}", m_previewRotationAngle));

		//設置モードでないときアイテムやプレビューオブジェクトの変数をリセットする。
		if (!IsInstallationState()) ResetItemAndPreviewObject();
	}

	void InstallationMode::GetComponentReference()
	{
		if (!m_playerItem.lock()) m_playerItem = GetComponent<PlayerItem>();

		if (!m_state.lock()) m_state = GetComponent<CharactorStateManager>();
	
		if (!m_playerUI.lock()) m_playerUI = GetComponent<PlayerUI>();
	}

	std::weak_ptr<ItemStack> InstallationMode::GetSelectedItemReference()
	{
		const auto& safePlayerItem = m_playerItem.lock();
		if (!safePlayerItem) return {};
		return safePlayerItem->GetSelectedItemBar();
	}

	void InstallationMode::SetSelectedItem()
	{
		const auto newSelectedItem = GetSelectedItemReference();
		const auto newSelectedItemShared = newSelectedItem.lock();
		const auto currentItemShared = m_selectedItem.lock();

		//アイテムの選択状態が変わっていないとき
		if (newSelectedItemShared == currentItemShared &&
			(newSelectedItemShared || m_previewObject.expired())) return;
		
		//今選択されているアイテムの選択終了処理を行う
		if (!m_selectedItem.expired() || !m_previewObject.expired()) ResetItemAndPreviewObject();

		//選択されているアイテムを保存するメンバ変数に保存
		m_selectedItem = newSelectedItem;

		//プレビューオブジェクトを保存
		m_previewObject = ShowPreview();

		RotatePreviewObject(0.0f);
	}

	std::weak_ptr<Object> InstallationMode::ShowPreview() const
	{
		//設置モードではないとき
		if (!IsInstallationState()) return std::weak_ptr<Object>();

		const auto selectedItem = m_selectedItem.lock();
		if (!selectedItem) return std::weak_ptr<Object>();

		return selectedItem->GeneratePreviewObject();
	}

	void InstallationMode::SetPositionPreviewObject(const Vector& position)
	{
		const auto selectedItem = m_selectedItem.lock();
		if (!selectedItem) return;

		selectedItem->SetPositionPreviewObject(position);
	}

	void InstallationMode::RotatePreviewObject(Radian rotationValue)
	{
		m_previewRotationAngle += rotationValue;
		const auto selectedItem = m_selectedItem.lock();
		if (!selectedItem) return;
			 
		selectedItem->RotationPreviewObject(m_previewRotationAngle);
	}

	void InstallationMode::Installation(const Vector& position)
	{
		//UIがクリックされたときはそのクリックでは設置しない
		if (m_uiManager.IsPointerHoverUI()) return;

		const auto selectedItem = m_selectedItem.lock();
		if (!selectedItem) return;

		//そのアイテムを持っていないとき
		if (!selectedItem->GetItemCount()) return;

		//指定座標をグリッド座標に変換
		auto gridPos = Game::WorldPosToGridPos(position);

		//シーンの全オブジェクトを取得
		const auto& sceneObjects = m_sceneManager.GetCurrentScene()->GetSceneObjects();

		/*すでにオブジェクトがあるかチェックする処理*/
		for (const auto& obj : sceneObjects)
		{
			//置きたい場所にすでにオブジェクトがあるときは処理を終了する。
			if (obj->GetGridPosition() == gridPos) {

				//もしそのオブジェクトが自身のときは判定のループを続行する。
				if (m_previewObject.lock() == obj) continue;

				return;
			}
		}

		//アイテムの設置する関数を呼び出す。
		const auto& obj = selectedItem->Installation(gridPos, m_previewRotationAngle);

		if (!obj.lock()) return;

		//設置に成功していたらアイテム数を1つ減らす。
		selectedItem->MinusItemCount(1);

		//現在の回転角度を保存
		Radian angleTemp = m_previewRotationAngle;

		//リセット
		ResetItemAndPreviewObject();

		//保存した回転角度を再代入
		m_previewRotationAngle = angleTemp;
	}

	void InstallationMode::ResetItemAndPreviewObject()
	{
		ResetItem();
		ResetPreviewObject();
	}

	void InstallationMode::ResetItem()
	{
		if (const auto selectedItem = m_selectedItem.lock()) {
			selectedItem->RemovePreviewObject();
		}
		else if (const auto previewObject = m_previewObject.lock()) {
			previewObject->Destroy();
		}

		m_selectedItem.reset();
	}

	void InstallationMode::ResetPreviewObject()
	{
		m_previewRotationAngle = 0.0f;
		m_previewObject.reset();
	}

	void InstallationMode::InputAction(const Vector& installationPos)
	{
		if (!IsInstallationState()) return;

		if (m_input.GetAction("PreviewTurnRight")->GetPhase() == ButtonPhase::kTrigger)
		{
			RotatePreviewObject(-DX_PI_F * 0.5f);
		}
		if (m_input.GetAction("Decide")->GetPhase() == ButtonPhase::kPressed)
		{
			Installation(installationPos);
		}
	}

	bool InstallationMode::IsInstallationState() const
	{
		const auto& safeState = m_state.lock();
		if (!safeState) return false;
		return safeState->CheckCurrentState(CharactorState::INSTALLATION);
	}

	RayCastResult InstallationMode::RayCastFromMousePoint() const
	{
		RayCastResult result;
		result.RaycastFromMousePoint();
		return result;
	}


