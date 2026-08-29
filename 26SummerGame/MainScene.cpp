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
#include "QuestManager.h"

namespace
{
	//プレイヤー
	const VectorInt kPlayerStartGridPos = { 0,0,0 };
	const Vector kSkyDomePos = { 7900,0,7900 };
}

MainScene::MainScene():
Scene(),
m_mapManager(MapManager::GetInstance())
{}

MainScene::~MainScene() = default;

void MainScene::Init()
{
	Scene::Init();

	QuestManager::GetInstance().Init();

	m_mapManager.Init();
	m_mapManager.GenerateMap();
	m_mapManager.GenerateMapResource();

	m_player = ObjectFactory::CreateObject(
		Game::GridPosToWorldPos(kPlayerStartGridPos), "Player"
	);

	const auto& player = m_player.lock();

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

	QuestManager::GetInstance().Update();

	const auto& player = m_player.lock();
	if (!player) return;

	if (player->GetPosition().m_y < -1000)
	{
		player->SetPosition(Game::GridPosToWorldPos(kPlayerStartGridPos));
	}

}

void MainScene::Draw() const
{
	Scene::Draw();

	if (m_stateManager.lock()->CheckCurrentState(CharactorState::INSTALLATION))
		DrawGrid();

	m_mapManager.Draw();
}


