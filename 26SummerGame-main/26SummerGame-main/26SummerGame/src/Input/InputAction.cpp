#include "InputAction.h"
#include <cstdlib>
#include "InputValue.h"

void InputAction::Update()
{
	m_previousButtonState = m_currentButtonState;

	switch (m_valueType)
	{
	case InputValueType::kButton:
	{
		//複数バインディングのいずれかが押されていたらtrue
		bool pressed = false;
		for (const auto& binding : m_bindings)
		{
			if (binding.m_device->GetButton(binding.m_code))
			{
				pressed = true;
				break;
			}
		}
		m_currentButtonState = pressed;
		m_currentValue = InputValue{ pressed };
		break;
	}

	case InputValueType::kAxis:
	{
		//複数バインディングのうち絶対値が最大のものを採用する。
		float value = 0.0f;
		for (const BindingSource& binding : m_bindings) {
			const float axisValue = binding.m_device->GetAxis(binding.m_code);
			if (std::abs(axisValue) > std::abs(value)) value = axisValue;
		}
		m_currentValue = InputValue{ value };
		break;
	}

	default:
	{
		break;
	}
	}
}

ButtonPhase InputAction::GetPhase() const
{
	if (m_currentButtonState && !m_previousButtonState) return ButtonPhase::kTrigger;
	if (m_currentButtonState && m_previousButtonState) return ButtonPhase::kPressed;
	if (!m_currentButtonState && m_previousButtonState) return ButtonPhase::kReleased;
	return ButtonPhase::kNone;
}
