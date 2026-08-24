#include "Collider.h"
#include "Object.h"
#include "HitPoint.h"
#include "SquareCollider3D.h"
#include "Scene.h"
#include <algorithm>
#include "SceneManager.h"
#include <string>
#include "Debug.h"
#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>
#include "Component.h"
#include "vector.h"
#include <cmath>

void Collider::Init()
{
	Component::Init();
	//現在のシーンのColliderの配列にこのコンポーネントを追加
	SceneManager::GetInstance().AddColliderToCurrentScene(std::dynamic_pointer_cast<Collider>(shared_from_this()));
}
void Collider::Update(float deltaTime)
{
	Component::Update(deltaTime);
	if (!m_isEnable) return;
	//ひとつ前のフレームの配列に移動させて、現在のフレームの配列は破棄する
	m_collideObjectBeforeFrame = std::move(m_collideObjectThisFrame);
	m_collideObjectThisFrame.clear();

	//法線の総和をリセット
	m_sumNormal = { 0,0 };
	//このフレームでの押し戻し量の和をリセット
	m_sumPushBack = { 0,0 };

}
void Collider::LateUpdate()
{
	Component::LateUpdate();
	if (!m_isEnable) return;
	CollisionAndPushBack();
}
void Collider::Finalize()
{
	Component::Finalize();
	m_collideObjectBeforeFrame.clear();
	m_collideObjectThisFrame.clear();
	SceneManager::GetInstance().GetCurrentScene()->AddToRemoveColliders(std::dynamic_pointer_cast<Collider>(shared_from_this()));
}
std::vector<std::shared_ptr<Object>> Collider::GetNearObjects(int radius) const
{
	std::vector<std::shared_ptr<Object>> nearObjects;
	if (radius < 0) return nearObjects;
	auto myPos = GetParentObject()->GetPosition();
	auto sqRadius = radius * radius;
	for (const auto& collider : SceneManager::GetInstance().GetCurrentScene()->GetSceneColliders())
	{
		auto obj = collider->GetParentObject();
		//距離の二乗が半径の二乗より小さいときにそのオブジェクトは範囲内にいる。
		float sqdist = myPos.SqDistance(obj->GetPosition());
		if (sqdist < sqRadius)
		{
			//距離の二乗が0ならそれは自分のコライダーなので含めない
			if (sqdist == 0) continue;

			nearObjects.push_back(obj);
		}
	}

	return nearObjects;
}
bool Collider::isCollideThisFrame(const std::string& tag)
{
	if (m_collideObjectThisFrame.size() == 0) return false;
	for (auto& obj : m_collideObjectThisFrame)
	{
		if (obj->GetTag() == tag)
		{
			return true;
		}

	}
	return false;
}
bool Collider::isCollideBeforeFrame(const std::string& tag)
{
	if (m_collideObjectBeforeFrame.size() == 0) return false;
	for (auto& obj : m_collideObjectBeforeFrame)
	{
		if (obj->GetTag() == tag)
		{
			return true;
		}
	}
	return false;
}
void Collider::CollisionAndPushBack()
{
	//ステージのColliderを取得する
	const auto& sceneCol = SceneManager::GetInstance().GetCurrentScene()->GetSceneColliders();
	//ステージColliderの要素数が0のときは関数を終了する
	if (static_cast<int>(sceneCol.size()) == 0) return;
	//加速度を速度に適用
	m_velocity += m_accel;
	//現在位置と速度から移動位置を仮定
	Vector resultPos = GetParentObject()->GetPosition() + m_offset + m_velocity;

	std::string myTag = GetParentObject()->GetTag();
	if (myTag == "Ground") return;
	//衝突しているかチェックする
	for (const auto& col : sceneCol)
	{
		if(!SceneManager::GetInstance().GetCurrentScene()->HasSceneColliders(col)) continue;

		//そのコンポーネントを持つオブジェクトが非アクティブのとき処理をスキップする
		if (!col->GetParentObject()->GetIsEnable()) continue;

		//そのコンポーネントが非アクティブのとき処理をスキップする
		if (!col->m_isEnable) continue;

		//当たり判定を無視するタグとしてもっているとき処理をスキップする
		if (HasIgnoreTag(col->GetParentObject()->GetTag()) || col->HasIgnoreTag(myTag)) continue;

		//このコンポーネントがSquareCollider3D
		if (std::shared_ptr<SquareCollider3D> mySquare = std::dynamic_pointer_cast<SquareCollider3D>(shared_from_this()))
		{
			//相手のコンポーネントがSquareColliderのとき
			if (std::shared_ptr<SquareCollider3D> targetSquare = std::dynamic_pointer_cast<SquareCollider3D>(col))
			{
				//比較する2つが同じものを指しているときは処理をスキップ
				if (mySquare == targetSquare) continue;
				//当たり判定処理
				isCollide(resultPos, mySquare, targetSquare);
			}
		}


	}


	//法線の向きが上向きのとき（地面に衝突しているといえるとき） かつ 速度ベクトルが下方向のときに落下速度を0にする
	if (m_sumNormal.m_y > 0.0f && GetVelocity().m_y < 0.0f)
	{
		SetVelocityY(0.0f);
	}
	//法線の向きが下向きの時(上方向にぶつかったとき)　かつ　速度ベクトルが上方向の時は反射するようにする
	if (m_sumNormal.m_y < 0.0f && GetVelocity().m_y > 0.0f)
	{
		//横方向から法線ベクトルがあるときは引っかかっているとみなして押し戻しのyを0にする
		if (m_sumNormal.m_x != 0)
		{
			m_sumPushBack.m_y = 0;
		}
		else
		{
			SetVelocityY(-GetVelocity().m_y);
		}
	}
	//横方向でオブジェクトとあたっているときは速度ベクトルのx成分を0にする
	if (m_sumNormal.m_x != 0)
	{
		m_velocity.m_x = 0;
	}
	//Debug::Log(std::format("velocity: {}", m_velocity.ToString()));
	//Debug::Log(std::format("sumNormal: {}", m_sumNormal.ToString()));
	//Debug::Log(std::format("sumPushBack:{}", m_sumPushBack.ToString()));
	//仮定した値に補正する押し戻し量を加えてプレイヤーの座標にする
	GetParentObject()->SetPosition(resultPos - m_offset + m_sumPushBack);
}

