#pragma once
#include"vector.h"
class HitPoint
{
public:
	HitPoint() { m_isCollide = false; }
	inline bool GetIsCollide() const { return m_isCollide; }
	inline Vector GetNormal() const { return m_normal; }
	inline Vector GetHitPoint() const { return m_hitPoint; }
	inline float GetPenetration() const { return m_penetration; }
	inline void SetIsCollide(const bool& value) { m_isCollide = value; }
	inline void SetNormal(const Vector& vec) { m_normal = vec; }
	inline void SetHitPoint(const Vector& vec) { m_hitPoint = vec; }
	inline void SetPenetration(const float& value) { m_penetration = value; }
private:
	//衝突しているか
	bool m_isCollide;
	//法線ベクトル
	Vector m_normal;
	//衝突座標
	Vector m_hitPoint;
	//めり込み量
	float m_penetration = 0;
};