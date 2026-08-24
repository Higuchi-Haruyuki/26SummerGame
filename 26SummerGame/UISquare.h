#pragma once
#include "Base_UIElement.h"
#include <string>
#include <algorithm>
#include <functional>
#include "Event.h"


class UISquare :
    public Base_UIElement
{
public:
	UISquare(const Vector& position, const Vector& size, unsigned int color, int alpha = 255);
	virtual ~UISquare() = default;

	void Draw() const override;

	bool OnClick(const Vector& screenPos) override;
	bool OnDrop(const Vector& screenPos) override;

	bool OnDragBegin(const Vector& screenPos) override;
	void OnDrag(const Vector& screenPos, const Vector& delta) override;
	bool OnDragEnd(const Vector& screenPos) override;

	//SETTER
	void SetColor(unsigned int color) { m_color = color; }
	void SetAlpha(int alpha) 
	{ 
		m_alpha = alpha; 
		m_alpha = std::clamp(m_alpha, 0, 255);
	}

	/// <summary>
		/// 押され始めたときに呼び出される
		/// </summary>
		/// <param name="onBegin"></param>
	void SubscribeOnDragBegin(const std::function<void()>& onBegin)
	{
		m_onDragBeginConnections.emplace_back(m_onDragBegin.AddListener(onBegin));
	}

	/// <summary>
	/// はなされたときによびだされる
	/// </summary>
	/// <param name="onBegin"></param>
	void SubscribeOnDrop(const std::function<void()>& onDrop)
	{
		m_onDropConnections.emplace_back(m_onDrop.AddListener(onDrop));
	}

	/// <summary>
	/// ドラッグ中に呼び出される
	/// </summary>
	/// <param name="onClick"></param>
	void SubscribeOnDrag(const std::function<void(const Vector& screenPos)>& onDrag)
	{
		m_onDragConnections.emplace_back(m_onDrag.AddListener(onDrag));
	}

	void SubscribeOnDragEnd(const std::function<void()>& onDragEnd)
	{
		m_onDragEndConnections.emplace_back(m_onDragEnd.AddListener(onDragEnd));
	}

	void SubscribeOnClick(const std::function<void()>& onClick)
	{
		m_onClickConnections.emplace_back(m_onClick.AddListener(onClick));
	}

private:

	unsigned int m_color = 0xFFFFFFFF; // 白色

	int m_alpha = 255; // 透明度(0-255)

	std::vector<Event<>::Connection> m_onDragBeginConnections;
	Event<> m_onDragBegin;

	std::vector<Event<const Vector&>::Connection> m_onDragConnections;
	Event<const Vector&> m_onDrag;

	std::vector<Event<>::Connection> m_onDragEndConnections;
	Event<> m_onDragEnd;

	std::vector<Event<>::Connection> m_onDropConnections;
	Event<> m_onDrop;

	std::vector<Event<>::Connection> m_onClickConnections;
	Event<> m_onClick;
};

