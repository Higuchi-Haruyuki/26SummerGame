#include "UIImage.h"
#include "ResourceManager.h"
#include <DxLib.h>
#include "ResourceType.h"

namespace
{
	constexpr size_t kStartCapacity = 10;
}

UIImage::UIImage(const Vector& position, const Vector& size, GraphicId imageResourceName, int alpha):
	Base_UIElement(position, size),
	m_resourceManager(ResourceManager::GetInstance()),
	m_imageGraphicId(imageResourceName),
	m_alpha(alpha)
{
	m_onDragBeginConnections.reserve(kStartCapacity);
	m_onDragConnections.reserve(kStartCapacity);
	m_onDragEndConnections.reserve(kStartCapacity);
	m_onDropConnections.reserve(kStartCapacity);
	m_onClickConnections.reserve(kStartCapacity);
}

void UIImage::Draw() const
{
	if (!m_isVisible) return;

	float halfWidth = m_size.m_x * 0.5f;
	float halfHeight = m_size.m_y * 0.5f;

	if (m_alpha < 255) SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);

	int handle = m_resourceManager.GetHandle(m_imageGraphicId);

	// ハンドルが無効な場合は描画しない
	if (handle == -1) return;

	DrawExtendGraph(
		static_cast<int>(m_position.m_x - halfWidth),
		static_cast<int>(m_position.m_y - halfHeight),
		static_cast<int>(m_position.m_x + halfWidth),
		static_cast<int>(m_position.m_y + halfHeight),
		handle,
		true
	);

	if (m_alpha < 255) SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

bool UIImage::OnClick(const Vector& screenPos)
{
	m_onClick.Invoke();
	return true;
}

bool UIImage::OnDrop(const Vector& screenPos)
{
	m_onDrop.Invoke();
	return true;
}

bool UIImage::OnDragBegin(const Vector& screenPos)
{
	m_onDragBegin.Invoke();
	return true;
}

void UIImage::OnDrag(const Vector& screenPos, const Vector& delta)
{
	m_onDrag.Invoke(screenPos);
}

bool UIImage::OnDragEnd(const Vector& screenPos)
{
	m_onDragEnd.Invoke();
	return true;
}
