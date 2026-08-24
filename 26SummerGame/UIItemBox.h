#pragma once
#include <string>
#include <format>
#include <memory>
#include <functional>
#include "ItemType.h"

class Vector;
class ItemSlot;
class UIPanel;
class UIText;
class UIImage;
class UISquare;
class UIManager;

struct TextArgs;

enum class GraphicId;

/// <summary>
/// アイテム欄UIをまとめて管理できるクラス
/// </summary>
class UIItemBox
{
public:
	UIItemBox(std::weak_ptr<UIPanel> addPanel, const Vector& position, const Vector& size);
	~UIItemBox() = default;

	void SetPosition(const Vector& pos);
	void SetSize(const Vector& size);
	void SetColor(unsigned int color);
	void SetDefaultColor();
	void SetGraphicID(GraphicId graphicId);
	void SetImageAlpha(int alpha);
	void SetFilterItem(Item item) { m_filterItem = item; }
	void SetEnableFilterItem(bool filter) { m_isEnableFilterItem = filter; }

	void SetText(const std::string& text);

	template <typename... Args>
	void SetText(const std::format_string<Args...>& format, Args&&... args)
	{
		std::string text = std::vformat(format.get(), std::make_format_args(args...));
		SetText(text);
	}

	void SetOnSelectItem(std::weak_ptr<ItemSlot> itemSlot, int index);

	void SetOnMoveItem(std::weak_ptr<ItemSlot> itemSlot, int index);

	void SetOnClickEvent(const std::function<void()>& onClick);
	void SetOnDragBeginEvent(const std::function<void()>& onDragBegin);
	void SetOnDropEvent(const std::function<void()>& onDragEnd);

private:
	UIManager& m_uiManager;

	Item m_filterItem;
	bool m_isEnableFilterItem = false;

	std::weak_ptr<UIText> m_text;
	std::weak_ptr<UIImage> m_image;
	std::weak_ptr<UIImage> m_previewImage;
	std::weak_ptr<UISquare> m_square;
};

