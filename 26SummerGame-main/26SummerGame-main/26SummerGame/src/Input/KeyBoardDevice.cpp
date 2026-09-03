#include "KeyBoardDevice.h"
#include <DxLib.h>

bool KeyBoardDevice::GetButton(int code) const
{
    if (code == -1) return false;

    return CheckHitKey(code);
}

float KeyBoardDevice::GetAxis(int code) const
{
    if (code == -1) return false;

    if (code == KeyBoardCode::kMoveHorizontal)
    {
        float value = 0.0f;
        if (GetButton(KEY_INPUT_A)) value += -1.0f;
        if (GetButton(KEY_INPUT_D)) value += 1.0f;
        return value;
    }
    else if (code == KeyBoardCode::kMoveVertical)
    {
        float value = 0.0f;
        if (GetButton(KEY_INPUT_W)) value += 1.0f;
        if (GetButton(KEY_INPUT_S)) value += -1.0f;
        return value;
    }

    return GetButton(code) ? 1.0f : 0.0f;
}

bool KeyBoardDevice::HasAnyInput() const
{
    char allState[256] = {};
    GetHitKeyStateAll(allState);
    for (const char& key : allState)
    {
        if (key) 
          return true;
    }
    return false;
}
