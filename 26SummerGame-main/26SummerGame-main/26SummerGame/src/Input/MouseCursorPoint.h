#pragma once
#include <DxLib.h>
#include "vector.h"
#include <cassert>

namespace MouseCursorPoint
{
	inline Vector GetCurrentMousePos()
	{
		Vector result;
		int x, y;
		if (GetMousePoint(&x, &y) == -1)
		{
			assert(false && "マウスが刺さっていません");
			return result;
		}
		result.m_x = static_cast<float>(x);
		result.m_y = static_cast<float>(y);
		return result;
	}
}