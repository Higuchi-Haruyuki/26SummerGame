#include "UIPointer.h"
#include <DxLib.h>
#include <cassert>

void UIPointer::Update()
{
	int x, y;
	GetMousePoint(&x, &y);
	m_position = { static_cast<float>(x),static_cast<float>(y) };

	m_lastMouseState = m_nowMouseState;
	m_nowMouseState = GetMouseInput();
}

bool UIPointer::IsLeftButtonTrigger() const
{
	return !WasPressedLeftButtonLast() && IsPressedLeftButtonNow();
}

bool UIPointer::IsLeftButtonReleased() const
{
	return WasPressedLeftButtonLast() && !IsPressedLeftButtonNow();
}

bool UIPointer::IsLeftButtonPressed() const
{
	return IsPressedLeftButtonNow();
}

bool UIPointer::WasPressedLeftButtonLast() const
{
	assert(m_lastMouseState != -1 && "マウスの入力がありません");
	return m_lastMouseState & MOUSE_INPUT_LEFT;
}

bool UIPointer::IsPressedLeftButtonNow() const
{
	assert(m_nowMouseState != -1 && "マウスの入力がありません");
	return m_nowMouseState & MOUSE_INPUT_LEFT;
}
