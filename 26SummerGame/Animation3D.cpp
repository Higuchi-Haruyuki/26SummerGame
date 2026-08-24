#include "Animation3D.h"
#include <string>
#include <DxLib.h>
#include "Object.h"
#include "Component.h"
#include "Model.h"
#include "Debug.h"
#include <format>

void Animation3D::Init()
{
	Component::Init();
	
	m_model = GetParentObject()->GetComponent<Model>();
}

void Animation3D::Update(float deltaTime)
{
	Component::Update(deltaTime);
	if (!m_isEnable) return;
	if (!m_model) m_model = GetParentObject()->GetComponent<Model>();
	if (!m_model) return;

	if (!m_isPlaying) return;
	m_animPlayTime += m_animSpeed;
	Debug::Log(std::format("animName: {},animPlayTime: {}",m_attachAnimName,m_animPlayTime));
	if (m_animPlayTime >= m_totalTime)
	{
		if (m_isLoop)m_animPlayTime = 0.0f;
		else Stop();
	}
	int modelHandle = m_model->GetModelHandle();
	if(modelHandle != -1 && m_attachIndex != -1 && m_animPlayTime != -1)
		MV1SetAttachAnimTime(modelHandle, m_attachIndex, m_animPlayTime);

}

void Animation3D::Finalize()
{
	Component::Finalize();
}

void Animation3D::AttachAnim(const std::string& animName)
{
	//すでにそのアニメーションが再生されているときはアタッチしない
	if (animName == m_attachAnimName) return;

	DettachAnim();
	m_animPlayTime = 0;
	AnimationInfo animInfo = GetAnimationInfoFromName(animName);
	m_animSpeed = animInfo.m_speed;
	m_attachIndex = MV1AttachAnim(m_model->GetModelHandle(),animInfo.m_index);
	if (m_attachIndex != -1) m_attachAnimName = animName;
	GetAttachAnimTotalTime();
}


void Animation3D::GetAttachAnimTotalTime()
{
	if (m_attachIndex == -1) return;
	m_totalTime = MV1GetAttachAnimTotalTime(m_model->GetModelHandle(), m_attachIndex);
}

AnimationInfo Animation3D::GetAnimationInfoFromName(const std::string& animName)
{
	for (auto& animInfo : m_animationInfo)
	{
		if (animName == animInfo.m_name) return animInfo;
	}
	printfDx("アニメーション名[%s]は不正です。", animName);
	return {"NULL",0,0};
}

void Animation3D::DettachAnim()
{
	if (m_attachIndex == -1) return;
	MV1DetachAnim(m_model->GetModelHandle(), m_attachIndex);
	m_attachIndex = -1;
	m_totalTime = -1;
	m_animPlayTime = 0;
	m_attachAnimName = "";
}


