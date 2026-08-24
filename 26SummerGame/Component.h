#pragma once
#include <memory>
class Object;
class HitPoint;

class Component: public std::enable_shared_from_this<Component>
{
public:
	Component(std::shared_ptr<Object> parentObject): m_parentObject(parentObject){}
	virtual ~Component() = default;

	//コンストラクタとは別に作成時に実行され、シーンにコンポーネントを追加する処理などを行う
	virtual void Init();

	//毎フレーム実行
	virtual void Update(float deltaTime);

	//毎フレームUpdateのあとに実行
	virtual void LateUpdate();

	//デストラクタとは別にオブジェクト破棄じに実行される
	virtual void Finalize();

	std::shared_ptr<Object> GetParentObject() const { return m_parentObject; }

	//なにかと衝突しているとき毎フレーム呼び出される関数
	virtual void OnCollisionStay(const HitPoint& hitPoint, const std::shared_ptr<Object> object);
	//なにかと衝突したときそのフレームだけで呼び出される関数
	virtual void OnCollisionEnter(const HitPoint& hitPoint, const std::shared_ptr<Object> object);
	//なにかと衝突状態ではなくなったとき
	virtual void OnCollisionExit(const HitPoint& hitPoint, const std::shared_ptr<Object> object);

	void IsEnable(const bool& isEnable) { m_isEnable = isEnable; }
protected:
	std::shared_ptr<Object> m_parentObject;
	bool m_isEnable = true;
	float m_timer = 0;
};