#include "Animation3D.h"
#include "Camera.h"
#include "CharactorStateManager.h"
#include "Collider.h"
#include "Component.h"
#include "Debug.h"
#include "FactoryComponent.h"
#include "HitPoint.h"
#include "Object.h"
#include "PlayerCraft.h"
#include "PlayerController.h"
#include "PlayerInput.h"
#include "PlayerUI.h"
#include "vector.h"
#include "PlayerItem.h"
#include "RayCastResult.h"
#include "ResourceGenerator.h"
#include "Game.h"
#include "MapManager.h"
#include <cmath>
#include <DxLib.h>
#include <format>
#include <map>
#include <memory>
#include <vector>
namespace
{

}

PlayerController::PlayerController(std::shared_ptr<Object> parentObject)
	: Component(parentObject), m_playerInput(PlayerInput::GetInstance())
{}

PlayerController::~PlayerController()
{

}

void PlayerController::Init()
{
	Component::Init();

	m_state = GetParentObject()->GetComponent<CharactorStateManager>();

	ChangeState(CharactorState::IDLE);

	m_playerItem = GetParentObject()->AddComponent<PlayerItem>();

	const auto& playerCraft = GetParentObject()->AddComponent<PlayerCraft>();

	playerCraft->AddCraftConsumeSlot(m_playerItem.lock()->GetItemBar());
	playerCraft->AddCraftConsumeSlot(m_playerItem.lock()->GetInventory());

	m_playerUI = GetParentObject()->AddComponent<PlayerUI>();
}

void PlayerController::Update(float deltaTime)
{
	Component::Update(deltaTime);
	if (!m_isEnable) return;

	InputAction();

	//プレイヤーの状態をデバックログで描画
	Debug::Log(std::format("PlayerState: {}", m_state.lock()->CurrentStateToString()));

	RayCastResult raycast;
	raycast.RaycastFromMousePoint();

}

std::weak_ptr<FactoryComponent>  PlayerController::GetScreenCenterFactoryObject()
{
	RayCastResult rayCastResult;
	rayCastResult.RaycastFromScreenCenter();
	const auto& rayCastCollider = rayCastResult.GetHitCollider().lock();
	if (!rayCastCollider) return std::weak_ptr<FactoryComponent>();
	return rayCastCollider->GetParentObject()->GetComponent<FactoryComponent>();
}


std::weak_ptr<FactoryComponent>  PlayerController::GetMousePointFactoryObject()
{
	RayCastResult rayCastResult;
	rayCastResult.RaycastFromMousePoint();
	const auto& rayCastCollider = rayCastResult.GetHitCollider().lock();
	if (!rayCastCollider) return std::weak_ptr<FactoryComponent>();
	return rayCastCollider->GetParentObject()->GetComponent<FactoryComponent>();
}

void PlayerController::SetChoiceIndex()
{
	static std::array<std::string, 10> array = 
	{
		"ItemBar0",
		"ItemBar1",
		"ItemBar2",
		"ItemBar3",
		"ItemBar4",
		"ItemBar5",
		"ItemBar6",
		"ItemBar7",
		"ItemBar8",
		"ItemBar9",
	};

	for (int i = 0; i < array.size(); i++)
	{
		if (m_playerInput.GetAction(array.at(i))->GetPhase() == ButtonPhase::kTrigger)
		{
			m_playerItem.lock()->SetItemBarChoiceIndex(i);
		}
	}


}


void PlayerController::InputAction()
{

	if (m_playerInput.GetAction("Decide")->GetPhase() == ButtonPhase::kTrigger)
	{
		OpenFactoryComponentUI();

	}

	if (m_playerInput.GetAction("OpenInventory")->GetPhase() == ButtonPhase::kTrigger)
	{
		if(!IsOpenInventoryUIState()) OpenInventoryUI();

		//開いているときにもう一回押せば閉じる
		else CloseInventoryUI();
	}

	if (m_playerInput.GetAction("InstallationMode")->GetPhase() == ButtonPhase::kTrigger)
	{
		if (!IsInstallationState())	EnterInstallationMode();
		else ExitInstallationMode();

	}

	if (m_playerInput.GetAction("DestroyMode")->GetPhase() == ButtonPhase::kTrigger)
	{
		if (!IsDestroyState()) EnterDestroyMode();
		else ExitDestroyMode();
	}

	if (m_playerInput.GetAction("Cancel")->GetPhase() == ButtonPhase::kTrigger)
	{
		CloseFactoryComponentUI();
		CloseInventoryUI();
		ExitInstallationMode();
		ExitDestroyMode();
	}

	SetChoiceIndex();
}

void PlayerController::OpenFactoryComponentUI()
{

	if (!IsIdleState()) return;
	if (IsOpenFactoryUIState()) return;

	ChangeState(CharactorState::OPEN_FACTORYUI);
}

void PlayerController::CloseFactoryComponentUI()
{
	if (!IsOpenFactoryUIState()) return;

	ChangeState(CharactorState::IDLE);
}

void PlayerController::OpenInventoryUI()
{
	if (!IsIdleState()) return;

	//既に開いているとき処理しない
	if (IsOpenInventoryUIState()) return;

	ChangeState(CharactorState::OPEN_INVENTORYUI);
}

void PlayerController::CloseInventoryUI()
{
	if (!IsOpenInventoryUIState()) return;
	//すでにUIを開いていていないとき処理しない

	ChangeState(CharactorState::IDLE);
}

void PlayerController::EnterInstallationMode()
{
	if (IsOpenFactoryUIState() || IsOpenInventoryUIState()) return;

	ChangeState(CharactorState::INSTALLATION);

}

void PlayerController::ExitInstallationMode()
{
	if (!IsInstallationState()) return;

	ChangeState(CharactorState::IDLE);
}

void PlayerController::EnterDestroyMode()
{
	if (IsOpenFactoryUIState() || IsOpenInventoryUIState()) return;

	ChangeState(CharactorState::DESTROY);
}

void PlayerController::ExitDestroyMode()
{
	if (!IsDestroyState()) return;

	ChangeState(CharactorState::IDLE);
}

void PlayerController::ChangeState(const CharactorState& newState)
{
	const auto& safeState = m_state.lock();
	if (!safeState) return;
	safeState->ChangeState(newState);
}

bool PlayerController::IsInstallationState() const
{
	const auto& safeState = m_state.lock();
	if (!safeState) return false;
	return safeState->CheckCurrentState(CharactorState::INSTALLATION);
}

bool PlayerController::IsDestroyState() const
{
	const auto& safeState = m_state.lock();
	if (!safeState) return false;
	return safeState->CheckCurrentState(CharactorState::DESTROY);
}

bool PlayerController::IsOpenFactoryUIState() const
{
	const auto& safeState = m_state.lock();
	if (!safeState) return false;
	return safeState->CheckCurrentState(CharactorState::OPEN_FACTORYUI);
}

bool PlayerController::IsOpenInventoryUIState() const
{
	const auto& safeState = m_state.lock();
	if (!safeState) return false;
	return safeState->CheckCurrentState(CharactorState::OPEN_INVENTORYUI);
}

bool PlayerController::IsIdleState() const
{
	const auto& safeState = m_state.lock();
	if (!safeState) return false;
	return safeState->CheckCurrentState(CharactorState::IDLE);
}

bool PlayerController::IsCurrentState(const CharactorState& state) const
{
	const auto& safeState = m_state.lock();
	if (!safeState) return false;
	return safeState->CheckCurrentState(state);
}
