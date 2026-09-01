#pragma once
#include <string>
#include <format>
#include <DxLib.h>
#include "VectorInt.h"
#include <cmath>
#include <algorithm>

class Vector {
public:
	float m_x;
	float m_y;
	float m_z;
	Vector(float _x = 0, float _y = 0, float _z = 0) : m_x(_x), m_y(_y), m_z(_z) {};
	Vector(const VECTOR& v) : m_x(v.x), m_y(v.y), m_z(v.z) {};
	Vector operator+(const Vector& v) const
	{
		Vector newVec;
		newVec.m_x = m_x + v.m_x;
		newVec.m_y = m_y + v.m_y;
		newVec.m_z = m_z + v.m_z;
		return newVec;
	}
	Vector operator-(const Vector& v) const
	{
		Vector newVec;
		newVec.m_x = m_x - v.m_x;
		newVec.m_y = m_y - v.m_y;
		newVec.m_z = m_z - v.m_z;
		return newVec;
	}
	Vector operator-() const
	{
		Vector newVec;
		newVec.m_x = -m_x;
		newVec.m_y = -m_y;
		newVec.m_z = -m_z;
		return newVec;
	}
	Vector operator*(const float& v) const
	{
		Vector newVec{ m_x * v,m_y * v, m_z * v };
		return newVec;
	}
	Vector operator/(const float& v) const
	{
		if (v == 0.0f) return { m_x,m_y,m_z };
		Vector newVec{ m_x / v, m_y / v , m_z / v };
		return newVec;
	}
	void operator+=(const Vector& v)
	{
		m_x += v.m_x;
		m_y += v.m_y;
		m_z += v.m_z;
	}
	void operator-=(const Vector& v)
	{
		m_x -= v.m_x;
		m_y -= v.m_y;
		m_z -= v.m_z;
	}
	void operator*=(const float& v)
	{
		m_x *= v;
		m_y *= v;
		m_z *= v;
	}
	bool operator==(const Vector& v) const
	{
		if (m_x != v.m_x) return false;
		if (m_y != v.m_y) return false;
		if (m_z != v.m_z) return false;
		return true;
	}

	void operator=(const VECTOR& v)
	{
		m_x = v.x;
		m_y = v.y;
		m_z = v.z;
	}

	std::string ToString() const
	{
		return std::format("({},{},{})", m_x, m_y, m_z);
	}
	VectorInt ToVectorInt() const
	{
		return VectorInt{ static_cast<int>(m_x),static_cast<int>(m_y),static_cast<int>(m_z) };
	}
	VECTOR ToVECTOR()const
	{
		return VGet(m_x, m_y, m_z);
	}
	void FromVECTOR(const VECTOR& VEC)
	{
		m_x = VEC.x;
		m_y = VEC.y;
		m_z = VEC.z;
	}
	float Dot(const Vector& v) const
	{
		auto xTemp = m_x * v.m_x;
		auto yTemp = m_y * v.m_y;
		auto zTemp = m_z * v.m_z;
		return xTemp + yTemp + zTemp;
	}

	float SqDistance(const Vector& v) const
	{
		return (m_x - v.m_x) * (m_x - v.m_x) + (m_y - v.m_y) * (m_y - v.m_y) + (m_z - v.m_z) * (m_z - v.m_z);
	}
	float Distance(const Vector& v) const
	{
		return static_cast<float>(sqrt(SqDistance(v)));
	}
	float SqLength() const
	{
		return m_x * m_x + m_y * m_y + m_z * m_z;
	}
	float Length() const
	{
		return static_cast<float>(sqrt(SqLength()));
	}
	void Normalize()
	{
		auto len = Length();
		if (len == 0.0f) return;
		m_x /= len;
		m_y /= len;
		m_z /= len;
	}

	/// <summary>
	/// このベクトルをY軸を回転軸として回転させたベクトルを返す。(回転単位はラジアン。)
	/// </summary>
	Vector RotationY(float rad) const
	{
		Vector newVec
		{
			cosf(rad) * m_x + sinf(rad) * m_z,
			m_y,
			sinf(rad) * m_x + cosf(rad) * m_z
		};
		return newVec;
	}

	/// <summary>
	/// Y軸を回転軸として回転させたベクトルにする。(回転単位はラジアン。)
	/// </summary>
	void RotateY(float rad)
	{
		Vector newVec
		{
			cosf(rad) * m_x + sinf(rad) * m_z,
			m_y,
			sinf(rad) * m_x + cosf(rad) * m_z
		};
		m_x = newVec.m_x;
		m_z = newVec.m_z;
	}

	void Clamp(Vector min, Vector max)
	{
		 m_x = std::clamp(m_x,min.m_x, max.m_x);
		 m_y = std::clamp(m_y, min.m_y, max.m_y);
		 m_z = std::clamp(m_z, min.m_z, max.m_z);
	}
};