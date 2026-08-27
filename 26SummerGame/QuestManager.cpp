#include "QuestManager.h"
#include "Base_Quest.h"
#include "Quest_MakeItem.h"
#include "Debug.h"
#include <format>
#include "ItemType.h"

// インスタンス取得
QuestManager& QuestManager::GetInstance()
{
	static QuestManager instance;
	return instance;
}

void QuestManager::Init()
{
	GenerateQuest();

	SetActiveQuest();

	ListenQuestClearEvent();
}

void QuestManager::Update()
{
	QuestClearCheck();


	if (!m_questQueue.size()) return;
	const auto& quest = m_questQueue.front();
	Debug::Log(std::format("クエスト名: {}",quest->GetDescription()));
}

Base_Quest* QuestManager::GetCurrentQuest() const
{
	if (!m_questQueue.size()) return nullptr;
	return m_questQueue.front().get();
}

void QuestManager::QuestClearCheck()
{
	const auto& quest = m_questQueue.front();
	quest->ClearCheck();

}

void QuestManager::GenerateQuest()
{
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_0", "石炭を10個掘ろう",
		Item::kCoalOre, 10));
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_1","鉄板を10個作ろう",
		Item::kIronPlate,10));
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_2", "鉄板を20個作ろう",
		Item::kIronPlate, 20));
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_3", "鉄板を30個作ろう",
		Item::kIronPlate, 30));
}

void QuestManager::SetActiveQuest()
{
	m_questQueue.front()->Init();
}

void QuestManager::ListenQuestClearEvent()
{
	m_questQueue.front()->SetOnClear([this]() 
		{
			NextQuest();
		});
}

void QuestManager::NextQuest()
{
	m_questQueue.pop();
	SetActiveQuest();
	ListenQuestClearEvent();
}

