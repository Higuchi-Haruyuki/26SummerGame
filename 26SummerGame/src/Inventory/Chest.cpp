#include "Chest.h"
#include "Game.h"
#include "Square3D.h"
#include "UIItemBox.h"
#include <cassert>
#include "ItemSlot.h"
#include "ItemStack.h"

namespace
{

	//アイテムの名前
	const Item kItemType = Item::kRockChest;

	const Vector kSize = { 200,200,200 };

	constexpr unsigned int kColor = 0xd3d3d3;

	constexpr int kItemBoxCountWidth = 13;
	constexpr int kItemBoxCountHeight = 7;

	constexpr int kMaxInputItemSlot = kItemBoxCountWidth * kItemBoxCountHeight;
	constexpr int kMaxOutputItemSlot = 1;

	//一秒間当たりどのくらいのアイテムを処理するか
	constexpr int kProcessItemPerSecond = 1;

	//UIの透明度
	constexpr int kUIAlpha = 180;

	const Vector kItemBoxStartPos = { Game::kDisplaySize.m_x * 0.305f , Game::kDisplaySize.m_y * 0.36f };

	constexpr float kItemBoxUIOffset = 10;

	const Vector kOutputUIPos = { Game::kDisplaySize.m_x / 3 * 2, Game::kDisplaySize.m_y / 2 };

	const Vector kFuelUIPos = { Game::kDisplaySize.m_x * 0.5f , Game::kDisplaySize.m_y * 0.6f };

	const Vector kUISize = { Game::kDisplaySize.m_x / 37, Game::kDisplaySize.m_x / 37 };
}

Chest::Chest(std::weak_ptr<Object> obj) :
	FactoryComponent(obj)
{}

Chest::~Chest()
{}

void Chest::Init()
{
	FactoryComponent::Init();

	m_shape = GetComponent<Square3D>();
	if (!m_shape.lock())
		m_shape = AddComponent<Square3D>();

	const auto& square3D = std::static_pointer_cast<Square3D>(m_shape.lock());
	square3D->SetUVScrollTexHandle(GraphicId::kRockChestIcon);
	square3D->SetUVScrollOffset(1);

	SetSizeAndColorAndMaxSlot(kSize, kColor, kMaxInputItemSlot, kMaxOutputItemSlot);
	SetItemName(kItemType);
}

void Chest::UpdateUIPanel()
{
	bool isSameSize = m_inputSlot->GetSlotCount() == m_itemBoxUIs.size();

	if (!isSameSize)
	{
		assert(false && "チェストのスロット数とUIのスロット数が異なります。");
		return;
	}

	for (int i = 0; i < m_inputSlot->GetSlotCount(); i++)
	{
		const auto& itemBox = m_itemBoxUIs.at(i);
		
		const auto& item = m_inputSlot->GetItem(i);

		if (!item)
		{
			itemBox->SetGraphicID(GraphicId::kNone);
			itemBox->SetText("");
		}
		else
		{
			itemBox->SetGraphicID(item->GetItemIconGraphicID());
			itemBox->SetText("x{}",item->GetItemCount());
		}
	}

}

void Chest::BuildUIPanel()
{
	auto pos = kItemBoxStartPos;

	int index = 0;
	for (int y= 0; y < kItemBoxCountHeight; y++)
	{
		for (int x = 0; x < kItemBoxCountWidth; x++)
		{
			const auto& itemBoxUI = std::make_shared<UIItemBox>(m_uiPanel, pos, kUISize);

			itemBoxUI->SetOnSelectItem(GetInputItemSlot(), index);
			itemBoxUI->SetOnMoveItem(GetInputItemSlot(), index);

			itemBoxUI->SetOnSelectHalfItem(GetInputItemSlot(), index);
			itemBoxUI->SetOnMoveHalfItem(GetInputItemSlot(), index);

			m_itemBoxUIs.push_back(itemBoxUI);

			pos += Vector{ kUISize.m_x } + Vector{ kItemBoxUIOffset };
			index++;
		}
		pos.m_x = kItemBoxStartPos.m_x;
		pos += Vector{ 0 , kUISize.m_y} + Vector{ 0 ,kItemBoxUIOffset };
	}
}
