#include "TitleScene.h"
#include "UIManager.h"
#include "UIFactory.h"
#include "UIPanel.h"
#include "SceneManager.h"
#include "UIImage.h"
#include "Game.h"
#include "ResourceType.h"
#include "Color.h"
#include "UISquare.h"
#include "UIText.h"
#include "PlayerInput.h"
#include "MainScene.h"
#include"MapManager.h"
#include "Object.h"
#include "Model.h"
#include "Camera.h"
#include "BeltConveyor.h"
#include "Inserter.h"
#include "MiningMachine.h"
#include "Furnace.h"
#include "SoundManager.h"

namespace
{
	//スカイボックス
	const VectorInt kPlayerStartGridPos = { 0,0,0 };
	const Vector kSkyDomePos = { 7900,0,7900 };

	//カメラの初期位置と角度
	const Vector kCameraStartPos = {2367,762,1290};
	constexpr float kCameraStartPitch = 0.444f;
	constexpr float kCameraStartYaw = 0.6f;

	//工業オブジェクト初期位置
	const VectorInt kFactoryObjectStartPos = { 12,0,12 };
	constexpr int kBeltConveyorCount = 10;

	const Vector kTitleUIPos = { Game::kDisplaySize.m_x * 0.5f,Game::kDisplaySize.m_y * 0.3f };
	const Vector kTitleUISize = { 1000,500 };

	const Vector kChoiceBoxSize = { 200,50 };
	const Vector kChoiceBoxFirstPos = { Game::kDisplaySize.m_x * 0.5f,Game::kDisplaySize.m_y * 0.6f };
	const Vector kChoiceBoxOffset = { 0,Game::kDisplaySize.m_y * 0.07f };

	const Color kChoiceBoxNormalColor = Color::kMainColor;
	const Color kChoiceBoxAccentColor = Color::kMainAccentColor;
}

TitleScene::TitleScene() :
	Scene(),
	m_uiManager(UIManager::GetInstance()),
	m_sceneManager(SceneManager::GetInstance()),
	m_playerInput(PlayerInput::GetInstance()),
	m_mapManager(MapManager::GetInstance())
{}

TitleScene::~TitleScene()
{}

void TitleScene::Init()
{
	Scene::Init();

	BuildUI();

	m_mapManager.Init();
	m_mapManager.GenerateMap();

	m_skyDome = ObjectFactory::CreateObject(
		kSkyDomePos, "SkyBox"
	);

	const auto& model = m_skyDome.lock()->AddComponent<Model>().lock();
	model->SetEnableLighting(false);
	model->SetModel(ModelId::kSkyBox);

	//カメラ操作を受け付けない
	SetCameraControll(false);
	CameraInitPos();

	//工業オブジェクト初期処理
	PutOnFactoryObject(kFactoryObjectStartPos,kBeltConveyorCount);

	PutOnFactoryObject(kFactoryObjectStartPos + VectorInt{-2,0,-3}, kBeltConveyorCount + 2);

	PutOnFactoryObject(kFactoryObjectStartPos + VectorInt{-5,0,4 }, kBeltConveyorCount + 10);

	//Bgm
	SoundManager::GetInstance().PlayLoop(SoundId::kTitleBGM);
}

void TitleScene::Update()
{
	Scene::Update();

	UpdateUI();
}

void TitleScene::Finalize()
{
	SoundManager::GetInstance().StopLoop(SoundId::kTitleBGM);
}

void TitleScene::BuildUI()
{
	m_mainUIPanel = m_uiManager.GetOrCreateScreen("titleSceneMain", UILayer::Screen);

	UIFactory::MakeUIToPanel<UIImage>(m_mainUIPanel, kTitleUIPos, kTitleUISize, GraphicId::kTitleImage);

	auto pos = kChoiceBoxFirstPos;

	auto normalColor = static_cast<unsigned int>(kChoiceBoxNormalColor);
	auto accentColor = static_cast<unsigned int>(kChoiceBoxAccentColor);

	for (auto& [square, text] : m_choiceBox)
	{
		UIFactory::MakeUIToPanel<UISquare>(m_mainUIPanel, pos, kChoiceBoxSize, normalColor, 100);
		square = UIFactory::MakeUIToPanel<UISquare>(m_mainUIPanel, pos, kChoiceBoxSize, accentColor, 0);
		square.lock()->SubscribeOnClick([this]()
			{
				m_uiManager.RemoveScreen("titleSceneMain");
				m_sceneManager.SetNextScene(std::make_shared<MainScene>());
			});
		square.lock()->SubscribeHover([this,&square]()
			{
				square.lock()->SetAlpha(255);
			});
		text = UIFactory::MakeUIToPanel<UIText>(m_mainUIPanel,
			TextArgs{
			pos
			});

		pos += Vector{ 0, kChoiceBoxSize.m_y } + kChoiceBoxOffset;

	}

	{
		const auto& [square, text] = m_choiceBox.at(0);
		text.lock()->SetText("はじめから");
	}

	{
		const auto& [square, text] = m_choiceBox.at(1);
		text.lock()->SetText("つづきから");
	}

}

void TitleScene::UpdateUI()
{
	for (auto& [square, text] : m_choiceBox)
	{
		square.lock()->SetAlpha(0);

	}
}

void TitleScene::CameraInitPos()
{
	m_camera.lock()->SetPosition(kCameraStartPos);
	m_camera.lock()->SetCameraPitch(kCameraStartPitch);
	m_camera.lock()->SetCameraYaw(kCameraStartYaw);
}

void TitleScene::PutOnFactoryObject(const VectorInt& startPos,int beltconveyorCount)
{
	VectorInt pos = startPos;
	const auto nextPos = [&pos]() 
		{
			pos += VectorInt{ 1,0,0 };
		};
	//採掘機
	ObjectFactory::CreateObject<MiningMachine>(pos);
	nextPos();

	//インサータ
	ObjectFactory::CreateObject<Inserter>(pos);
	nextPos();

	for (int i = 0; i < beltconveyorCount; i++)
	{
		ObjectFactory::CreateObject<BeltConveyor>(pos);
		nextPos();
	}

	//インサータ
	ObjectFactory::CreateObject<Inserter>(pos);
	nextPos();

	//採掘機
	ObjectFactory::CreateObject<Furnace>(pos);
	nextPos();
}
