#pragma once
#include "Collider.h"
#include "vector.h"
#include "Object.h"

class SquareCollider3D : public Collider
{
public:
	SquareCollider3D(std::shared_ptr<Object> parentObject) : Collider(parentObject) {};

	void Draw(const Vector& cameraPos, const unsigned int& color) const override;

	// GETTER
	float GetWidth() const { return m_width; }
	float GetHeight() const { return m_height; }
	float GetDepth() const { return m_depth; }

	Vector GetMin() const
	{
		const auto& p = GetParentObject()->GetPosition();
		return Vector{ p.m_x - m_width / 2, p.m_y - m_height / 2, p.m_z - m_depth / 2 };
	}
	Vector GetMax() const
	{
		const auto& p = GetParentObject()->GetPosition();
		return Vector{ p.m_x + m_width / 2, p.m_y + m_height / 2, p.m_z + m_depth / 2 };
	}

	// SETTER
	void SetWidth(const float& width) { m_width = width; }
	void SetHeight(const float& height) { m_height = height; }
	void SetDepth(const float& depth) { m_depth = depth; }
	void SetSize(const Vector& size) override
	{
		m_width = size.m_x;
		m_height = size.m_y;
		m_depth = size.m_z;
	}
private:
	float m_width = 100.0f;
	float m_height = 10.0f;
	float m_depth = 10.0f;
};
