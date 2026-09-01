#pragma once
#include "InputDevice.h"

namespace MouseCode
{
    //マウスのX方向の移動量を取得
    constexpr int kMouseMoveX = 0;
    //マウスのY方向の移動量を取得
    constexpr int kMouseMoveY = 1;

    constexpr int kMouseWheelValue = 2;

    constexpr int kMouseMoveXOnRightClick = 3;

    constexpr int kMouseMoveYOnRightClick = 4;

    constexpr int kMouseMoveXOnWheelClick = 5;

    constexpr int kMouseMoveYOnWheelClick = 6;
}

class MouseDevice :
    public IInputDevice
{
public:
    bool GetButton(int code) const override;

    /// <summary>
    /// マウスの移動方向と移動量を取得する。
    /// </summary>
    /// <param name="code">名前空間MouseCodeを参照</param>
    /// <returns></returns>
    float GetAxis(int code) const override;
    bool HasAnyInput() const override;

    //マウスの移動量取得用
    void MousePosReflesh();
private:
    int m_lastX = 0;
    int m_lastY = 0;

    // 中ボタンが「前フレーム時点で」押されていたかどうか。
    // 押下エッジの1フレームだけ視点移動量を無効化するために使用する。
    bool m_wasWheelClickPressed = false;
};

