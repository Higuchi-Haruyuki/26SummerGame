#include "MiningSystem.h"
#include "RayCastResult.h"
#include "ResourceManager.h"
#include "MapManager.h"
#include "Game.h"
#include "ItemType.h"
#include "ItemStackFactory.h"
#include "ItemStack.h"
#include "Debug.h"
#include "ItemManager.h"
#include "UIManager.h"
#include "CharactorStateManager.h"
#include "FactoryManager.h"

MiningSystem::MiningSystem(std::weak_ptr<Object> obj) :
	Component(obj),
	m_mapManager(MapManager::GetInstance())
{}

MiningSystem::~MiningSystem() = default;

void MiningSystem::Init()
{
	Component::Init();
	m_stateManager = GetComponent<CharactorStateManager>();

}

void MiningSystem::Update()
{
	Component::Update();

	const auto hitPos = GetHitPosFromMousePointer();

	const auto hitGridPos = Game::WorldPosToGridPos(hitPos);

	Debug::Log(std::format("hitGridPos: {}",hitGridPos.ToString()));

	const auto item = m_mapManager.GetResourceAtGridPos(hitGridPos);

	Debug::Log(std::format("資源: {}",ItemTable::ItemTypeToDisplayName(item)));
}

Vector MiningSystem::GetHitPosFromMousePointer() const
{
	RayCastResult result;
	result.RaycastFromMousePoint();
	return result.GetHitPoint();
}

std::shared_ptr<ItemStack> MiningSystem::Mining(int count) const
{
	if (UIManager::GetInstance().IsPointerHoverUI()) return nullptr;

	if (!m_stateManager.lock()->CheckCurrentState(CharactorState::IDLE)) return nullptr;

	const auto hitPos = GetHitPosFromMousePointer();

	const auto hitGridPos = Game::WorldPosToGridPos(hitPos);

	const auto item = m_mapManager.GetResourceAtGridPos(hitGridPos);

	if (item == Item::kNone) return nullptr;

	FactoryManager::GetInstance().OnMakeItem(item, count);

	return ItemStackFactory::Make(item, count);

}
