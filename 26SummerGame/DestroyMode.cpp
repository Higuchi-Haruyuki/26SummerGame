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
#include "ItemStackFactory.h"

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

	void DestroyMode::Update()
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
		if (!m_playerItem.lock()) m_playerItem = GetComponent<PlayerItem>();

		if (!m_state.lock()) m_state = GetComponent<CharactorStateManager>();
	
	}

	void DestroyMode::PreviewDestoryObject(const std::weak_ptr<Collider>& destoryCol)
	{
		//FactoryComponentを持っていないとき
		if (!GetFactoryComponentFromCol(destoryCol).lock()) return;
	}

	void DestroyMode::Destory(const std::weak_ptr<Collider>& destoryCol)
	{
		//Objectを取得
		const auto& object = GetObjectFromCol(destoryCol).lock();
		if (!object) return;

		//FactoryComponentを取得
		const auto& factoryCom = GetFactoryComponentFromCol(destoryCol).lock();
		//FactoryComponentを持ってなかったとき
		if (!factoryCom) return;

		const auto& safePlayerItem = m_playerItem.lock();
		if (!safePlayerItem) return;

		//アイテムバーに追加
		const auto itemType = factoryCom->GetItemType();

		safePlayerItem->AddItem(ItemStackFactory::Make(itemType,1), 1);

		//格納用配列の宣言と初期化
		std::vector<std::pair<std::unique_ptr<ItemStack>, int>> allItem = {};
		
		//工業オブジェクトからすべてのアイテムの所有権を取得
		factoryCom->GetAllItemOwnership(&allItem);

		//プレイヤーにアイテムを移動する
		for (auto& [item,itemCount] : allItem)
		{
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

	std::weak_ptr<FactoryComponent> DestroyMode::GetFactoryComponentFromCol(const std::weak_ptr<Collider>& col)
	{
		//Objectを取得
		const auto& object = GetObjectFromCol(col).lock();
		if (!object) return std::weak_ptr<FactoryComponent>();

		//FactoryComponentを取得
		const auto& factoryCom = object->GetComponent<FactoryComponent>();

		return factoryCom;
	}

	std::weak_ptr<Object> DestroyMode::GetObjectFromCol(const std::weak_ptr<Collider>& col)
	{
		const auto& safeDestroyCol = col.lock();
		if (!safeDestroyCol) return std::weak_ptr<Object>();

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
