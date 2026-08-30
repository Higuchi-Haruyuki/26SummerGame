#pragma once
#include "Component.h"
#include <memory>

class Object;
class MapManager;
class ItemStack;
class CharactorStateManager;

class MiningSystem:
	public Component
{
public:
	MiningSystem(std::weak_ptr<Object> obj);
	~MiningSystem();

	void Init() override;
	void Update() override;

	std::unique_ptr<ItemStack> Mining(int count) const;

private:
	MapManager& m_mapManager;

	std::weak_ptr<CharactorStateManager> m_stateManager;

	Vector GetHitPosFromMousePointer() const;

};

