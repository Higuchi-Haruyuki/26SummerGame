#pragma once
#include "InputDevice.h"


namespace KeyBoardCode
{
    constexpr int kMoveHorizontal = -2;
    constexpr int kMoveVertical = -3;
}

class KeyBoardDevice :
    public IInputDevice
{
public:
    bool GetButton(int code) const override;
    float GetAxis(int code) const override;
    bool HasAnyInput() const override;

};

