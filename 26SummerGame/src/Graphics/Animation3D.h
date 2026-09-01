#pragma once
#include "Component.h"
#include <vector>
#include <memory>
#include <map>
#include <string>
#include "Object.h"
struct AnimationInfo
{
	std::string m_name;
	int m_index;
	float m_speed;
};
class Model;
class Animation3D : public Component
{
public:
	Animation3D(std::shared_ptr<Object> parentObject) : Component(parentObject) {}
	~Animation3D() override = default;

	void Init() override;
	void Update() override;
	void Finalize() override;

	/// <summary>
	/// アニメーションの名前とインデックスを紐づけた配列を設定する。
	/// </summary>
	/// <param name="nameAndIndex"></param>
	void SetAnimNameAndIndex(const std::vector<AnimationInfo>& nameAndIndex)
	{
		m_animationInfo = nameAndIndex;
	}
	
	/// <summary>
	/// 保存された配列からアニメーションの名前を検索して、そのアニメーションをアタッチする。
	/// </summary>
	/// <param name="animName"></param>
	void AttachAnim(const std::string& animName);

	/// <summary>
	/// アタッチ中のアニメーションの時間を取得して設定する。
	/// </summary>
	void GetAttachAnimTotalTime();

	/// <summary>
	/// 保存された配列からアニメーションの名前を検索して、そのAnimationInfoへの参照を返す。
	/// </summary>
	/// <param name="aniName"></param>
	/// <returns></returns>
	AnimationInfo GetAnimationInfoFromName(const std::string& aniName);
	
	/// <summary>
	/// アニメーションを再生する。
	/// </summary>
	void Play() { m_isPlaying = true; }

	/// <summary>
	/// アニメーションを停止する。
	/// </summary>
	void Stop() { m_isPlaying = false; }

	/// <summary>
	/// アニメーションのループを設定する。
	/// </summary>
	/// <param name="b"></param>
	void IsLoop(bool b) { m_isLoop = b; }


private:

	/// <summary>
	/// アニメーションのアタッチを解除する。
	/// </summary>
	/// <param name="animName"></param>
	void DettachAnim();
private:
	std::weak_ptr<Model> m_model;
	int m_attachIndex = -1;
	int m_totalTime = -1;
	std::string m_attachAnimName = "";
	std::vector<AnimationInfo> m_animationInfo;
	float m_animSpeed = 1;
	bool m_isPlaying = false;
	bool m_isLoop = true;
	float m_animPlayTime = 0;
};
