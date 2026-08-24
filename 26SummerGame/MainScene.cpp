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

	m_playerCollider = m_player->AddComponent<SquareCollider3D>();
	m_playerCollider->SetSize(Vector{ 150,150,150 });
	m_playerCollider->SetOffset({ 0,0,0 });
	m_playerCollider->IsTrigger(true);

	m_stateManager = m_player->AddComponent<CharactorStateManager>();

	m_player->AddComponent<PlayerController>();

	m_player->AddComponent<component::InstallationMode>();

	m_player->AddComponent<component::DestroyMode>();
	
	m_skyDome = ObjectFactory::CreateObject(
		kSkyDomePos, "SkyBox"
	);
	
	const auto& model = m_skyDome.lock()->AddComponent<Model>();
	model->SetEnableLighting(false);
	model->SetModel(ModelId::kSkyBox);


}
void MainScene::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	if (m_player->GetPosition().m_y < -1000)
	{
		m_player->SetPosition(Game::GridPosToWorldPos(kPlayerStartGridPos));
	}

	if (m_stateManager.lock()->CheckCurrentState(CharactorState::INSTALLATION))
		DrawGrid();

	MapManager::GetInstance().Draw();

}


