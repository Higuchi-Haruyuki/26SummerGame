#include "RayCastResult.h"
#include "Collider.h"
#include "Ray.h"
#include "SceneManager.h"
#include "Scene.h"
#include <cfloat>
#include <memory>

#include "SquareCollider3D.h"

#include"Debug.h"

#include "Game.h"

namespace
{
	// レイキャストのインタラクション可能な最大距離（要調整）
	constexpr float kInteractRayMaxDistance = FLT_MAX;
}
void RayCastResult::RaycastFromScreenCenter()
{
	Ray ray;
	ray.CreateScreenCenterRay();

	RayCastCalc(ray);
}

void RayCastResult::RaycastFromMousePoint()
{
	int x = 0, y = 0;

	GetMousePoint(&x, &y);

	Ray ray;
	ray.CreateScreenPointToRay(
		{static_cast<float>(x),
		static_cast<float>(y)}
	);

	RayCastCalc(ray);
}

void RayCastResult::RayCastCalc(Ray& ray)
{
	//現在のシーンにあるコライダーの配列を取得
	const auto& colliders = SceneManager::GetInstance().
		GetCurrentScene()->GetSceneColliders();

	float minDistance = FLT_MAX;

	//SquareCollider3Dのみを考慮する。
	for (const auto& col : colliders)
	{
		const auto& obj = col->GetParentObject().lock();
		if (!obj) continue;

		if (obj->GetTag() == "Player") continue;

		const auto& squareCol = std::dynamic_pointer_cast<SquareCollider3D>(col);
		if (!squareCol) continue;
		if (!ray.RayIntersects(squareCol->GetMin(), squareCol->GetMax(),
			kInteractRayMaxDistance, &minDistance)) continue;

		if (minDistance < m_distance)
		{
			m_distance = minDistance;
			m_collider = squareCol;
			m_hitPoint = ray.m_origin + ray.m_direction * minDistance;
		}
		int i = 0;
	}
}

