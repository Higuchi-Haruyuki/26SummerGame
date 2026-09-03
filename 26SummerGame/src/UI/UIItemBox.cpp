#include "UIItemBox.h"
#include "UIPanel.h"
#include "UIFactory.h"
#include "UIImage.h"
#include "UIText.h"
#include "UISquare.h"
#include "ResourceType.h"
#include "UIManager.h"
#include "Color.h"
#include "UITextLabel.h"

namespace
{
	constexpr unsigned int kColor = static_cast<unsigned int>(Color::kSubColor);
	constexpr int kAlpha = 150;
	constexpr int kPreviewAlpha = 100;

	constexpr unsigned int kTextMainColor = static_cast<unsigned int>(Color::kMainCharColor);
	constexpr unsigned int kTextEdgeColor = static_cast<unsigned int>(Color::kSubCharColor);
}

UIItemBox::UIItemBox(std::weak_ptr<UIPanel> addPanel, const Vector& position, const Vector& size) :
	m_uiManager(UIManager::GetInstance())
{
	m_square = UIFactory::MakeUIToPanel<UISquare>(addPanel, position, size, kColor, kAlpha);

	m_image = UIFactory::MakeUIToPanel<UIImage>(addPanel, position, size,GraphicId::kNone );

	m_previewImage = UIFactory::MakeUIToPanel<UIImage>(addPanel, position, size, GraphicId::kNone,kPreviewAlpha);

	m_textLabel = std::make_shared<UITextLabel>(addPanel, position - Vector{0,size.m_y * 0.5f + 17}, "アイテム名");

	const auto& sharedImage = m_image.lock();

	const auto& sharedPreviewImage = m_previewImage.lock();

	sharedPreviewImage->SetIsHitTarget(false);
	sharedPreviewImage->SetVisible(false);

	sharedImage->SubscribeOnDragBegin([sharedPreviewImage]()
		{
			sharedPreviewImage->SetVisible(true);
		});

	sharedImage->SubscribeOnDrag([sharedPreviewImage](const Vector& screenPos)
		{
			sharedPreviewImage->SetPosition(screenPos);
		});

	sharedImage->SubscribeOnDragEnd([sharedPreviewImage]()
		{
			sharedPreviewImage->SetVisible(false);
		});

	sharedImage->SubscribeOnRightClickDragBegin([sharedPreviewImage]()
		{
			sharedPreviewImage->SetVisible(true);
		});

	sharedImage->SubscribeOnRightClickDrag([sharedPreviewImage](const Vector& screenPos)
		{
			sharedPreviewImage->SetPosition(screenPos);
		});

	sharedImage->SubscribeOnRightClickDragEnd([sharedPreviewImage]()
		{
			sharedPreviewImage->SetVisible(false);
		});
	sharedImage->SubscribeOnHover([this]()
		{
			if (!m_isNotVisibleWhenNoGraphic)
			{
				SetLabelVisible(true);
				return;
			}
			const auto& safeImage = m_image.lock();
			if (!safeImage) return;
			if (safeImage->GetGraphicID() != GraphicId::kNone)
				SetLabelVisible(true);
		});

	auto textPos = position + size / 2;

	m_text = UIFactory::MakeUIToPanel<UIText>(addPanel,
		TextArgs{
				textPos,
				TextPivot::RightBottom,
				FontId::kFactoryUITextMini,
				"x0",
				kTextMainColor,
				kTextEdgeColor,
				255
		});
}

void UIItemBox::SetVisible(bool isVisible)
{
	m_square.lock()->SetVisible(isVisible);
	m_image.lock()->SetVisible(isVisible);
	m_text.lock()->SetVisible(isVisible);
	if (!isVisible) m_previewImage.lock()->SetVisible(false);
}

void UIItemBox::SetLabelVisible(bool visible)
{
	m_textLabel->SetVisible(visible);
}

