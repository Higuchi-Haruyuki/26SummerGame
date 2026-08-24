#pragma once
#include <memory>
#include "vector.h"

class PlayerInput;

class Base_UIElement : public std::enable_shared_from_this<Base_UIElement>
{
public:
	Base_UIElement(const Vector& position, const Vector& size) :
		m_position(position), m_size(size)
	{}

	virtual ~Base_UIElement() = default;

	virtual void Update(float deltaTime) {}

	virtual void Draw() const {}

	/// <summary>
	/// 指定したスクリーン座標がこの要素の矩形内にあるかどうかを判定する。
	/// </summary>
	/// <param name="screenPos"></param>
	/// <returns></returns>
	virtual bool HitTest(const Vector& screenPos) const
	{
		if (!m_isVisible) return false;
		if (!m_isHitTarget) return false;

		bool isInLeft = screenPos.m_x >= Left();
		bool isInRight = screenPos.m_x <= Right();
		bool isInTop = screenPos.m_y >= Top();
		bool isInBottom = screenPos.m_y <= Bottom();

		bool isInUIMouseCursor = isInLeft && isInRight && isInTop && isInBottom;

#if _DEBUG 
	#if false
		DrawBox(Left(), Top(), Right(), Bottom(), 0xff0000, false);
	#endif // true or false
#endif // _DEBUG


		return isInUIMouseCursor;
	}

	// ポインタ(マウス)イベント群。
	// 戻り値がtrueのとき「入力を消費した」ことを表し、それ以上他要素へ伝播させない。

	/// <summary>
	/// UI要素押下時に呼ばれる。
	/// </summary>
	/// <param name="screenPos"></param>
	/// <returns></returns>
	virtual bool OnPointerDown(const Vector& screenPos) { return false; }

	/// <summary>
	/// UI要素クリック時に呼ばれる。
	/// </summary>
	/// <param name="screenPos"></param>
	/// <returns></returns>
	virtual bool OnClick(const Vector& screenPos) { return false; }

	/// <summary>
	/// UI要素のクリックが終了したときに呼ばれる。
	/// </summary>
	/// <param name="screenPos"></param>
	/// <returns></returns>
	virtual bool OnPointerUp(const Vector& screenPos) { return false; }
	
	/// <summary>
	/// UI要素の上にマウスカーソルが置かれたとき
	/// </summary>
	virtual void OnPointerEnter() {}

	/// <summary>
	/// UI要素からマウスカーソルが離れたとき
	/// </summary>
	virtual void OnPointerExit() {}

	// ドラッグ操作。
	virtual bool OnDragBegin(const Vector& screenPos) { return false; }
	virtual void OnDrag(const Vector& screenPos, const Vector& delta) {}
	virtual bool OnDragEnd(const Vector& screenPos) { return false; }

	/// <summary>
	///ドラッグ中のアイテムがこの要素にドロップされたとき
	///	</summary>
	virtual bool OnDrop(const Vector& screenPos) { return false; }

	//GETTER
	bool GetIsVisible() const { return m_isVisible; }
	Vector GetPosition() const { return m_position; }
	Vector GetSize() const { return m_size; }
	bool IsHitTarget() const { return m_isHitTarget; }

	//SETTER
	virtual void SetVisible(bool isVisible) { m_isVisible = isVisible; }
	void SetPosition(const Vector& position) { m_position = position; }
	void SetSize(const Vector& size) { m_size = size; }
	void SetIsHitTarget(bool isHitTarget) { m_isHitTarget = isHitTarget; }

protected:
	float Left() const { return m_position.m_x - m_size.m_x * 0.5f; }
	float Right() const { return m_position.m_x + m_size.m_x * 0.5f; }
	float Top() const { return m_position.m_y - m_size.m_y * 0.5f; }
	float Bottom() const { return m_position.m_y + m_size.m_y * 0.5f; }

protected:
	/// <summary>
	/// 左上座標
	/// </summary>
	Vector m_position;

	/// <summary>
	/// 大きさ
	/// </summary>
	Vector m_size;

	/// <summary>
	/// falseのときDrawとHitTestが対象外になる
	/// </summary>
	bool m_isVisible = true;

	/// <summary>
	/// falseのときHitTestが対象外になる
	/// </summary>
	bool m_isHitTarget = true;
};

