#pragma once
#include <memory>
#include "vector.h"
#include "Event.h"

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
	/// UI要素クリック時に呼ばれる。
	/// </summary>
	/// <param name="screenPos"></param>
	/// <returns></returns>
	virtual bool OnClick(const Vector& screenPos) 
	{ 
		m_onClick.Invoke();
		if (m_onClick.GetListerCount()) return true;
		return false; 
	}

	virtual bool OnRightClick(const Vector& screenPos) 
	{ 
		m_onRightClick.Invoke();
		if (m_onRightClick.GetListerCount()) return true;
		return false;
	}

	// ドラッグ操作。
	virtual bool OnDragBegin(const Vector& screenPos) 
	{
		m_onDragBegin.Invoke();
		if (m_onDragBegin.GetListerCount()) return true;
		return false;
	}

	// ドラッグ操作。
	virtual bool OnRightClickDragBegin(const Vector& screenPos)
	{
		m_onRightClickDragBegin.Invoke();
		if (m_onRightClickDragBegin.GetListerCount()) return true;
		return false;
	}

	virtual void OnDrag(const Vector& screenPos, const Vector& delta) 
	{
		m_onDrag.Invoke(screenPos);
	}

	virtual void OnRightClickDrag(const Vector& screenPos, const Vector& delta)
	{
		m_onRightClickDrag.Invoke(screenPos);
	}

	virtual bool OnDragEnd(const Vector& screenPos) 
	{ 
		m_onDragEnd.Invoke();
		if (m_onDragEnd.GetListerCount()) return true;
		return false;
	}

	virtual bool OnRightClickDragEnd(const Vector& screenPos)
	{
		m_onRightClickDragEnd.Invoke();
		if (m_onRightClickDragEnd.GetListerCount()) return true;
		return false;
	}
	
	/// <summary>
	///ドラッグ中のアイテムがこの要素にドロップされたとき
	///	</summary>
	virtual bool OnDrop(const Vector& screenPos) 
	{
		m_onDrop.Invoke();
		if (m_onDrop.GetListerCount()) return true;
		return false;
	}

	/// <summary>
	///ドラッグ中のアイテムがこの要素にドロップされたとき
	///	</summary>
	virtual bool OnRightClickDrop(const Vector& screenPos)
	{
		m_onRightClickDrop.Invoke();
		if (m_onRightClickDrop.GetListerCount()) return true;
		return false;
	}

	/*イベント購読関数*/
	/// <summary>
	/// 押され始めたときに呼び出される
	/// </summary>
	/// <param name="onBegin"></param>
	void SubscribeOnDragBegin(const std::function<void()>& onBegin)
	{
		m_onDragBeginConnections.emplace_back(m_onDragBegin.AddListener(onBegin));
	}

	void SubscribeOnRightClickDragBegin(const std::function<void()>& onBegin)
	{
		m_onRightClickDragBeginConnections.emplace_back(m_onRightClickDragBegin.AddListener(onBegin));
	}

	/// <summary>
	/// はなされたときによびだされる
	/// </summary>
	/// <param name="onBegin"></param>
	void SubscribeOnDrop(const std::function<void()>& onDrop)
	{
		m_onDropConnections.emplace_back(m_onDrop.AddListener(onDrop));
	}

	void SubscribeOnRightClickDrop(const std::function<void()>& onDrop)
	{
		m_onRightClickDropConnections.emplace_back(m_onRightClickDrop.AddListener(onDrop));
	}

	/// <summary>
	/// ドラッグ中に呼び出される
	/// </summary>
	/// <param name="onClick"></param>
	void SubscribeOnDrag(const std::function<void(const Vector& screenPos)>& onDrag)
	{
		m_onDragConnections.emplace_back(m_onDrag.AddListener(onDrag));
	}

	void SubscribeOnRightClickDrag(const std::function<void(const Vector& screenPos)>& onDrag)
	{
		m_onRightClickDragConnections.emplace_back(m_onRightClickDrag.AddListener(onDrag));
	}

	void SubscribeOnDragEnd(const std::function<void()>& onDragEnd)
	{
		m_onDragEndConnections.emplace_back(m_onDragEnd.AddListener(onDragEnd));
	}

	void SubscribeOnRightClickDragEnd(const std::function<void()>& onDragEnd)
	{
		m_onRightClickDragEndConnections.emplace_back(m_onRightClickDragEnd.AddListener(onDragEnd));
	}

	void SubscribeOnClick(const std::function<void()>& onClick)
	{
		m_onClickConnections.emplace_back(m_onClick.AddListener(onClick));
	}

	void SubscribeOnRightClick(const std::function<void()>& onClick)
	{
		m_onRightClickConnections.emplace_back(m_onRightClick.AddListener(onClick));
	}

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

	Event<> m_onDragBegin;
	std::vector<Event<>::Connection> m_onDragBeginConnections;

	Event<> m_onRightClickDragBegin;
	std::vector<Event<>::Connection> m_onRightClickDragBeginConnections;

	Event<const Vector&> m_onDrag;
	std::vector<Event<const Vector&>::Connection> m_onDragConnections;

	Event<const Vector&> m_onRightClickDrag;
	std::vector<Event<const Vector&>::Connection> m_onRightClickDragConnections;

	Event<> m_onDragEnd;
	std::vector<Event<>::Connection> m_onDragEndConnections;

	Event<> m_onRightClickDragEnd;
	std::vector<Event<>::Connection> m_onRightClickDragEndConnections;

	Event<> m_onDrop;
	std::vector<Event<>::Connection> m_onDropConnections;

	Event<> m_onRightClickDrop;
	std::vector<Event<>::Connection> m_onRightClickDropConnections;

	Event<> m_onClick;
	std::vector<Event<>::Connection> m_onClickConnections;

	Event<> m_onRightClick;
	std::vector<Event<>::Connection> m_onRightClickConnections;

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

