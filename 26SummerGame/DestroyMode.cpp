#include "CharactorStateManager.h"
#include "Collider.h"
#include "DestroyMode.h"
#include "FactoryComponent.h"
#include "Game.h"
#include "ItemStack.h"
#include "Object.h"
#include "PlayerInput.h"
#include "PlayerItem.h"
#include "RayCastResult.h"
#include "Scene.h"
#include "SceneManager.h"
#include "vector.h"
#include <algorithm>
#include "ItemSlot.h"

namespace component
{
	DestroyMode::DestroyMode(std::shared_ptr<Object> parentObject):
		Component(parentObject),
		m_input(PlayerInput::GetInstance()),
		m_sceneManager(SceneManager::GetInstance())
	{}

	void DestroyMode::Init()
	{
		Component::Init();

		//コンポーネントへの参照を取得する。
		GetComponentReference();

	}

	void DestroyMode::Update(float deltaTime)
	{
		//コンポーネントへの参照を取得する。
		GetComponentReference();

		if (!IsDestoryState()) return;

		//マウスカーソル位置からレイを飛ばし、レイの衝突位置を取得する。
		const auto& hitCol = RayCastFromMousePoint().GetHitCollider();

		//破壊するオブジェクトの表示をする
		PreviewDestoryObject(hitCol);

		//入力処理
		InputAction(hitCol);
	
	}

	void DestroyMode::GetComponentReference()
	{
		if (!m_playerItem.lock()) m_playerItem = GetParentObject()->GetComponent<PlayerItem>();

		if (!m_state.lock()) m_state = GetParentObject()->GetComponent<CharactorStateManager>();
	
	}

	void DestroyMode::PreviewDestoryObject(const std::weak_ptr<Collider>& destoryCol)
	{
		//FactoryComponentを持っていないとき
		if (!GetFactoryComponentFromCol(destoryCol)) return;
	}

	void DestroyMode::Destory(const std::weak_ptr<Collider>& destoryCol)
	{
		//Objectを取得
		const auto& object = GetObjectFromCol(destoryCol);
		if (!object) return;

		//FactoryComponentを取得
		const auto& factoryCom = GetFactoryComponentFromCol(destoryCol);
		//FactoryComponentを持ってなかったとき
		if (!factoryCom) return;

		const auto& safePlayerItem = m_playerItem.lock();
		if (!safePlayerItem) return;

		//アイテムバーに追加
		safePlayerItem->AddItem(factoryCom->MakeItemStackFromThisComponent(), 1);

		const auto& inputItemSlot = factoryCom->GetInputItemSlot().lock();

		if (!inputItemSlot) return;

		for (int i = 0; i < inputItemSlot->GetSlotCount(); i++)
		{
			auto item = inputItemSlot->GetItemOwnership(i);
			if (!item) continue;
			auto itemCount = item->GetItemCount();
			safePlayerItem->AddItem(std::move(item), itemCount);
		}

		const auto& outputItemSlot = factoryCom->GetOutputItemSlot().lock();

		for (int i = 0; i < outputItemSlot->GetSlotCount(); i++)
		{
			auto item = outputItemSlot->GetItemOwnership(i);
			if (!item) continue;
			auto itemCount = item->GetItemCount();
			safePlayerItem->AddItem(std::move(item), itemCount);
		}

		//オブジェクトを破壊する
		object->Destroy();
	}

	void DestroyMode::InputAction(const std::weak_ptr<Collider>& destoryCol)
	{

		if (!IsDestoryState()) return;

		if (m_input.GetAction("Decide")->GetPhase() == ButtonPhase::kPressed)
		{
			Destory(destoryCol);
		}
	}

	bool DestroyMode::IsDestoryState() const
	{
		const auto& safeState = m_state.lock();
		if (!safeState) return false;
		return safeState->CheckCurrentState(CharactorState::DESTROY);
	}

	std::shared_ptr<FactoryComponent> DestroyMode::GetFactoryComponentFromCol(const std::weak_ptr<Collider>& col)
	{
		//Objectを取得
		const auto& object = GetObjectFromCol(col);
		if (!object) return nullptr;

		//FactoryComponentを取得
		const auto& factoryCom = object->GetComponent<FactoryComponent>();

		return factoryCom;
	}

	std::shared_ptr<Object> DestroyMode::GetObjectFromCol(const std::weak_ptr<Collider>& col)
	{
		const auto& safeDestroyCol = col.lock();
		if (!safeDestroyCol) return nullptr;

		//Objectを取得
		const auto& object = safeDestroyCol->GetParentObject();
		return object;
	}

	RayCastResult DestroyMode::RayCastFromMousePoint() const
	{
		RayCastResult result;
		result.RaycastFromMousePoint();
		return result;
	}


}
