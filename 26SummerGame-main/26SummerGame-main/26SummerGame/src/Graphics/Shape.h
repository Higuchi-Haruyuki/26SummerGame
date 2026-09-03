#pragma once
#include "Component.h"
#include "vector.h"
class Shape : public Component
{
public:
	Shape(std::weak_ptr<Object> parentObject) :Component(parentObject) { m_isVisible = true; };
	virtual ~Shape() = default;
	void Init() override;
	void Update() override;
	void Finalize() override;
	virtual void Draw() const;

	//GETTER
	Vector GetScale() const { return m_scale; }
	bool GetIsVisible() const { return m_isVisible; }

	//SETTER
	void SetScale(const Vector& scale) { m_scale = scale; }

	void IsVisible(bool isVisible) { m_isVisible = isVisible; }

	/// <summary>
	/// Y軸を軸とした回転角を設定する。単位はラジアン。
	/// </summary>
	/// <param name="rad"></param>
	void SetRotationAngle(float rad) { m_rotationAngle = rad; }

	virtual void SetSize(const Vector& siz) {}

	void SetColor(unsigned int col) { m_color = col; }

	/// <summary>
	/// 不透明度を設定。最大値は255。
	/// </summary>
	/// <param name="alpha"></param>
	void SetAlpha(int alpha) { m_alpha = alpha; }
protected:
	bool m_isVisible = true;

	/// <summary>
	/// サイズに対して何倍するかの値
	/// </summary>
	Vector m_scale = { 1,1,1 };

	/// <summary>
	/// 回転角度
	/// </summary>
	float m_rotationAngle = 0.0f;

	unsigned int m_color = 0;

	int m_alpha = 255;
};