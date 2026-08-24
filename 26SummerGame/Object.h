#pragma once
#include <memory>
#include <vector>
#include <functional>
#include "vector.h"
#include "VectorInt.h"
class Image;
class Component;
class HitPoint;
class Collider;
class Shape;
class CircleCollider;
class Object : public std::enable_shared_from_this<Object>
{
public:
	Object() {}
	virtual ~Object() = default;

	//コンストラクタとは別に作成時に実行され、シーンにオブジェクトを追加する処理などを行う
	virtual void Init();

	//毎フレーム実行
	virtual void Update(float deltaTime);

	//毎フレームUpdateのあとに実行
	virtual void LateUpdate();

	//デストラクタとは別にオブジェクト破棄じに実行される
	virtual void Finalize();

	virtual void Destroy(); 

	//GETTER
	bool GetIsEnable() const { return m_isEnable; }
	Vector GetPosition() const { return m_position; }
	VectorInt GetGridPosition() const { return m_gridPosition; }
	float GetTimer() const { return m_timer; }
	std::string GetTag() const { return m_tag; }
	std::shared_ptr<Object> GetParentObject() const { return m_parentObject; }
	std::vector<std::shared_ptr<Object>> GetChildObjects() const { return m_childObjects; }

	//SETTER
	void SetIsEnable(const bool& isEnable) { m_isEnable = isEnable; }
	void SetPosition(const Vector& position);
	void SetTag(const std::string& tag) { m_tag = tag; }

	/// <summary>
	/// 子オブジェクトを追加し、親オブジェクトを設定します。
	/// </summary>
	/// <param name="child">追加する子オブジェクト（std::shared_ptr<Object> 型の参照）。</param>
	void SetChild(const std::shared_ptr<Object>& child)
	{
		m_childObjects.push_back(child);
		//子オブジェクトの親オブジェクトがこのオブジェクトではないとき子オブジェクトの変数にセットする
		if (!child->GetParentObject()) child->SetParent(shared_from_this());

	}

	/// <summary>
	/// 親オブジェクトを設定し、このオブジェクトを親の子オブジェクトとして追加します。
	/// </summary>
	/// <param name="parent">新しく設定する親オブジェクト（std::shared_ptr<Object> 型の参照）。</param>
	void SetParent(const std::shared_ptr<Object>& parent)
	{
		m_parentObject = parent;
		//親オブジェクトがこのオブジェクトを子オブジェクトとしてないとき子オブジェクトの配列にセットする
		if (!m_parentObject->HasChild(shared_from_this())) m_parentObject->SetChild(shared_from_this());
	}

	/// <summary>
	/// 指定されたオブジェクトが子オブジェクトとして存在するかどうかを判定します。
	/// </summary>
	/// <param name="obj">存在を確認する対象のオブジェクト（std::shared_ptr<Object> 型の参照）。</param>
	/// <returns>子オブジェクトとして存在する場合は true、存在しない場合は false を返します。</returns>
	bool HasChild(const std::shared_ptr<Object>& obj)
	{
		for (const auto& child : m_childObjects)
		{
			if (child == obj) return true;
		}
		return false;
	}

	//なにかと衝突しているとき毎フレーム呼び出される関数
	virtual void OnCollisionStay(const HitPoint& hitPoint, const std::shared_ptr<Object> object);
	//なにかと衝突したときそのフレームだけで呼び出される関数
	virtual void OnCollisionEnter(const HitPoint& hitPoint, const std::shared_ptr<Object> object);
	//なにかと衝突状態ではなくなったとき
	virtual void OnCollisionExit(const HitPoint& hitPoint, const std::shared_ptr<Object> object);

	/// <summary>
	/// 指定した型のコンポーネントを検索し、見つかった場合は共有ポインタとして返す。
	/// </summary>
	/// <typeparam name="T">検索するコンポーネントの型</typeparam>
	/// <returns>指定した型にキャスト可能な最初のコンポーネントの std::shared_ptr。見つからない場合は nullptr を返す。</returns>
	template <typename T>
	std::shared_ptr<T> GetComponent()
	{
		for (const auto& component : m_components)
		{
			if (std::shared_ptr<T> castedComponent = std::dynamic_pointer_cast<T>(component))
			{
				return castedComponent;
			}
		}
		return nullptr;

	}

	/// <summary>
	/// 指定した型のコンポーネントを追加し、成功した場合は共有ポインタとして返す。
	/// </summary>
	/// <typeparam name="T">追加するコンポーネントの型</typeparam>
	/// <returns>指定した型の std::shared_ptr。追加に失敗した場合は nullptr を返す。</returns>
	template <std::derived_from<Component> T>
	std::shared_ptr<T> AddComponent()
	{
		auto newComponent = std::make_shared<T>(shared_from_this());
		//コンポーネントリストにそのコンポーネントが存在しないとき
		if (GetComponent<T>() == nullptr) {
			m_components.push_back(newComponent);
			newComponent->Init();
			return newComponent;
		}
		return nullptr;
	}

public:
	/// <summary>
	/// オブジェクトと衝突していると保持している関数を実行する
	/// </summary>
	std::function<void(const std::shared_ptr<Object>&) > OnCollisionStayFunc;
	/// <summary>
	/// オブジェクトと衝突し始めるとそのフレームのみ保持している関数を実行する
	/// </summary>
	std::function<void(const std::shared_ptr<Object>&) > OnCollisionEnterFunc;
	/// <summary>
	/// オブジェクトと衝突している状態が終了したとき保持している関数を実行する
	/// </summary>
	std::function<void(const std::shared_ptr<Object>&) > OnCollisionExitFunc;

protected:
	bool m_isEnable = true;
	Vector m_position{ 0,0,0 };
	VectorInt m_gridPosition{ 0,0,0 };
	float m_timer = 0;
	std::string m_tag;
	//所有するコンポーネント
	std::vector<std::shared_ptr<Component>> m_components;
	//親オブジェクト(ないならnullptr)
	std::shared_ptr<Object> m_parentObject;
	//子オブジェクトの配列
	std::vector<std::shared_ptr<Object>> m_childObjects;
};
