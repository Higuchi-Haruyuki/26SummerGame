#include "AssemblingMachine.h"
#include "Debug.h"
#include "FactoryComponent.h"
#include "FactoryManager.h"
#include "Game.h"
#include "ItemStack.h"
#include "Object.h"
#include "PlayerInput.h"
#include "Square3D.h"
#include "SquareCollider3D.h"
#include "UIManager.h"
#include "vector.h"
#include "Recipe.h"
#include "RecipeManager.h"
#include "DrawText.h"
#include <DxLib.h>
#include <format>
#include <functional>
#include <memory>
#include <Windows.h>
#include <string>
#include "ResourceManager.h"
#include"ResourceName.h"
#include "FuelSystem.h"
#include "Timer.h"
#include "ManufacturingSystem.h"
#include "ItemType.h"
#include "UIItemBox.h"
#include "ItemSlot.h"
#include "UIPanel.h"
#include "ItemManager.h"

AssemblingMachine::AssemblingMachine(std::shared_ptr<Object> parentObject):
	FactoryComponent(parentObject)
{}

AssemblingMachine::~AssemblingMachine()
{}

void AssemblingMachine::Init()
{}

void AssemblingMachine::Update(float deltaTime)
{}

void AssemblingMachine::UpdateUIPanel()
{}

std::unique_ptr<ItemStack> AssemblingMachine::MakeItemStackFromThisComponent()
{
	return std::unique_ptr<ItemStack>();
}

void AssemblingMachine::BuildUIPanel()
{}