void UIItemBox::SetHitTarget(bool isEnable)
{
	m_square.lock()->SetIsHitTarget(isEnable);
	m_image.lock()->SetIsHitTarget(isEnable);
	m_text.lock()->SetIsHitTarget(isEnable);
}

void UIItemBox::SetPosition(const Vector & pos)
{
	m_square.lock()->SetPosition(pos);
	m_image.lock()->SetPosition(pos);
	m_text.lock()->SetPosition(pos);
}

void UIItemBox::SetSize(const Vector& size)
{
	m_square.lock()->SetSize(size);
	m_image.lock()->SetSize(size);
}

void UIItemBox::SetColor(unsigned int color)
{
	m_square.lock()->SetColor(color);
}

void UIItemBox::SetDefaultColor()
{
	SetColor(kColor);
}

void UIItemBox::SetGraphicID(GraphicId graphicId)
{
	m_image.lock()->SetGraphicID(graphicId);
	m_previewImage.lock()->SetGraphicID(graphicId);
}

void UIItemBox::SetImageAlpha(int alpha)
{
	m_image.lock()->SetAlpha(alpha);
}

void UIItemBox::SetText(const std::string & text)
{
	m_text.lock()->SetText(text);
}

void UIItemBox::SetLabelText(const std::string& text)
{
	m_textLabel->SetText(text);
}

void UIItemBox::SetOnSelectItem(std::weak_ptr<ItemSlot> itemSlot, int index)
{
	SetOnDragBeginEvent(
		[this, itemSlot,index]()
		{
			m_uiManager.ItemSelect(itemSlot, index);
		}
	);
}

void UIItemBox::SetOnMoveItem(std::weak_ptr<ItemSlot> itemSlot, int index)
{

	SetOnDropEvent(
		[this, itemSlot, index]()
		{
			if (m_isEnableFilterItem)
			{
				if (m_filterItem != m_uiManager.GetSelectedItemType()) return;
			
			}
			m_uiManager.MoveItem(itemSlot, index);
		}
	);
}

void UIItemBox::SetOnSelectHalfItem(std::weak_ptr<ItemSlot> itemSlot, int index)
{
	SetOnRightDragBeginEvent(
		[this, itemSlot, index]()
		{
			m_uiManager.SelectHalfItem(itemSlot, index);
		}
	);
}

void UIItemBox::SetOnMoveHalfItem(std::weak_ptr<ItemSlot> itemSlot, int index)
{
	SetOnRightDropEvent(
		[this, itemSlot, index]()
		{
			if (m_isEnableFilterItem)
			{
				if (m_filterItem != m_uiManager.GetSelectedHalfItemType()) return;
			}
			m_uiManager.MoveHalfItem(itemSlot, index);
		}
	);
}

void UIItemBox::SetOnClickEvent(const std::function<void()>& onClick)
{
	m_square.lock()->SubscribeOnClick(onClick);
	m_image.lock()->SubscribeOnClick(onClick);

}

void UIItemBox::SetOnDragBeginEvent(const std::function<void()>& onDragBegin)
{
	m_image.lock()->SubscribeOnDragBegin(onDragBegin);
}

void UIItemBox::SetOnDropEvent(const std::function<void()>& onDrop)
{
	m_square.lock()->SubscribeOnDrop(onDrop);
	m_image.lock()->SubscribeOnDrop(onDrop);
}

void UIItemBox::SetOnRightClickEvent(const std::function<void()>& onClick)
{
	m_square.lock()->SubscribeOnRightClick(onClick);
	m_image.lock()->SubscribeOnRightClick(onClick);
}

void UIItemBox::SetOnRightDragBeginEvent(const std::function<void()>&onDragBegin)
{
	m_image.lock()->SubscribeOnRightClickDragBegin(onDragBegin);
}

void UIItemBox::SetOnRightDropEvent(const std::function<void()>&onDrop)
{
	m_square.lock()->SubscribeOnRightClickDrop(onDrop);
	m_image.lock()->SubscribeOnRightClickDrop(onDrop);
}
