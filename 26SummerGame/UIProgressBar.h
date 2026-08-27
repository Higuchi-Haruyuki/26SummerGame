#pragma once
#include "Base_UIElement.h"
#include <algorithm>

class UIProgressBar :
    public Base_UIElement
{
public:
	/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="position">位置</param>
		/// <param name="size">ここで指定した大きさに合わせて描画される。</param>
		/// <param name="imageResourceName">ResourceManagerで管理される画像リソースの名前</param>
		/// <param name="alpha">透明度 (0-255)</param>
	UIProgressBar(const Vector& position, const Vector& size, unsigned int color, int alpha = 255);
	virtual ~UIProgressBar() = default;

	void Draw() const override;

	void SetProgress(float progress) { m_progress = std::clamp(progress,0.0f,1.0f); }

	void SetProgress(float current, float min, float max)
	{
		auto m = max - min;
		auto c = current - min;

		SetProgress(c / m);
	}

private:

	float m_progress = 1.0f;

	unsigned int m_color = 0xFFFFFFFF; // 白色

	int m_alpha = 255; // 透明度(0-255)
};

