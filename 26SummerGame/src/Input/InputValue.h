#pragma once

#include <variant>

#include "vector.h"
#include <cassert>

/// <summary>
/// アクションが返すあたいのしゅべつ
/// </summary>
enum class InputValueType
{
	kButton,
	kAxis,
};


/// <summary>
/// 統一入力値
/// </summary>
class InputValue
{
public:
	InputValue() : m_value(false) {}
	explicit InputValue(bool value) : m_value(value) {}
	explicit InputValue(float value) : m_value(value) {}

	// 型を誤って取得した場合はassertで検知する。
	bool AsButton() const 
	{ 
		const auto value = std::get_if<bool>(&m_value);
		assert(value);
		return *value;
	}
	float AsAxis() const 
	{ 
		const auto value = std::get_if<float>(&m_value);
		assert(value);
		return *value;
	}

private:
	std::variant<bool, float> m_value;
};
