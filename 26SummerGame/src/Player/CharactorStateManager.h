#pragma once
#include "Component.h"
#include <functional>
#include <map>
#include <string>
enum class CharactorState
{
	DESTROY,INSTALLATION,OPEN_FACTORYUI,OPEN_INVENTORYUI,IDLE
};
class CharactorStateManager : public Component
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	CharactorStateManager(const std::shared_ptr<Object>& parentObject) : Component(parentObject) ,m_currentState(CharactorState::IDLE){}
	//GETTER
	CharactorState GetCurrentState() const { return m_currentState; }

	void Init() override;
	void Update() override;

	//SETTER
	/// <summary>
	/// それぞれのステートの最低時間を設定する
	/// </summary>
	/// <param name="stateMinTime"></param>
	void SetStateMinTime(const std::map<CharactorState, int> stateMinTime);

	/// <summary>
	/// 現在のステートを変更する
	/// </summary>
	/// <param name="newState"></param>
	bool ChangeState(const CharactorState& newState);

	/// <summary>
	/// 設定された最低時間を無視して現在のステートを変更する
	/// </summary>
	/// <param name="newState"></param>
	void ChangeStateIgnoreMinTime(const CharactorState& newState);


	/// <summary>
	/// 現在のステートを確認する
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	bool CheckCurrentState(const CharactorState& state) const
	{
		if (m_currentState == state) return true;
		return false;
	}

	std::string CurrentStateToString() const
	{
		return CurrentStateToString(m_currentState);
	}

	std::string CurrentStateToString(const CharactorState& state) const
	{
		switch (state)
		{
		case CharactorState::DESTROY:
			return "Destroy";
		case CharactorState::INSTALLATION:
			return "Installation";
		case CharactorState::IDLE:
			return "Idle";
		case CharactorState::OPEN_FACTORYUI:
			return "OpenUI";
		default:
			return "Idle";
		}
	}
public:
	/// <summary>
	/// ステートが変更されたときに呼ばれるfunction
	/// 第1引数; 以前のステート
	/// 第2引数: 新しいステート
	/// </summary>
	std::function<void(const CharactorState&,const CharactorState&)> OnChangeState;
private:
	float m_timer = 0.0f;
	// それぞれのステートの最低時間
	std::map<CharactorState, int> m_stateMinTime;
	// 内部で保持する今のステート
	CharactorState m_currentState;
	//現在のステートが開始された時間
	float m_stateStartTime = 0;
};

