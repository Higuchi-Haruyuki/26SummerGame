#pragma once
#include <cfloat>
#include <memory>
#include <vector>
#include "vector.h"

class Collider;
class Ray;

/// <summary>
/// レイキャストの結果を格納するクラス
/// </summary>
class RayCastResult
{
public:
	RayCastResult(){};
	~RayCastResult() {};

	/// <summary>
	/// 画面の中心からレイをとばしてその結果を格納する。
	/// </summary>
	void RaycastFromScreenCenter();

	/// <summary>
	/// マウスカーソルの位置からレイをとばしてその結果を格納する。
	/// </summary>
	void RaycastFromMousePoint();


	/// <summary>
	/// レイがヒットしたコライダーの弱参照を返す。ヒットしていない場合は空の弱参照を返す。
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<Collider> GetHitCollider() const { return m_collider; }

	/// <summary>
	/// レイがヒットした距離を返す。ヒットしていない場合はFLT_MAXを返す。
	/// </summary>
	/// <returns></returns>
	float GetDistance() const { return m_distance; }

	/// <summary>
	/// レイがヒットした地点の座標を返す。ヒットしていない場合は{0,0,0}を返す。
	/// </summary>
	/// <returns></returns>
	Vector GetHitPoint() const { return m_hitPoint; }

	void Reset()
	{
		m_collider.lock() = nullptr;
		m_distance = FLT_MAX;
		m_hitPoint = { 0,0,0 };
	}

private:

	/// <summary>
	/// レイキャストの計算を行い、結果を格納する。
	/// </summary>
	/// <param name="ray"></param>
	void RayCastCalc(Ray& ray);

private:
	//ヒットしたコライダー
	std::weak_ptr<Collider> m_collider;

	//距離
	float m_distance = FLT_MAX;
	
	//ヒット地点
	Vector m_hitPoint;

};
