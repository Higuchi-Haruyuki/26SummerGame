#pragma once
#include <memory>
#include "Shape.h"
#include "Object.h"
#include "vector.h"
#include "ResourceType.h"

class Square3D : public Shape
{
public:
	Square3D(std::shared_ptr<Object> parentObject) :Shape(parentObject) {};
	~Square3D() {};
	void Init() override;
	void Update(float deltaTime) override;
	void Finalize() override;
	void Draw(const Vector& cameraPos) const override;

	// GETTER
	Vector GetSize() const { return Vector{ m_width, m_height, m_depth }; }
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
	void SetSize(const Vector& size) override
	{
		m_width = size.m_x * m_scale.m_x;
		m_height = size.m_y * m_scale.m_y;
		m_depth = size.m_z * m_scale.m_z;
	}
	void SetWidth(const float& width) { m_width = width * m_scale.m_x; }
	void SetHeight(const float& height) { m_height = height * m_scale.m_y; }
	void SetDepth(const float& depth) { m_depth = depth * m_scale.m_z; }
	void SetUVScrollOffset(float offset)
	{
		m_uvScrollOffset = offset;
	}

	void SetUVScrollTexHandle(GraphicId id);


private:
	float m_width = 100.0f;
	float m_height = 10.0f;
	float m_depth = 10.0f;

	//UVスクロールオフセット
	float m_uvScrollOffset = 0.0f;

	//UVスクロールするテクスチャのハンドル
	int m_uvScrollTexHandle = -1;
};
