#pragma once
#include "vector.h"

/// <summary>
/// UIヒット用のマウス入力状態を保存するクラス
/// </summary>
class UIPointer
{
public:

	UIPointer() {};
	~UIPointer() {};

	void Update();

	Vector GetScreenPosition() const { return m_position; }

	/// <summary>
	/// 左クリックがこのプレームで押されたとき
	/// </summary>
	/// <returns></returns>
	bool IsLeftButtonTrigger() const;

	/// <summary>
	/// 左クリックが離されたとき
	/// </summary>
	/// <returns></returns>
	bool IsLeftButtonReleased() const;

	/// <summary>
	/// 左クリックが押しっぱなし状態
	/// </summary>
	/// <returns></returns>
	bool IsLeftButtonPressed() const;

	/// <summary>
	/// 左クリックがこのプレームで押されたとき
	/// </summary>
	/// <returns></returns>
	bool IsRightButtonTrigger() const;

	/// <summary>
	/// 左クリックが離されたとき
	/// </summary>
	/// <returns></returns>
	bool IsRightButtonReleased() const;

	/// <summary>
	/// 左クリックが押しっぱなし状態
	/// </summary>
	/// <returns></returns>
	bool IsRightButtonPressed() const;

private:

	/// <summary>
	/// 1フレーム前でクリックされていたか
	/// </summary>
	/// <returns></returns>
	bool WasPressedLeftButtonLast() const;

	/// <summary>
	/// このフレームでクリックされているか
	/// </summary>
	/// <returns></returns>
	bool IsPressedLeftButtonNow() const;

	/// <summary>
	/// 1フレーム前でクリックされていたか
	/// </summary>
	/// <returns></returns>
	bool WasPressedRightButtonLast() const;

	/// <summary>
	/// このフレームでクリックされているか
	/// </summary>
	/// <returns></returns>
	bool IsPressedRightButtonNow() const;

private:
	Vector m_position = {};

	/// <summary>
	/// このフレームのマウスの状態
	/// </summary>
	int m_nowMouseState = 0;

	/// <summary>
	/// 1フレーム前のマウスの状態
	/// </summary>
	int m_lastMouseState = 0;
};

