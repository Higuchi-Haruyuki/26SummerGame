#include "Square3D.h"
#include <DxLib.h>
#include "Shape.h"
#include "vector.h"
#include"SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"
#include <cmath>
#include <memory>
#include <Windows.h>
void Square3D::Init() {
	Shape::Init();
	//はみ出しても繰り返すようにWRAPモードにする。
	SetTextureAddressMode(DX_TEXADDRESS_WRAP);
}

void Square3D::Update() {
	Shape::Update();
	if (!m_isEnable) return;
}

void Square3D::Finalize() {
	Shape::Finalize();
}

void Square3D::Draw() const {
	if (!m_isEnable) return;
	if (!GetParentObject().lock()) return;
	//if (!SceneManager::GetInstance().GetCurrentScene()->HasSceneShapes(std::dynamic_pointer_cast<Shape>(shared_from_this()))) return;
	if (!GetParentObject().lock()->GetIsEnable()) return;
	if (!m_isVisible) return;

	auto position = GetPosition();

	float width = cosf(m_rotationAngle) * m_width + sinf(m_rotationAngle) * m_depth;
	float depth = sinf(m_rotationAngle) * m_width + cosf(m_rotationAngle) * m_depth;

	// 8頂点を計算
	Vector p1 = { position.m_x - width / 2, position.m_y + m_height / 2, position.m_z - depth / 2 };
	Vector p2 = { position.m_x - width / 2, position.m_y + m_height / 2, position.m_z + depth / 2 };
	Vector p3 = { position.m_x + width / 2, position.m_y + m_height / 2, position.m_z - depth / 2 };
	Vector p4 = { position.m_x + width / 2, position.m_y + m_height / 2, position.m_z + depth / 2 };

	Vector p5 = { position.m_x - width / 2, position.m_y - m_height / 2, position.m_z - depth / 2 };
	Vector p6 = { position.m_x - width / 2, position.m_y - m_height / 2, position.m_z + depth / 2 };
	Vector p7 = { position.m_x + width / 2, position.m_y - m_height / 2, position.m_z - depth / 2 };
	Vector p8 = { position.m_x + width / 2, position.m_y - m_height / 2, position.m_z + depth / 2 };

	//不透明でない時だけブレンドモードを設定する関数を呼び出す。
	if (m_alpha != 255)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA_X4, m_alpha);
	}

	// 上面(スクロールしたいから特殊処理する)
	if (m_uvScrollTexHandle != -1)
	{
		// --- ローカル空間(未回転)での頂点オフセットを先に作る ---
		const float halfWidth = m_width / 2.0f;
		const float halfDepth = m_depth / 2.0f;

		VECTOR localOffset[4] =
		{
			VGet(-halfWidth, 0.0f, -halfDepth), // 左手前
			VGet(halfWidth, 0.0f, -halfDepth), // 右手前
			VGet(halfWidth, 0.0f,  halfDepth), // 右奥
			VGet(-halfWidth, 0.0f,  halfDepth), // 左奥
		};

		// --- Y軸回転を各オフセットに適用する ---
		// m_rotationAngle は設置時の向き(BeltDirection)から算出した角度(ラジアン)
		const float cosA = cosf(-m_rotationAngle);
		const float sinA = sinf(-m_rotationAngle);

		VERTEX3D v[4];
		for (int i = 0; i < 4; i++)
		{
			// ローカルXZをY軸回転させる(標準的な2D回転行列)
			const float rotatedX = localOffset[i].x * cosA - localOffset[i].z * sinA;
			const float rotatedZ = localOffset[i].x * sinA + localOffset[i].z * cosA;

			// 回転後のオフセットをワールド中心座標に加算
			v[i].pos = VGet(
				position.m_x + rotatedX,
				position.m_y + m_height / 2,
				position.m_z + rotatedZ
			);

			// 法線は上向き固定(Y軸回転では変化しないため常に真上でよい)
			v[i].norm = VGet(0.0f, 1.0f, 0.0f);
			v[i].dif = GetColorU8(255, 255, 255, 255);
			v[i].spc = GetColorU8(0, 0, 0, 0);
			v[i].su = 0.0f;
			v[i].sv = 0.0f;
		}

		// --- UV座標はローカル空間基準で固定する ---
		// ローカルのV軸(0→1の向き)を常に「搬送方向」として統一しているため、
		// 回転前と回転後で個別に向きを分岐させる必要がなくなる
		v[0].u = 0.0f - m_uvScrollOffset; v[0].v = 1.0f;
		v[1].u = 1.0f - m_uvScrollOffset; v[1].v = 1.0f;
		v[2].u = 1.0f - m_uvScrollOffset; v[2].v = 0.0f;
		v[3].u = 0.0f - m_uvScrollOffset; v[3].v = 0.0f ;

		// 矩形を2三角形のインデックスで表現
		WORD idx[6] = { 0, 3, 1, 3, 2, 1 };

		DrawTriangle3D(p1.ToVECTOR(), p2.ToVECTOR(), p3.ToVECTOR(), m_color, true);
		DrawTriangle3D(p3.ToVECTOR(), p2.ToVECTOR(), p4.ToVECTOR(), m_color, true);

		DrawPolygonIndexed3D(v, 4, idx, 2, m_uvScrollTexHandle, true);

	}
	else
	{
		DrawTriangle3D(p1.ToVECTOR(), p2.ToVECTOR(), p3.ToVECTOR(), m_color, true);
		DrawTriangle3D(p3.ToVECTOR(), p2.ToVECTOR(), p4.ToVECTOR(), m_color, true);
	}

	// 底面
	DrawTriangle3D(p7.ToVECTOR(), p6.ToVECTOR(), p5.ToVECTOR(), m_color, true);
	DrawTriangle3D(p8.ToVECTOR(), p6.ToVECTOR(), p7.ToVECTOR(), m_color, true);

	//側面
	DrawTriangle3D(p5.ToVECTOR(), p1.ToVECTOR(), p7.ToVECTOR(), m_color, true);
	DrawTriangle3D(p7.ToVECTOR(), p1.ToVECTOR(), p3.ToVECTOR(), m_color, true);

	DrawTriangle3D(p7.ToVECTOR(), p3.ToVECTOR(), p8.ToVECTOR(), m_color, true);
	DrawTriangle3D(p8.ToVECTOR(), p3.ToVECTOR(), p4.ToVECTOR(), m_color, true);

	DrawTriangle3D(p8.ToVECTOR(), p4.ToVECTOR(), p6.ToVECTOR(), m_color, true);
	DrawTriangle3D(p6.ToVECTOR(), p4.ToVECTOR(), p2.ToVECTOR(), m_color, true);

	DrawTriangle3D(p6.ToVECTOR(), p2.ToVECTOR(), p5.ToVECTOR(), m_color, true);
	DrawTriangle3D(p5.ToVECTOR(), p2.ToVECTOR(), p1.ToVECTOR(), m_color, true);

	if (m_alpha != 255)
	{
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}

}

void Square3D::SetUVScrollTexHandle(GraphicId id)
{
	m_uvScrollTexHandle = ResourceManager::GetInstance().GetHandle(id);
}
