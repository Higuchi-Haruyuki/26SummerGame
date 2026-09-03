#pragma once
#include "Component.h"
#include <memory>

class Object;
class MapManager;
class ItemStack;
class CharactorStateManager;

enum class Item;

class MiningSystem:
	public Component
{
public:
	MiningSystem(std::weak_ptr<Object> obj);
	~MiningSystem();

	void Init() override;
	void Update() override;

	Item GetResourceAtMousePointer() const;
	std::shared_ptr<ItemStack> Mining(int count) const;

private:
	MapManager& m_mapManager;

	std::weak_ptr<CharactorStateManager> m_stateManager;

	Vector GetHitPosFromMousePointer() const;

};

