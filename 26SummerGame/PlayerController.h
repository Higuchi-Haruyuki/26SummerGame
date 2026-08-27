#pragma once
#include "Component.h"

#include <memory>

class PlayerInput;
class PlayerItem;
class Object;
class Colllder;
class HitPoint;
class Collider;
class Model;
class Animation3D;
class CharactorStateManager;
class PlayerUI;

enum class CharactorState;

class PlayerController : public Component
{
public:
	PlayerController(std::weak_ptr<Object> parentObject);
	virtual ~PlayerController() override;

	void Init() override;
	void Update() override;

	/// <summary>
	/// 画面の中心にいるFactoryComponentを持つオブジェクトを返す。
	/// 存在しない場合はnullptrを返す。
	/// </summary>
	std::weak_ptr<FactoryComponent> GetScreenCenterFactoryObject();

	/// <summary>
	/// マウスカーソルにいるFactoryComponentを持つオブジェクトを返す。
	/// 存在しない場合はnullptrを返す。
	/// </summary>
	std::weak_ptr<FactoryComponent> GetMousePointFactoryObject();

	void SetChoiceIndex();

	void EnterInstallationMode();

	void ExitInstallationMode();

	void EnterDestroyMode();

	void ExitDestroyMode();

	void CloseFactoryComponentUI();
	
	void CloseInventoryUI();

private:
	/// <summary>
	/// 入力を受け取り、処理を割り振る。
	/// </summary>
	void InputAction();

	void OpenFactoryComponentUI();

	
	void OpenInventoryUI();





	void ChangeState(const CharactorState& newState);

	/*ステートの確認*/
	bool IsInstallationState() const;
	bool IsDestroyState() const;
	bool IsOpenFactoryUIState() const;
	bool IsOpenInventoryUIState() const;
	bool IsIdleState() const;

	bool IsCurrentState(const CharactorState& state) const;

private:
	//
	PlayerInput& m_playerInput;

	//コンポーネントのポインタ
	std::weak_ptr<CharactorStateManager> m_state;
	std::weak_ptr<PlayerUI> m_playerUI;
	std::weak_ptr<PlayerItem> m_playerItem;
};
