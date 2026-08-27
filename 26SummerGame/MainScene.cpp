#include "Debug.h"
#include "MainScene.h"
#include "ResourceName.h"
#include "ObjectFactory.h"
#include "Square3D.h"
#include "PlayerController.h"
#include "SquareCollider3D.h"
#include "Game.h"
#include "BeltConveyor.h"
#include "FactoryManager.h"
#include "CharactorStateManager.h"
#include "MapManager.h"	
#include <memory>
#include "Scene.h"
#include "vector.h"
#include <DxLib.h>
#include "VectorInt.h"
#include "Furnace.h"
#include "RecipeRegister.h"
#include "Model.h"
#include "InstallationMode.h"
#include "DestroyMode.h"
#include "StringUtil.h"

namespace
{
	//プレイヤー
	const VectorInt kPlayerStartGridPos = { 0,0,0 };
	const Vector kSkyDomePos = { 7900,0,7900 };
}

void MainScene::Init()
{
	Scene::Init();

	MapManager::GetInstance().Init();

	m_player = ObjectFactory::CreateObject(
		Game::GridPosToWorldPos(kPlayerStartGridPos), "Player"
	);

	const auto& player = m_player.lock();

	m_playerCollider = player->AddComponent<SquareCollider3D>();
	
	const auto& collider = m_playerCollider.lock();
	
	collider->SetSize(Vector{ 150,150,150 });
	collider->SetOffset({ 0,0,0 });
	collider->IsTrigger(true);

	m_stateManager = player->AddComponent<CharactorStateManager>();

	player->AddComponent<PlayerController>();

	player->AddComponent<InstallationMode>();

	player->AddComponent<DestroyMode>();
	
	m_skyDome = ObjectFactory::CreateObject(
		kSkyDomePos, "SkyBox"
	);
	
	const auto& model = m_skyDome.lock()->AddComponent<Model>().lock();
	model->SetEnableLighting(false);
	model->SetModel(ModelId::kSkyBox);

}
void MainScene::Update()
{
	Scene::Update();

	const auto& player = m_player.lock();
	if (!player) return;

	if (player->GetPosition().m_y < -1000)
	{
		player->SetPosition(Game::GridPosToWorldPos(kPlayerStartGridPos));
	}


	auto objectCount = GetSceneObjects().size();
	Debug::Log("Object数: " + StringUtil::IntToString(objectCount));

	auto colliderCount = m_sceneColliders.size();
	Debug::Log("Collider数: " + StringUtil::IntToString(colliderCount));

	auto shapeCount =  m_sceneShapes.size();
	Debug::Log("Shape数: " + StringUtil::IntToString(shapeCount));

	auto factoryCount = FactoryManager::GetInstance().GetFactoryComponentCount();
	Debug::Log("Factory数: " + StringUtil::IntToString(factoryCount));

}

void MainScene::Draw() const
{
	Scene::Draw();

	if (m_stateManager.lock()->CheckCurrentState(CharactorState::INSTALLATION))
		DrawGrid();

	MapManager::GetInstance().Draw();
}


