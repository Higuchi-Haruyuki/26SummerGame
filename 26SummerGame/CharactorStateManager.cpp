#include "CharactorStateManager.h"
#include "DxLib.h"
#include "Debug.h"


void CharactorStateManager::Init()
{
	Component::Init();
}

void CharactorStateManager::Update()
{
	Component::Update();
	if (m_isEnable) return;
	m_timer++;
}

void CharactorStateManager::SetStateMinTime(const std::map<CharactorState, int> stateMinTime)
{
	if (stateMinTime.size() == 0) return;
	for (auto v : stateMinTime)
	{
		m_stateMinTime.emplace(v);
	}
}

void CharactorStateManager::ChangeState(const CharactorState& newState)
{

	//引数で与えられたステートが現在のステートと同じ時処理をしない
	if (m_currentState == newState) return;

	//現在のステートの最低時間情報を持っているか
	auto it = m_stateMinTime.find(m_currentState);
	//持っているなら
	if (it != m_stateMinTime.end())
	{
		//指定経過時間以下なら処理をしない
		if (m_timer - m_stateStartTime <= m_stateMinTime[m_currentState]) 
		{
			return;
		}
	}
	auto before = m_currentState;
	m_currentState = newState;

	//ステート変更時のfunctionをよぶ
	if(OnChangeState) OnChangeState(before, m_currentState);

	m_stateStartTime = m_timer;
}

void CharactorStateManager::ChangeStateIgnoreMinTime(const CharactorState& newState)
{

	//引数で与えられたステートが現在のステートと同じ時処理をしない
	if (m_currentState == newState) return;

	auto before = m_currentState;
	m_currentState = newState;

	//ステート変更時のfunctionをよぶ
	if(OnChangeState) OnChangeState(before, m_currentState);


	m_stateStartTime = m_timer;
}
