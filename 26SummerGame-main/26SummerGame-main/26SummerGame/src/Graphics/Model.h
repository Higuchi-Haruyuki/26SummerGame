#pragma once
#include "Shape.h"
#include "ResourceType.h"

class Model : public Shape
{
public:
	Model(std::weak_ptr<Object> parentObject) :Shape(parentObject) {};
	~Model() {};

	void Init() override;
	void Update() override;
	void Finalize() override;
	void Draw() const override;

	//GETTER
	int GetModelHandle() const { return m_modelHandle; }

	//SETTER
	void SetModel(ModelId id);
	void SetScroll(int frameIndex, float scrollSpeedU)
	{
		m_frameIndex = frameIndex;
		m_scrollSpeedU = scrollSpeedU;
	}
	void SetEnableLighting(bool isEnable) { m_isEnableLighting = isEnable; }

	void TextureScroll();

private:

	bool m_isEnableLighting = true;

	int m_modelHandle = -1;

	//フレーム番号
	int m_frameIndex = -1;

	//U方向のスクロール速度
	float m_scrollSpeedU = 0;

	//現在のUオフセット値
	float m_currentOffsetU = 0;

};
