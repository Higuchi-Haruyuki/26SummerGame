#include "UIItemBox.h"
#include "UIPanel.h"
#include "UIFactory.h"
#include "UIImage.h"
#include "UIText.h"
#include "UISquare.h"
#include "ResourceType.h"
#include "UIManager.h"

namespace
{
	constexpr unsigned int kColor = 0x808080;
	constexpr int kAlpha = 150;
	constexpr int kPreviewAlpha = 100;
}

UIItemBox::UIItemBox(std::weak_ptr<UIPanel> addPanel, const Vector& position, const Vector& size) :
	m_uiManager(UIManager::GetInstance())
{
	m_square = UIFactory::MakeUIToPanel<UISquare>(addPanel, position, size, kColor, kAlpha);

	m_image = UIFactory::MakeUIToPanel<UIImage>(addPanel, position, size,GraphicId::kNone );

	m_previewImage = UIFactory::MakeUIToPanel<UIImage>(addPanel, position, size, GraphicId::kNone,kPreviewAlpha);

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


	auto textPos = position + size / 2;

	m_text = UIFactory::MakeUIToPanel<UIText>(addPanel,
		TextArgs{
				textPos,
				TextPivot::RightBottom,
				FontId::kFactoryUITextMini,
				"x0",
				0xffffff,
				0x000000,
				255
		});
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
