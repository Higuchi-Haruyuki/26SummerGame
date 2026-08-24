#pragma once
#include <charconv>
#include <string>
#include <system_error>
#include <type_traits>

namespace StringUtil
{
	template<typename T>
	std::string IntToString(T value)
	{
		static_assert(std::is_integral_v<T>, "IntToStringは整数型を要求します。");

		char buffer[32];

		const auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);

		return std::string(buffer, ptr);
	}
}
