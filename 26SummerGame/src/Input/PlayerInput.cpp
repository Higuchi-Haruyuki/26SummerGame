#include "PlayerInput.h"
#include "DxLib.h"
#include "InputAction.h"
#include "Debug.h"
#include <memory>
#include "KeyBoardDevice.h"
#include "GamepadDevice.h"
#include <format>
#include "InputValue.h"
#include <string>
#include <utility>
#include "MouseDevice.h"

void PlayerInput::Init()
{

	//<移動>
	AddActionKeyboardAndGamepad("MoveHorizontal", InputValueType::kAxis, KeyBoardCode::kMoveHorizontal, GamepadCode::kXInputLeftStickX);
	AddActionKeyboardAndGamepad("MoveVertical", InputValueType::kAxis, KeyBoardCode::kMoveVertical, GamepadCode::kXInputLeftStickY);

	//加速移動
	AddActionKeyboardAndGamepad("MoveAccel", InputValueType::kButton, KEY_INPUT_LSHIFT, XINPUT_BUTTON_LEFT_THUMB);


	//カメラ視点操作
	AddActionMouseAndGamepad("CameraYaw", InputValueType::kAxis, MouseCode::kMouseMoveXOnWheelClick, GamepadCode::kXInputRightStickX);
	AddActionMouseAndGamepad("CameraZoom", InputValueType::kAxis, MouseCode::kMouseWheelValue, GamepadCode::kXInputTriggerAxis);
	AddActionMouseAndGamepad("CameraPitch", InputValueType::kAxis, MouseCode::kMouseMoveYOnWheelClick, GamepadCode::kXInputRightStickY);


	//<操作>
	AddActionMouseAndGamepad("Decide", InputValueType::kButton, MOUSE_INPUT_LEFT, XINPUT_BUTTON_A);
	AddActionKeyboardAndGamepad("Cancel", InputValueType::kButton, KEY_INPUT_C, XINPUT_BUTTON_B);
	AddActionKeyboardAndGamepad("Cancel", InputValueType::kButton, KEY_INPUT_ESCAPE, -1);
	AddActionKeyboardAndGamepad("OpenInventory", InputValueType::kButton, KEY_INPUT_TAB, XINPUT_BUTTON_DPAD_UP);
	AddActionKeyboardAndGamepad("InstallationMode", InputValueType::kButton, KEY_INPUT_Z, XINPUT_BUTTON_Y);
	AddActionKeyboardAndGamepad("DestroyMode", InputValueType::kButton, KEY_INPUT_X, XINPUT_BUTTON_X);
	AddActionKeyboardAndGamepad("PreviewTurnRight", InputValueType::kButton, KEY_INPUT_R, XINPUT_BUTTON_LEFT_SHOULDER);

	//マウス操作
	AddActionMouseAndGamepad("LeftClick", InputValueType::kButton, MOUSE_INPUT_LEFT, -1);
	AddActionMouseAndGamepad("RightClick", InputValueType::kButton, MOUSE_INPUT_RIGHT, -1);

	//アイテムバーの選択
	AddActionKeyboardAndGamepad("ItemBar0", InputValueType::kButton, KEY_INPUT_1, -1);
	AddActionKeyboardAndGamepad("ItemBar1", InputValueType::kButton, KEY_INPUT_2, -1);
	AddActionKeyboardAndGamepad("ItemBar2", InputValueType::kButton, KEY_INPUT_3, -1);
	AddActionKeyboardAndGamepad("ItemBar3", InputValueType::kButton, KEY_INPUT_4, -1);
	AddActionKeyboardAndGamepad("ItemBar4", InputValueType::kButton, KEY_INPUT_5, -1);
	AddActionKeyboardAndGamepad("ItemBar5", InputValueType::kButton, KEY_INPUT_6, -1);
	AddActionKeyboardAndGamepad("ItemBar6", InputValueType::kButton, KEY_INPUT_7, -1);
	AddActionKeyboardAndGamepad("ItemBar7", InputValueType::kButton, KEY_INPUT_8, -1);
	AddActionKeyboardAndGamepad("ItemBar8", InputValueType::kButton, KEY_INPUT_9, -1);
	AddActionKeyboardAndGamepad("ItemBar9", InputValueType::kButton, KEY_INPUT_0, -1);
}

void PlayerInput::Update(float deltaTime)
{

	UpdateActiveDevice();
	for (const auto& [name, action] : m_actions)
	{
		action->Update();
	}

}

void PlayerInput::LateUpdate()
{
	m_mouseDevice.MousePosReflesh();
}

void PlayerInput::AddActionKeyboardAndGamepad(const std::string& actionName, InputValueType inputType, int keyboardCode, int gamepadCode)
{
	//すでに同じ名前のActionが存在する場合は、バインディングを追加するだけにする。
	if (IsExistAction(actionName))
	{
		GetAction(actionName)->AddBinding({ &m_keyboardDevice,keyboardCode });
		GetAction(actionName)->AddBinding({ &m_gamepadDevice ,gamepadCode });
		return;
	}

	auto moveLeft = std::make_unique<InputAction>(actionName, inputType);
	moveLeft->AddBinding({ &m_keyboardDevice,keyboardCode });
	moveLeft->AddBinding({ &m_gamepadDevice ,gamepadCode });
	m_actions.emplace(actionName, std::move(moveLeft));
}

void PlayerInput::AddActionMouseAndGamepad(const std::string& actionName, InputValueType inputType, int mouseCode, int gamepadCode)
{
	auto moveLeft = std::make_unique<InputAction>(actionName, inputType);
	moveLeft->AddBinding({ &m_mouseDevice,mouseCode });
	moveLeft->AddBinding({ &m_gamepadDevice ,gamepadCode });
	m_actions.emplace(actionName, std::move(moveLeft));
}

bool PlayerInput::IsExistAction(const std::string& name) const
{
	return m_actions.find(name) != m_actions.end();
}

void PlayerInput::UpdateActiveDevice()
{
	const bool gamepadHasInput = m_gamepadDevice.HasAnyInput();
	const bool keyboardHasInput = m_keyboardDevice.HasAnyInput();
	const bool mouseHasInput = m_mouseDevice.HasAnyInput();

	if (gamepadHasInput) 
		m_activeDeviceType = DeviceType::kGamepad;
	if (keyboardHasInput || mouseHasInput)
		m_activeDeviceType = DeviceType::kKeyboard;

}

