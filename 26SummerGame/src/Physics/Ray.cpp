#include "Ray.h"
#include <DxLib.h>
#include <cmath>
#include <utility>
#include <Windows.h>
#include "vector.h"
#include "Game.h"


void Ray::CreateScreenCenterRay()
{
	CreateScreenPointToRay(Game::kDisplaySize / 2);
}

void Ray::CreateScreenPointToRay(const Vector& screenPoint)
{
	const Vector nearWorldPos = ConvScreenPosToWorldPos({ screenPoint.m_x,screenPoint.m_y,0.0f });
	const Vector farWorldPos = ConvScreenPosToWorldPos({ screenPoint.m_x,screenPoint.m_y,1.0f });

	m_origin = nearWorldPos;

	Vector temp = farWorldPos - nearWorldPos;

	temp.Normalize();

	m_direction = temp;
}

bool Ray::RayIntersects(const Vector& minPos, const Vector& maxPos, float maxDistance, float* tOut)
{
	float tMin = 0.0f;
	float tMax = maxDistance;

	//X,Y,Zの各軸について、レイがスラブと交差する区間を計算
	const float dirArr[3] = { m_direction.m_x, m_direction.m_y, m_direction.m_z };

	const float originArr[3] = { m_origin.m_x, m_origin.m_y, m_origin.m_z };

	const float minArr[3] = { minPos.m_x, minPos.m_y, minPos.m_z };

	const float maxArr[3] = { maxPos.m_x, maxPos.m_y, maxPos.m_z };

	for (int axis = 0; axis < 3; axis++)
	{
		if (fabsf(dirArr[axis]) < 1e-6f)
		{
			//この軸にレイがほぼ進まない場合:
			//起点がスラブの範囲外なら絶対に交差しない
			if (originArr[axis]  < minArr[axis] || originArr[axis] > maxArr[axis]) return false;
		}
		else
		{
			//スラブとの交差距離 t1, t2 を計算する。
			const float invDir = 1.0f / dirArr[axis];
			float t1 = (minArr[axis] - originArr[axis]) * invDir;
			float t2 = (maxArr[axis] - originArr[axis]) * invDir;

			if (t1 > t2)
			{
				std::swap(t1, t2);
			}

			tMin = max(tMin, t1);
			tMax = min(tMax, t2);

			//区間が潰れた->交差なし
			if (tMin > tMax) return false;

		}
	}

	*tOut = tMin;
	return true;
}
