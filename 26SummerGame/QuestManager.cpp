#include "QuestManager.h"
#include "Base_Quest.h"
#include "Quest_MakeItem.h"
#include "Debug.h"
#include <format>
#include "ItemType.h"
#include "SoundManager.h"
#include "ResourceType.h"

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
	if (m_questQueue.empty()) {
		return;
	}
	if (m_questQueue.front()->ClearCheck()) {
		NextQuest();
		SoundManager::GetInstance().Play(SoundId::kOnQuestEndSe);
	}
}

void QuestManager::GenerateQuest()
{
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_0", "石を5個採掘しよう",
		Item::kRockOre, 5));
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_1", "石炭を5個採掘しよう",
		Item::kCoalOre, 5));
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_2", "溶鉱炉を1個作ろう",
		Item::kFurnace, 1));
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_3", "鉄板を10個生産しよう",
		Item::kIronPlate, 10));
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_4", "採掘機を1個作ろう",
		Item::kMiningMachine, 1));
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_5", "銅板を5個生産しよう",
		Item::kCopperPlate, 5));
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_6", "銅線を10個生産しよう",
		Item::kCopperCable, 10));
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_7", "電子基板を3個生産しよう",
		Item::kElectronicCircuit, 3));
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_8", "ベルトコンベアを6個作ろう",
		Item::kBeltconveyor, 6));
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_9", "インサータを2個作ろう",
		Item::kInserter, 2));
	m_questQueue.push(std::make_unique<Quest_MakeItem>(
		"quest_10", "組立機を1個作ろう",
		Item::kAssemblingMachine, 1));
}

void QuestManager::SetActiveQuest()
{
	if (m_questQueue.empty()) {
		return;
	}
	m_questQueue.front()->Init();
}

void QuestManager::NextQuest()
{
	if (m_questQueue.empty()) {
		return;
	}

	m_questQueue.pop();
	SetActiveQuest();
}

