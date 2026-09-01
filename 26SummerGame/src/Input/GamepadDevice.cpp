#include "GamepadDevice.h"
#include <DxLib.h>
#include <cstdlib>


//struct XINPUT_STATE
//{
//    unsigned char	Buttons[16];	// ボタン１６個( 添字には XINPUT_BUTTON_DPAD_UP 等を使用する、
//    //			0:押されていない  1:押されている )
//    unsigned char	LeftTrigger;	// 左トリガー( 0～255 )
//    unsigned char	RightTrigger;	// 右トリガー( 0～255 )
//    short		ThumbLX;	// 左スティックの横軸値( -32768 ～ 32767 )
//    short		ThumbLY;	// 左スティックの縦軸値( -32768 ～ 32767 )
//    short		ThumbRX;	// 右スティックの横軸値( -32768 ～ 32767 )
//    short		ThumbRY;	// 右スティックの縦軸値( -32768 ～ 32767 )
//};
//
//// XInputボタン入力定義
//#define XINPUT_BUTTON_DPAD_UP		(0)	// デジタル方向ボタン上
//#define XINPUT_BUTTON_DPAD_DOWN		(1)	// デジタル方向ボタン下
//#define XINPUT_BUTTON_DPAD_LEFT		(2)	// デジタル方向ボタン左
//#define XINPUT_BUTTON_DPAD_RIGHT	(3)	// デジタル方向ボタン右
//#define XINPUT_BUTTON_START		(4)	// STARTボタン
//#define XINPUT_BUTTON_BACK		(5)	// BACKボタン
//#define XINPUT_BUTTON_LEFT_THUMB	(6)	// 左スティック押し込み
//#define XINPUT_BUTTON_RIGHT_THUMB	(7)	// 右スティック押し込み
//#define XINPUT_BUTTON_LEFT_SHOULDER	(8)	// LBボタン
//#define XINPUT_BUTTON_RIGHT_SHOULDER	(9)	// RBボタン
//#define XINPUT_BUTTON_A			(12)	// Aボタン
//#define XINPUT_BUTTON_B			(13)	// Bボタン
//#define XINPUT_BUTTON_X			(14)	// Xボタン
//#define XINPUT_BUTTON_Y			(15)	// Yボタン

namespace
{
	constexpr int kInputType = DX_INPUT_PAD1;

	constexpr float kStickDeadZone = 0.15f;
}


bool GamepadDevice::GetButton(int code) const
{
	if (code == -1) return false;

	XINPUT_STATE nowState;
	GetJoypadXInputState(kInputType, &nowState);
	return nowState.Buttons[code];
}

float GamepadDevice::GetAxis(int code) const
{
	if (code == -1) return false;

	XINPUT_STATE nowState;
	GetJoypadXInputState(kInputType, &nowState);

	float value = 0;

	switch (code)
	{
	case GamepadCode::kXInputLeftStickX:
		value = nowState.ThumbLX;
		value /= 32767;
		if (std::abs(value) < kStickDeadZone)
			return 0.0f;
		break;
	case GamepadCode::kXInputLeftStickY:
		value = nowState.ThumbLY;
		value /= 32767;
		if (std::abs(value) < kStickDeadZone)
			return 0.0f;
		break;
	case GamepadCode::kXInputRightStickX:
		value = nowState.ThumbRX;
		value /= 32767;
		if (std::abs(value) < kStickDeadZone)
			return 0.0f;
		break;
	case GamepadCode::kXInputRightStickY:
		value = -nowState.ThumbRY;
		value /= 32767;
		if (std::abs(value) < kStickDeadZone)
			return 0.0f;
		break;
	case GamepadCode::kXInputTriggerAxis:
		value = nowState.RightTrigger - nowState.LeftTrigger;
		value /= 255;
		break;
	default:
		break;
	}
	return value;
}

bool GamepadDevice::HasAnyInput() const
{
	XINPUT_STATE nowState;
	GetJoypadXInputState(kInputType, &nowState);

	for (const auto& button : nowState.Buttons)
	{
		if (button) return true;
	}

	if (nowState.LeftTrigger) return true;
	if (nowState.RightTrigger) return true;
	if (auto value = nowState.ThumbLX) 
	{
		value /= 32767;
		if (std::abs(value) > kStickDeadZone)
			return true;
	}
	if (auto value = nowState.ThumbLY) 
	{
		value /= 32767;
		if (std::abs(value) > kStickDeadZone)
			return true;
	}
	if (auto value = nowState.ThumbRX)
	{
		value /= 32767;
		if (std::abs(value) > kStickDeadZone)
			return true;
	}
	if (auto value = nowState.ThumbRY)
	{
		value /= 32767;
		if (std::abs(value) > kStickDeadZone)
			return true;
	}
	return false;
}
