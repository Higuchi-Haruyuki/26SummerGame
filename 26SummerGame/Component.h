#pragma once
#include <memory>
#include <cassert>
#include "vector.h"
#include "Object.h"

class HitPoint;

class Component: public std::enable_shared_from_this<Component>
{
public:
	Component(std::weak_ptr<Object> parentObject): m_parentObject(parentObject){}
	virtual ~Component() = default;

	//コンストラクタとは別に作成時に実行され、シーンにコンポーネントを追加する処理などを行う
	virtual void Init();

	//毎フレーム実行
	virtual void Update();

	//毎フレームUpdateのあとに実行
	virtual void LateUpdate();

	//デストラクタとは別にオブジェクト破棄じに実行される
	virtual void Finalize();

	std::weak_ptr<Object> GetParentObject() const { return m_parentObject; }

	Vector GetPosition() const;

	virtual void SetPosition(const Vector& newPos);

	/// <summary>
	/// 指定した型のコンポーネントを検索し、見つかった場合は共有ポインタとして返す。
	/// </summary>
	/// <typeparam name="T">検索するコンポーネントの型</typeparam>
	/// <returns>指定した型にキャスト可能な最初のコンポーネントの std::shared_ptr。見つからない場合は nullptr を返す。</returns>
	template <std::derived_from<Component> T>
	std::weak_ptr<T> GetComponent()
	{
		const auto& parentObj = GetParentObject().lock();
		if (!parentObj) return std::weak_ptr<T>();

		return parentObj->GetComponent<T>();

	}

	/// <summary>
	/// 指定した型のコンポーネントを追加し、成功した場合は共有ポインタとして返す。
	/// </summary>
	/// <typeparam name="T">追加するコンポーネントの型</typeparam>
	/// <returns>指定した型の std::shared_ptr。追加に失敗した場合は nullptr を返す。</returns>
	template <std::derived_from<Component> T>
	std::weak_ptr<T> AddComponent()
	{
		const auto& parentObj = GetParentObject().lock();
		if (!parentObj) return std::weak_ptr<T>();

		return parentObj->AddComponent<T>();
	}

	//なにかと衝突しているとき毎フレーム呼び出される関数
	virtual void OnCollisionStay(const HitPoint& hitPoint, std::weak_ptr<Object> object);
	//なにかと衝突したときそのフレームだけで呼び出される関数
	virtual void OnCollisionEnter(const HitPoint& hitPoint, std::weak_ptr<Object> object);
	//なにかと衝突状態ではなくなったとき
	virtual void OnCollisionExit(const HitPoint& hitPoint, std::weak_ptr<Object> object);

	void IsEnable(const bool& isEnable) { m_isEnable = isEnable; }
protected:
	std::weak_ptr<Object> m_parentObject;
	bool m_isEnable = true;
};