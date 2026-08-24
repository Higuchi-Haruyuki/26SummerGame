#include "SquareCollider3D.h"
#include <DxLib.h>
#include <cmath>
void SquareCollider3D::Draw(const Vector& cameraPos, const unsigned int& color) const
{
	if (!m_isEnable) return;
	if (!GetParentObject()) return;
	if (!GetParentObject()->GetIsEnable()) return;
	if (!m_isVisible) return;

	auto position = GetParentObject()->GetPosition() + m_offset;

	float width = cosf(GetRotationAngle()) * m_width + sinf(GetRotationAngle()) * m_depth;
	float depth = sinf(GetRotationAngle()) * m_width + cosf(GetRotationAngle()) * m_depth;



	//一番上の面の描画
	Vector p1 = { position.m_x - width / 2, position.m_y + m_height / 2,position.m_z - depth / 2 };
	Vector p2 = { position.m_x - width / 2, position.m_y + m_height / 2,position.m_z + depth / 2 };
	DrawLine3D(p1.ToVECTOR(), p2.ToVECTOR(), 0x00ffff);

	Vector p3 = { position.m_x + width / 2, position.m_y + m_height / 2,position.m_z - depth / 2 };
	Vector p4 = { position.m_x + width / 2, position.m_y + m_height / 2,position.m_z + depth / 2 };
	DrawLine3D(p3.ToVECTOR(), p4.ToVECTOR(), 0x00ffff);
	
	DrawLine3D(p1.ToVECTOR(), p3.ToVECTOR(), 0x00ffff);
	DrawLine3D(p2.ToVECTOR(), p4.ToVECTOR(), 0x00ffff);

	//一番下の面の描画
	Vector p5 = { position.m_x - width / 2, position.m_y - m_height / 2,position.m_z - depth / 2 };
	Vector p6 = { position.m_x - width / 2, position.m_y - m_height / 2,position.m_z + depth / 2 };
	DrawLine3D(p5.ToVECTOR(), p6.ToVECTOR(), 0x00ffff);

	Vector p7 = { position.m_x + width / 2, position.m_y - m_height / 2,position.m_z - depth / 2 };
	Vector p8 = { position.m_x + width / 2, position.m_y - m_height / 2,position.m_z + depth / 2 };
	DrawLine3D(p7.ToVECTOR(), p8.ToVECTOR(), 0x00ffff);

	DrawLine3D(p5.ToVECTOR(), p7.ToVECTOR(), 0x00ffff);
	DrawLine3D(p6.ToVECTOR(), p8.ToVECTOR(), 0x00ffff);

	//Y軸に平行な線の描画
	DrawLine3D(p1.ToVECTOR(), p5.ToVECTOR(), 0x00ffff);
	DrawLine3D(p2.ToVECTOR(), p6.ToVECTOR(), 0x00ffff);
	DrawLine3D(p3.ToVECTOR(), p7.ToVECTOR(), 0x00ffff);
	DrawLine3D(p4.ToVECTOR(), p8.ToVECTOR(), 0x00ffff);
}
