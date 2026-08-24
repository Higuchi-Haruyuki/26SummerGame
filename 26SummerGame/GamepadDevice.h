#pragma once
#include "InputDevice.h"


namespace GamepadCode
{
    constexpr int kXInputLeftStickX = 16;
    constexpr int kXInputLeftStickY = 17;
    constexpr int kXInputRightStickX = 18;
    constexpr int kXInputRightStickY = 19;
    constexpr int kXInputTriggerLeft = 20;
    constexpr int kXInputTriggerRight = 21;
    constexpr int kXInputTriggerAxis = 22;
}

class GamepadDevice :
    public IInputDevice
{
public:
    bool GetButton(int code) const override;
    float GetAxis(int code) const override;
    bool HasAnyInput() const override;
};

