#pragma once
#include <map>
#include <concepts>

namespace ContainerUtil
{
	//+=演算子を持っているか
	template<typename T>
	concept HasPlusEqualOperator = requires(const T & a, const T & b)
	{
		{ a += b } -> std::same_as<T>;
	};

	template<typename K, typename V>
	inline std::map<K, V> MergeMap(const std::map<K, V>& left, const std::map<K, V>& right)
	{
		static_assert(HasPlusEqualOperator<V>);

		if (!right.size()) return left;
		if (!left.size()) return right;


		std::map<K, V> largeMap = (left.size() > right.size()) ? left : right;
		std::map<K, V> smallMap = (left.size() > right.size()) ? right : left;

		for (const auto& [key, value] : smallMap)
		{
			//すでにそのアイテムがキーとしてあるか
			auto it = largeMap.find(key);

			//ないなら新規追加
			if (it == largeMap.end())
			{
				largeMap.emplace(key, value);
			}

			//あるときは個数追加
			(*it).second += value;
		}

		return largeMap;
	}
}