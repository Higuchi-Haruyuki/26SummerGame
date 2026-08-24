#pragma once
#include <memory>
#include <vector>
#include "Component.h"
#include "vector.h"

class Circle;
class Square;
class Object;
class HitPoint;
class Scene;
class SquareCollider3D;

class Collider : public Component
{
public:
	Collider(std::shared_ptr<Object> parentObject) :Component(parentObject), m_velocity({ 0,0 }), m_accel({ 0,0 })
	{
		m_collideObjectBeforeFrame.clear();
		m_collideObjectThisFrame.clear();
	};
	virtual ~Collider() = default;
	void Init() override;
	void Update(float deltaTime) override;
	void LateUpdate() override;
	void Finalize() override;
	virtual void Draw(const Vector& cameraPos, const unsigned int& color) const {};

	//GETTER
	std::shared_ptr<Object> GetParentObject() const { return m_parentObject; }
	float GetMass() const { return m_mass; }
	/// <summary>
	/// 回転角を取得する。単位はラジアン。
	/// </summary>
	/// <returns></returns>
	float GetRotationAngle() const { return m_rotationAngle; }
	Vector GetVelocity() const { return m_velocity; }
	Vector GetAccel() const { return m_accel; }
	std::vector<std::shared_ptr<Object>> GetCollideObjectThisFrame() const { return m_collideObjectThisFrame; }
	Vector GetOffset() const { return m_offset; }


	/// <summary>
	/// 引数で指定した円の半径の範囲の中にいるオブジェクトの配列を返す。
	/// </summary>
	std::vector<std::shared_ptr<Object>> GetNearObjects(int radius) const;

	//SETTER
	void SetParentObject(const std::shared_ptr<Object> o) { m_parentObject = o; }
	void SetMass(const float& mass) { m_mass = mass; }
	virtual void SetSize(const Vector& size) {};
	/// <summary>
	/// 回転角度を設定する。単位はラジアン。
	/// </summary>
	/// <param name="angle"></param>
	void SetRotationAngle(float angle) { m_rotationAngle = angle; }
	void SetVelocity(const Vector& velocity) { m_velocity = velocity; }
	void SetVelocityX(const float& value) { m_velocity.m_x = value; }
	void SetVelocityY(const float& value) { m_velocity.m_y = value; }
	void SetAccel(const Vector& accel) { m_accel = accel; }
	void SetOffset(const Vector& offset) { m_offset = offset; }


	void AddVelocity(const Vector& velocity) { m_velocity += velocity; }
	void AddVelocityX(float value) { m_velocity.m_x += value; }
	void AddVelocityY(float value) { m_velocity.m_y += value; }
	void AddVelocityZ(float value) { m_velocity.m_z += value; }

	/// <summary>
	/// このフレームで衝突したオブジェクトを保存している配列にオブジェクトを追加する。
	/// </summary>
	void AddCollideObjectThisFrame(const std::shared_ptr<Object>& collideObject)
	{
		if (IsCollideThisFrame(collideObject)) return;
		m_collideObjectThisFrame.push_back(collideObject);
	}

	/// <summary>
	/// このフレームでオブジェクトと衝突しているならtrueを返す。
	/// </summary>
	/// <returns></returns>
	bool IsCollideThisFrame(const std::shared_ptr<Object>& collideObject)
	{
		for (const auto& obj : m_collideObjectThisFrame)
		{
			if (obj == collideObject) return true;
		}
		return false;
	}

	//Visible: trueにするとコライダーの範囲が見えるようになる
	void IsVisible(const bool& isVisible) { m_isVisible = isVisible; }
	//Trigger: trueにすると当たり判定はあるが押し戻しはしなくなる
	void IsTrigger(const bool& isTrigger) { m_isTrigger = isTrigger; }

	/// <summary>
	/// 当たり判定を無視するタグを追加する
	/// </summary>
	/// <param name="tagName"></param>
	void AddIgnoreTag(const std::string& tagName) { m_ignoreTag.push_back(tagName); }
	/// <summary>
	/// 当たり判定を無視するタグを削除する
	/// </summary>
	/// <param name="tagName"></param>
	void RemoveIgnoreTag(const std::string& tagName)
	{
		for (int i = 0; i < m_ignoreTag.size(); i++)
		{
			if (tagName == m_ignoreTag[i])
			{
				m_ignoreTag.erase(m_ignoreTag.begin() + i);
				return;
			}
		}
	}

	/// <summary>
	/// このタグを無視するタグとしてもっているときtrueを返す。
	/// </summary>
	/// <param name="tag"></param>
	/// <returns></returns>
	bool HasIgnoreTag(const std::string& tag)
	{
		for (const auto& ignoreTag : m_ignoreTag)
		{
			if (ignoreTag == tag) return true;
		}
		return false;
	}

	//質量の逆数を返す。
	float InverseMass() const
	{
		if (m_mass == 0)
		{
			//0除算防止のため質量が0のときは0を固定で返すようにする
			return 0;
		}
		else
		{
			return 1 / m_mass;
		}
	}
	//このフレームでタグを持つオブジェクトと衝突しているなら
	bool isCollideThisFrame(const std::string& tag);

	//前回のフレームでタグを持つオブジェクトと衝突しているなら
	bool isCollideBeforeFrame(const std::string& tag);

protected:
	//見えるか
	bool m_isVisible = false;
	//当たり判定はあるけど押し戻しはないようにする
	bool m_isTrigger = false;
	//オブジェクトの座標に対してコライダーの座標をどれだけずらすか
	Vector m_offset = { 0,0,0 };
private:

	/// <summary>
	/// SquareAの移動後の座標を追加で受け取り、押し戻し量と衝突情報を返す関数
	/// </summary>
	/// <param name="pushBackValue"></param>
	/// <param name="resultPos">SquareAの移動後の座標</param>
	/// <param name="squareACollider"></param>
	/// <param name="squareBCollider"></param>
	/// <returns></returns>
	HitPoint isCollide(const Vector& resultPos, const std::shared_ptr<SquareCollider3D> squareACollider, const std::shared_ptr<SquareCollider3D> squareBCollider);

	/// <summary>
	/// 衝突情報と該当2オブジェクトから押し戻し量を求める。
	/// </summary>
	/// <returns></returns>
	Vector GetPushBackValue(const HitPoint& hitPoint, const std::shared_ptr<Collider> objectA, const std::shared_ptr<Collider> objectB);

	/// <summary>
	/// 一つ前のフレームでオブジェクトと衝突していたならtrueを返す。
	/// </summary>
	/// <returns></returns>
	bool isCollideBeforeFlame(const std::shared_ptr<Object> object);

	/// <summary>
	/// プレイヤーの速度と位置から当たり判定の処理を行い、必要によって押し戻しを行う
	/// </summary>
	void CollisionAndPushBack();

private:
	//一つ前のフレームで衝突したオブジェクトを保存する配列
	std::vector<std::shared_ptr<Object>> m_collideObjectBeforeFrame;
	//このフレームで衝突したオブジェクトを保存する配列
	std::vector<std::shared_ptr<Object>> m_collideObjectThisFrame;

	//当たり判定を無視するタグを保存
	std::vector <std::string> m_ignoreTag;

	//このフレームでの法線ベクトルの和
	Vector m_sumNormal = { 0,0 };

	//このフレームでの押し戻し量の和
	Vector m_sumPushBack = { 0,0 };

	//速度
	Vector m_velocity;

	//加速度
	Vector m_accel;

	//質量
	float m_mass = 10;

	//回転角(ラジアン)
	float m_rotationAngle = 0.0f;
};