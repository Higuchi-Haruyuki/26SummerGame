#pragma once
#pragma once
#include <string>
#include <format>
#include <DxLib.h>
#include <cmath>
class Vector;

class VectorInt {
public:
	int m_x;
	int m_y;
	int m_z;
	VectorInt(int _x = 0, int _y = 0, int _z = 0) : m_x(_x), m_y(_y), m_z(_z) {};
	VectorInt operator+(const VectorInt& v) const
	{
		VectorInt newVec;
		newVec.m_x = m_x + v.m_x;
		newVec.m_y = m_y + v.m_y;
		newVec.m_z = m_z + v.m_z;
		return newVec;
	}
	VectorInt operator-(const VectorInt& v) const
	{
		VectorInt newVec;
		newVec.m_x = m_x - v.m_x;
		newVec.m_y = m_y - v.m_y;
		newVec.m_z = m_z - v.m_z;
		return newVec;
	}
	VectorInt operator-() const
	{
		VectorInt newVec;
		newVec.m_x = -m_x;
		newVec.m_y = -m_y;
		newVec.m_z = -m_z;
		return newVec;
	}
	VectorInt operator*(const int& v) const
	{
		VectorInt newVec{ m_x * v,m_y * v, m_z * v };
		return newVec;
	}
	VectorInt operator/(const int& v) const
	{
		if (v == 0) return { m_x,m_y,m_z };
		VectorInt newVec{ m_x / v, m_y / v , m_z / v };
		return newVec;
	}
	void operator+=(const VectorInt& v)
	{
		m_x += v.m_x;
		m_y += v.m_y;
		m_z += v.m_z;
	}
	void operator-=(const VectorInt& v)
	{
		m_x -= v.m_x;
		m_y -= v.m_y;
		m_z -= v.m_z;
	}
	void operator*=(const int& v)
	{
		m_x *= v;
		m_y *= v;
		m_z *= v;
	}
	bool operator==(const VectorInt& v) const
	{
		if (m_x != v.m_x) return false;
		if (m_y != v.m_y) return false;
		if (m_z != v.m_z) return false;
		return true;
	}

	Vector ToVector() const;
	
	std::string ToString() const
	{
		return std::format("({},{},{})", m_x, m_y, m_z);
	}
	VECTOR ToVECTOR()const
	{
		return VGet(static_cast<float>(m_x), static_cast<float>(m_y), static_cast<float>(m_z));
	}
	void FromVECTOR(const VECTOR& VEC)
	{
		m_x = static_cast<int>(VEC.x);
		m_y = static_cast<int>(VEC.y);
		m_z = static_cast<int>(VEC.z);
	}
	int Dot(const VectorInt& v) const
	{
		auto xTemp = m_x * v.m_x;
		auto yTemp = m_y * v.m_y;
		auto zTemp = m_z * v.m_z;
		return xTemp + yTemp + zTemp;
	}

	int SqDistance(const VectorInt& v) const
	{
		return (m_x - v.m_x) * (m_x - v.m_x) + (m_y - v.m_y) * (m_y - v.m_y) * (m_z - v.m_z) * (m_z - v.m_z);
	}
	float Distance(const VectorInt& v) const
	{
		return static_cast<float>(sqrt(SqDistance(v)));
	}
	int SqLength() const
	{
		return m_x * m_x + m_y * m_y + m_z * m_z;
	}
	float Length() const
	{
		return static_cast<float>(sqrt(SqLength()));
	}
	void Normalize()
	{
		int len = static_cast<int>(Length());
		if (len == 0) return;
		m_x /= len;
		m_y /= len;
		m_z /= len;
	}


	/// <summary>
	/// このベクトルをY軸を回転軸として回転させたベクトルを返す。(回転単位はラジアン。)
	/// </summary>
	VectorInt RotationY(float rad) const
	{
		VectorInt newVec
		{
			static_cast<int>(cosf(rad) * m_x + sinf(rad) * m_z),
			m_y,
			static_cast<int>(sinf(rad) * m_x + cosf(rad) * m_z)
		};
		return newVec;
	}

	/// <summary>
	/// Y軸を回転軸として回転させたベクトルにする。(回転単位はラジアン。)
	/// </summary>
	void RotateY(float rad)
	{
		VectorInt newVec
		{
			static_cast<int>(cosf(rad) * m_x + sinf(rad) * m_z),
			m_y,
			static_cast<int>(sinf(rad) * m_x + cosf(rad) * m_z)
		};
		m_x = newVec.m_x;
		m_z = newVec.m_z;
	}
};