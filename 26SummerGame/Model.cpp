#include "Model.h"
#include "Object.h"
#include "Debug.h"
#include "ResourceManager.h"
#include <DxLib.h>
#include <cmath>

void Model::Init()
{
	Shape::Init();
}

void Model::Update(float deltaTime)
{
	Shape::Update(deltaTime);
	if (!m_isEnable) return;
	TextureScroll(deltaTime);
}

void Model::Finalize()
{
	Shape::Finalize();
}

void Model::Draw(const Vector& cameraPos) const
{
	if (!m_isEnable) return;
	if (!GetParentObject()) return;
	if (!GetParentObject()->GetIsEnable()) return;
	if (!m_isVisible) return;
	if (m_modelHandle == -1)
	{
		printfDx("モデルハンドルが不正です\n");
		return;
	}

	//不透明でない時だけブレンドモードを設定する関数を呼び出す。
	if (m_alpha != 255)
	{
		MV1SetMaterialDrawBlendModeAll(m_modelHandle, DX_BLENDMODE_ALPHA_X4);
		MV1SetMaterialDrawBlendParamAll(m_modelHandle, m_alpha);
	}

	MV1SetPosition(m_modelHandle, GetParentObject()->GetPosition().ToVECTOR());
	//MV1SetScale(m_modelHandle, m_scale.ToVECTOR());
	MV1SetRotationXYZ(m_modelHandle, VGet(0, m_rotationAngle, 0));
	
	if (!m_isEnableLighting) SetUseLighting(false);
	MV1DrawModel(m_modelHandle);
	if (!m_isEnableLighting) SetUseLighting(true);

	if (m_alpha != 255)
	{
		MV1SetMaterialDrawBlendModeAll(m_modelHandle, DX_BLENDMODE_NOBLEND);
	}
}

void Model::SetModel(ModelId id)
{
	m_modelHandle = ResourceManager::GetInstance().GetHandle(id);
}

void Model::TextureScroll(float deltaTime)
{
	if (m_modelHandle == -1) return;
	if (m_frameIndex == -1) return;

	m_currentOffsetU = m_currentOffsetU + m_scrollSpeedU * deltaTime;
	
	int i = MV1SetFrameTextureAddressTransform(
		m_modelHandle, 
		m_frameIndex, 
		m_currentOffsetU, 0.0f, 
		1.0f, 1.0f, 
		0.0f, 0.0f, 0.0f
	);
}

