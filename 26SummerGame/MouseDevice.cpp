#include "MouseDevice.h"
#include <DxLib.h>
#include "Game.h"
#include"Debug.h"


namespace
{
    constexpr float kMouseSens = 80.0f;

    constexpr float kMouseWheelSens = 5.0f;
}

bool MouseDevice::GetButton(int code) const
{
    if (code == -1) return false;
    if (GetMouseInput() & code) return true;
    return false;
}

/// <summary>
/// マウスの画面サイズにおける移動量の割合を取得する。
/// </summary>
/// <param name="code"></param>
/// <returns> (移動量) / (画面サイズ) の演算結果</returns>
float MouseDevice::GetAxis(int code) const
{
    int x = 0, y = 0;

    GetMousePoint(&x, &y);

    auto diff = 0.0f;

    auto halfDisplay = Game::kDisplaySize / 2;


    if (code == MouseCode::kMouseMoveX)
    {
        diff = (x - halfDisplay.m_x) / Game::kDisplaySize.m_x;
    }
    else if(code == MouseCode::kMouseMoveY)
    {
        diff = (y - halfDisplay.m_y) / Game::kDisplaySize.m_y;
    }
    else if (code == MouseCode::kMouseWheelValue)
    {
        return GetMouseWheelRotVolF() * kMouseWheelSens;
    }
    else if (code == MouseCode::kMouseMoveXOnRightClick)
    {
        if (!GetButton(MOUSE_INPUT_RIGHT)) return 0.0f;
        diff = (x - halfDisplay.m_x) / Game::kDisplaySize.m_x;
    }
    else if (code == MouseCode::kMouseMoveYOnRightClick)
    {
        if (!GetButton(MOUSE_INPUT_RIGHT)) return 0.0f;
        diff = (y - halfDisplay.m_y) / Game::kDisplaySize.m_y;
    }
    else if (code == MouseCode::kMouseMoveXOnWheelClick)
    {
        if (!GetButton(MOUSE_INPUT_MIDDLE)) return 0.0f;

        if (!m_wasWheelClickPressed) return 0.0f;

        diff = (x - halfDisplay.m_x) / Game::kDisplaySize.m_x;
    }
    else if (code == MouseCode::kMouseMoveYOnWheelClick)
    {
        if (!GetButton(MOUSE_INPUT_MIDDLE)) return 0.0f;

        if (!m_wasWheelClickPressed) return 0.0f;

        diff = (y - halfDisplay.m_y) / Game::kDisplaySize.m_y;
    }
    return diff * kMouseSens;
}

bool MouseDevice::HasAnyInput() const
{
    if (GetMouseInput()) return true;
    int x = 0, y = 0;

    GetMousePoint(&x, &y);

    if (x != m_lastX || y != m_lastY) return true;

    return false;

}

void MouseDevice::MousePosReflesh()
{

    GetMousePoint(&m_lastX, &m_lastY);

    //このフレームで押したか
    const bool isWheelClickPressed = GetButton(MOUSE_INPUT_MIDDLE);

    if (isWheelClickPressed)
    {
        SetMousePoint(Game::kDisplaySize.m_x / 2, Game::kDisplaySize.m_y / 2);
        SetMouseDispFlag(false);
    }
    else
    {
        if(!GetMouseDispFlag()) SetMouseDispFlag(true);
    }

    m_wasWheelClickPressed = isWheelClickPressed;
}
