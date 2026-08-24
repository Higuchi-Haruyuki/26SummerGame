#pragma once
#include "InputDevice.h"
#include "InputValue.h"
#include <vector>
#include <memory>
#include <string>

enum class ButtonPhase
{
	kNone,	//押されていない
	kTrigger, //このフレームで押されたとき
	kPressed,	//押しっぱなし
	kReleased	//このフレームで離された瞬間
};

struct BindingSource
{
	IInputDevice* m_device;
	int m_code = 0;
};

class InputAction
{
public:
	InputAction(const std::string& name, InputValueType type) :
		m_name(name), m_valueType(type) {
	}

	void AddBinding(const BindingSource& source)
	{
		m_bindings.push_back(source);
	}

	void Update();

	const InputValue& GetValue() const { return m_currentValue; }

	ButtonPhase GetPhase() const;
private:
	std::string m_name;
	InputValueType m_valueType;

	//操作を保存する配列
	std::vector<BindingSource> m_bindings;

	InputValue m_currentValue;

	//今のボタンの状態
	bool m_currentButtonState = false;
	//前のフレームのボタンの状態
	bool m_previousButtonState = false;

};

