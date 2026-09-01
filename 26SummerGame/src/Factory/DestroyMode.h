#pragma once
#include "Component.h"
#include <memory>

class Object;
class PlayerItem;
class PlayerInput;
class SceneManager;
class CharactorStateManager;
class RayCastResult;
class Vector;
class Collider;
class FactoryComponent;

	/// <summary>
	/// 破壊モードでの処理を行う。
	/// </summary>
	class DestroyMode :
		public Component
	{
	public:
		DestroyMode(std::shared_ptr<Object> parentObject);
		~DestroyMode() override = default;
		void Init() override;
		void Update() override;

	private:

		/// <summary>
		/// コンポーネントへの参照を取得する。
		/// </summary>
		void GetComponentReference();

		void PreviewDestoryObject(const std::weak_ptr<Collider>& destoryCol);

		void Destory(const std::weak_ptr<Collider>& destoryCol);

		/// <summary>
		/// 入力処理
		/// </summary>
		/// <param name="installationPos">破壊するオブジェクトのコライダー</param>
		void InputAction(const std::weak_ptr<Collider>& destoryCol);

		/// <summary>
		/// 現在、破壊モードである
		/// </summary>
		/// <returns></returns>
		bool IsDestoryState() const;

		/// <summary>
		/// Colliderからそのオブジェクトが持っているFactoryComponentを取得する。
		/// </summary>
		/// <param name="col"></param>
		/// <returns></returns>
		std::weak_ptr<FactoryComponent> GetFactoryComponentFromCol(const std::weak_ptr<Collider>& col);

		/// <summary>
		/// Colliderからそのオブジェクトを取得する。
		/// </summary>
		/// <param name="col"></param>
		/// <returns></returns>
		std::weak_ptr<Object> GetObjectFromCol(const std::weak_ptr<Collider>& col);

		/// <summary>
		/// マウスの位置からレイキャストを行う。
		/// </summary>
		RayCastResult RayCastFromMousePoint() const;

	private:

		//シングルトンへの参照
		PlayerInput& m_input;
		SceneManager& m_sceneManager;

		//プレイヤーのアイテムコンポーネントへの弱参照
		std::weak_ptr<PlayerItem> m_playerItem;

		//プレイヤーのステートを管理するコンポーネントへの弱参照
		std::weak_ptr<CharactorStateManager> m_state;

	};




