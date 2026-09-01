#pragma once
#include "vector.h"
#include <DxLib.h>
#include "Debug.h"

#include "GamepadDevice.h"
#include "InputAction.h"
#include "KeyBoardDevice.h"
#include "MouseDevice.h"
#include <memory>
#include <string>
#include <unordered_map>

enum class DeviceType{ kKeyboard, kGamepad};


class PlayerInput final
{
public:
	/// <summary>
	/// シングルトンインスタンスを取得
	/// </summary>
	static PlayerInput& GetInstance()
	{
		static PlayerInput instance;
		return instance;
	}

	void Init();


	// 更新
	void Update(float deltaTime);

	void LateUpdate();

	InputAction* GetAction(const std::string& name)
	{
		const auto it = m_actions.find(name);
		const auto inputAction = (it != m_actions.end()) ? it->second.get() : nullptr;
		assert(inputAction && "その名前のInputActionはありません。");
		return inputAction;
	}

	/// <summary>
	/// 現在アクティブなデバイスを取得する。
	/// </summary>
	/// <returns></returns>
	DeviceType GetActiveDeviceType() const { return m_activeDeviceType; }

	/// <summary>
	/// キーボードとコントローラーのInputActionを追加する。
	/// </summary>
	/// <param name="actionName">Actionの名前</param>
	/// <param name="inputType">入力方式</param>
	/// <param name="keyboardCode">キーボードの入力コード(-1で無効)</param>
	/// <param name="gamepadCode">コントローラーの入力コード(-1で無効)</param>
	void AddActionKeyboardAndGamepad(const std::string& actionName, InputValueType inputType ,int keyboardCode, int gamepadCode);
	
	/// <summary>
	/// マウスとコントローラーのInputActionを追加する。
	/// </summary>
	/// <param name="actionName">Actionの名前</param>
	/// <param name="inputType">入力方式</param>
	/// <param name="mouseCode">マウスの入力コード(-1で無効)</param>
	/// <param name="gamepadCode">コントローラーの入力コード(-1で無効)</param>
	void AddActionMouseAndGamepad(const std::string& actionName, InputValueType inputType, int mouseCode, int gamepadCode);

private:
	//シングルトンの準備
	PlayerInput() = default;
	~PlayerInput() = default;
	//コピー禁止
	PlayerInput(const PlayerInput&) = delete;
	PlayerInput& operator = (const PlayerInput&) = delete;
	//ムーブ禁止
	PlayerInput(PlayerInput&&) = delete;
	PlayerInput& operator = (PlayerInput&&) = delete;

	void UpdateActiveDevice();

private:

	//キーボードデバイス
	KeyBoardDevice m_keyboardDevice;
	//コントローラーデバイス
	GamepadDevice m_gamepadDevice;
	//マウスデバイス
	MouseDevice m_mouseDevice;

	//現在有効なデバイス
	DeviceType m_activeDeviceType = DeviceType::kGamepad;

	//inputActionと名前の結び付け
	std::unordered_map<std::string, std::unique_ptr<InputAction>> m_actions;

};
