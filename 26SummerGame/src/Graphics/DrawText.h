#pragma once
#include <DxLib.h>
#include <string>
#include <format>
namespace text
{
	//指定座標を中心として描画する
	template <typename... Args>
	inline void DrawCenterFormatString(int x, int y,unsigned int color, const std::format_string<Args...>& fmt, Args&&... args)
	{
		std::string str = std::vformat(fmt.get(), std::make_format_args(args...));
		int length = static_cast<int>(str.length());
		int width = 0, height = 0;
		GetDrawStringSize(&width, &height, NULL, str.c_str(),length);
		DrawString(x - width / 2, y - height / 2, str.c_str(), color);
	}

	template<typename... Args>
	inline void DrawCenterFormatStringToHandle(int x, int y, unsigned int color, int fontHandle, const std::format_string<Args...>& fmt, Args&&... args)
	{
		std::string str = std::vformat(fmt.get(), std::make_format_args(args...));
		int length = static_cast<int>(str.length());
		int width = 0, height = 0;
		GetDrawStringSizeToHandle(&width, &height, NULL, str.c_str(), length,fontHandle);
		DrawStringToHandle(x - width / 2, y - height / 2, str.c_str(), color, fontHandle);
	}

	template<typename... Args>
	inline void DrawCenterFormatStringToHandleWithEdge
	(int x, int y, unsigned int color, unsigned int edgeColor,int fontHandle, const std::format_string<Args...>& fmt, Args&&... args)
	{
		std::string str = std::vformat(fmt.get(), std::make_format_args(args...));
		int length = static_cast<int>(str.length());
		int width = 0, height = 0;
		GetDrawStringSizeToHandle(&width, &height, NULL, str.c_str(), length, fontHandle);
		DrawStringToHandle(
			x - width / 2,
			y - height / 2, 
			str.c_str(), color, fontHandle,
			edgeColor);
	}

	//指定座標を中心上座標として描画する。
	template<typename... Args>
	inline void DrawCenterTopFormatStringToHandleWithEdge
	(int x, int y, unsigned int color, unsigned int edgeColor, int fontHandle, const std::format_string<Args...>& fmt, Args&&... args)
	{
		std::string str = std::vformat(fmt.get(), std::make_format_args(args...));
		int length = static_cast<int>(str.length());
		int width = 0, height = 0;
		GetDrawStringSizeToHandle(&width, &height, NULL, str.c_str(), length, fontHandle);
		DrawStringToHandle(
			x - width / 2,
			y,
			str.c_str(), color, fontHandle,
			edgeColor);
	}

	//指定座標を中心下座標として描画する。
	template<typename... Args>
	inline void DrawCenterBottomFormatStringToHandleWithEdge
	(int x, int y, unsigned int color, unsigned int edgeColor, int fontHandle, const std::format_string<Args...>& fmt, Args&&... args)
	{
		std::string str = std::vformat(fmt.get(), std::make_format_args(args...));
		int length = static_cast<int>(str.length());
		int width = 0, height = 0;
		GetDrawStringSizeToHandle(&width, &height, NULL, str.c_str(), length, fontHandle);
		DrawStringToHandle(
			x - width / 2,
			y - height,
			str.c_str(), color, fontHandle,
			edgeColor);
	}


	//指定座標を右下座標として描画する
	template<typename... Args>
	inline void DrawRightBottomFormatStringToHandleWithEdge
	(int x, int y, unsigned int color, unsigned int edgeColor, int fontHandle, const std::format_string<Args...>& fmt, Args&&... args)
	{
		std::string str = std::vformat(fmt.get(), std::make_format_args(args...));
		int length = static_cast<int>(str.length());
		int width = 0, height = 0;
		GetDrawStringSizeToHandle(&width, &height, NULL, str.c_str(), length, fontHandle);
		DrawStringToHandle(
			x - width,
			y - height,
			str.c_str(), color, fontHandle,
			edgeColor);
	}

	//指定座標を右端座標として描画する
	template<typename... Args>
	inline void DrawRightCenterFormatStringToHandleWithEdge
	(int x, int y, unsigned int color, unsigned int edgeColor, int fontHandle, const std::format_string<Args...>& fmt, Args&&... args)
	{
		std::string str = std::vformat(fmt.get(), std::make_format_args(args...));
		int length = static_cast<int>(str.length());
		int width = 0, height = 0;
		GetDrawStringSizeToHandle(&width, &height, NULL, str.c_str(), length, fontHandle);
		DrawStringToHandle(
			x - width,
			y - height / 2,
			str.c_str(), color, fontHandle,
			edgeColor);
	}

	//指定座標を左中央座標として描画する
	template<typename... Args>
	inline void DrawLeftCenterFormatStringToHandleWithEdge
	(int x, int y, unsigned int color, unsigned int edgeColor, int fontHandle, const std::format_string<Args...>& fmt, Args&&... args)
	{
		std::string str = std::vformat(fmt.get(), std::make_format_args(args...));
		int length = static_cast<int>(str.length());
		int width = 0, height = 0;
		GetDrawStringSizeToHandle(&width, &height, NULL, str.c_str(), length, fontHandle);
		DrawStringToHandle(
			x,
			y - height / 2,
			str.c_str(), color, fontHandle,
			edgeColor);
	}
}