HitPoint Collider::isCollide(const Vector& resultPos, const std::shared_ptr<SquareCollider3D> squareACollider, const std::shared_ptr<SquareCollider3D> squareBCollider)
{
	HitPoint hitPoint;
	auto tempPos = resultPos + m_sumPushBack;
	auto squareA = squareACollider->GetParentObject();
	auto squareB = squareBCollider->GetParentObject();

	auto squareBPos = squareB->GetPosition() + squareBCollider->GetOffset();

	//長方形のxの差分を求める
	auto xDiff = squareBPos.m_x - tempPos.m_x;

	//長方形のyの差分を求める
	auto yDiff = squareBPos.m_y - tempPos.m_y;

	//長方形のzの差分を求める
	auto zDiff = squareBPos.m_z - tempPos.m_z;

	//それぞれの軸の長さを回転させたあとの値を取得
	float widthA = cosf(squareACollider->GetRotationAngle()) * squareACollider->GetWidth() 
		+ sinf(squareACollider->GetRotationAngle()) * squareACollider->GetDepth();

	float depthA = sinf(squareACollider->GetRotationAngle()) * squareACollider->GetWidth()
		+ cosf(squareACollider->GetRotationAngle()) * squareACollider->GetDepth();

	float widthB = cosf(squareBCollider->GetRotationAngle()) * squareBCollider->GetWidth()
		+ sinf(squareBCollider->GetRotationAngle()) * squareBCollider->GetDepth();

	float depthB = sinf(squareBCollider->GetRotationAngle()) * squareBCollider->GetWidth()
		+ cosf(squareBCollider->GetRotationAngle()) * squareBCollider->GetDepth();

	//それぞれの軸のめり込み量を計算
	// それぞれ（長方形の幅の半分の和）が中心座標の差の絶対値より大きいときに衝突している
	auto overlapX = (widthA / 2 + widthB / 2) - std::abs(xDiff);
	auto overlapY = (squareACollider->GetHeight() / 2 + squareBCollider->GetHeight() / 2) - std::abs(yDiff);
	auto overlapZ = (depthA / 2 + depthB / 2) - std::abs(zDiff);

	//両方の軸で重なっているとき衝突
	if (overlapX > 0 && overlapY > 0 && overlapZ > 0)
	{
		hitPoint.SetIsCollide(true);

		//衝突点の計算
		//重なっている領域のMinとMaxを求める
		float minX = (std::max)(tempPos.m_x - widthA / 2, squareBPos.m_x - widthB / 2);
		float maxX = (std::min)(tempPos.m_x + widthA / 2, squareBPos.m_x + widthB / 2);

		float minY = (std::max)(tempPos.m_y - squareACollider->GetHeight() / 2, squareBPos.m_y - squareBCollider->GetHeight() / 2);
		float maxY = (std::min)(tempPos.m_y + squareACollider->GetHeight() / 2, squareBPos.m_y + squareBCollider->GetHeight() / 2);

		float minZ = (std::max)(tempPos.m_z - depthA / 2,
			squareBPos.m_z - depthB / 2);
		float maxZ = (std::min)(tempPos.m_z + depthA / 2,
			squareBPos.m_z + depthB / 2);

		//その領域の真ん中を衝突点とする
		hitPoint.SetHitPoint(Vector{ (minX + maxX) / 2, (minY + maxY) / 2, (minZ + maxZ) / 2 });

		//x軸のほうがめり込み量が小さいとき
		//左右に押し出す
		if (overlapX < overlapY && overlapX < overlapZ)
		{
			hitPoint.SetPenetration(overlapX);
			//squareBが左にあるなら法線ベクトルは右を向く
			hitPoint.SetNormal(Vector{ xDiff < 0 ? 1.0f : -1.0f, 0 , 0 });
		}
		//y軸のほうがめり込み量が小さいとき
		//上下に押し出す
		else if (overlapY < overlapX && overlapY < overlapZ)
		{
			hitPoint.SetPenetration(overlapY);
			//squareBが下にあるなら法線ベクトルは上を向く
			hitPoint.SetNormal(Vector{ 0 , yDiff < 0 ? 1.0f : -1.0f , 0 });
		}
		else
		{
			hitPoint.SetPenetration(overlapZ);
			//squareBが下にあるなら法線ベクトルは上を向く
			hitPoint.SetNormal(Vector{ 0 , 0 , zDiff < 0 ? 1.0f : -1.0f });
		}
		
		m_collideObjectThisFrame.push_back(squareB);

		//どちらもトリガーじゃないときの処理
		if (!squareACollider->m_isTrigger && !squareBCollider->m_isTrigger)
		{
			//押し戻し量の総和を計算
			m_sumPushBack += GetPushBackValue(hitPoint, squareACollider, squareBCollider);
			//法線ベクトルの総和を計算
			m_sumNormal += hitPoint.GetNormal();
		}

		//衝突時に呼び出す関数を呼び出す
		//前回のフレームで衝突していなくて現在のフレームで衝突しているなら
		if (!isCollideBeforeFlame(squareB))
		{
			squareA->OnCollisionEnter(hitPoint, squareB);
		}
		squareA->OnCollisionStay(hitPoint, squareB);
	}
	else
	{
		hitPoint.SetIsCollide(false);

		//前回のフレームで衝突していて現在のフレームで衝突していないなら
		if (isCollideBeforeFlame(squareB))
		{
			squareA->OnCollisionExit(hitPoint, squareB);
		}
	}
	return hitPoint;
}
Vector Collider::GetPushBackValue(const HitPoint& hitPoint, const std::shared_ptr<Collider> colliderA, const std::shared_ptr<Collider> colliderB)
{
	//どちらかのisTriggerがtrueなら押し戻し判定を行わない
	if (colliderA->m_isTrigger || colliderB->m_isTrigger) return { 0,0 };
	//2つのオブジェクトの質量の逆数の和
	auto AInvMass = colliderA->InverseMass();
	auto BInvMass = colliderB->InverseMass();
	auto totalInvMass = AInvMass + BInvMass;
	//両方とも質量の逆数が0のときは押し戻し判定をしない
	if (totalInvMass > 0.0f)
	{
		//押し戻す割合を計算
		auto pushRatioA = AInvMass / totalInvMass;

		auto pushBackA = hitPoint.GetNormal() * (hitPoint.GetPenetration() * pushRatioA);

		return pushBackA;
	}
	return { 0,0 };
}
//前回のフレームで衝突しているのか確認する関数
bool Collider::isCollideBeforeFlame(const std::shared_ptr<Object> object)
{
	for (const auto& obj : m_collideObjectBeforeFrame)
	{
		if (obj == object) return true;
	}
	return false;
}
