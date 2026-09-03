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
class UITextLabel;

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

	void SetVisible(bool visible);
	void SetLabelVisible(bool visible);
	
	/// <summary>
	/// グラフィックが設定されていないときにテキストラベルを見えなくするか
	/// </summary>
	/// <param name="visible"></param>
	void SetNotVisibleWhenNoGraphic(bool visible) { m_isNotVisibleWhenNoGraphic = visible; }
	void SetHitTarget(bool enable);
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

	void SetLabelText(const std::string& text);

	template<typename... Args>
	void SetLabelText(const std::format_string<Args...>& format, Args&&... args)
	{
		std::string text = std::vformat(format.get(), std::make_format_args(args...));
		SetLabelText(text);
	}

	void SetOnSelectItem(std::weak_ptr<ItemSlot> itemSlot, int index);

	void SetOnMoveItem(std::weak_ptr<ItemSlot> itemSlot, int index);

	void SetOnSelectHalfItem(std::weak_ptr<ItemSlot> itemSlot, int index);

	void SetOnMoveHalfItem(std::weak_ptr<ItemSlot> itemSlot, int index);

	void SetOnClickEvent(const std::function<void()>& onClick);
	void SetOnDragBeginEvent(const std::function<void()>& onDragBegin);
	void SetOnDropEvent(const std::function<void()>& onDragEnd);

	void SetOnRightClickEvent(const std::function<void()>& onClick);
	void SetOnRightDragBeginEvent(const std::function<void()>& onDragBegin);
	void SetOnRightDropEvent(const std::function<void()>& onDragEnd);

private:
	UIManager& m_uiManager;

	Item m_filterItem = Item::kNone;
	bool m_isEnableFilterItem = false;

	bool m_isNotVisibleWhenNoGraphic = true;

	std::weak_ptr<UIText> m_text;
	std::weak_ptr<UIImage> m_image;
	std::weak_ptr<UIImage> m_previewImage;
	std::weak_ptr<UISquare> m_square;
	
	//マウスオーバー時に表示されるテキストラベル
	std::shared_ptr<UITextLabel> m_textLabel;
};

