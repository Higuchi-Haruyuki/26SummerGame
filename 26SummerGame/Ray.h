#pragma once
#include "vector.h"


class Ray
{
public:
	Vector m_origin;
	Vector m_direction;

	/// <summary>
	/// 画面の中心からレイを生成する。
	/// </summary>
	void CreateScreenCenterRay();

	/// <summary>
	/// 画面の指定された点からレイを生成する。
	/// </summary>
	/// <param name="screenPoint">画面の点</param>
	void CreateScreenPointToRay(const Vector& screenPoint);

	/// <summary>
	/// 衝突していたらtrueを返し、tOUtにヒットまでの距離を格納する。
	/// </summary>
	/// <param name="minPos">AABBの最小点</param>
	/// <param name="maxPos">AABBの最大点</param>
	/// <param name="maxDistance">判定を出す距離</param>
	/// <param name="tOut">ヒットまでの距離を格納する変数</param>
	/// <returns>衝突したか</returns>
	bool RayIntersects(const Vector& minPos,
						const Vector& maxPos, float maxDistance,
						float* tOut);
	

};

