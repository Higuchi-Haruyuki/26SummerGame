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


namespace
{
	const Vector kTitleUIPos = { Game::kDisplaySize.m_x * 0.5f,Game::kDisplaySize.m_y * 0.3f };
	const Vector kTitleUISize = { 1000,500 };

	const Vector kChoiceBoxSize = { 200,50 };
	const Vector kChoiceBoxFirstPos = { Game::kDisplaySize.m_x * 0.5f,Game::kDisplaySize.m_y * 0.5f };
	const Vector kChoiceBoxOffset = { 0,Game::kDisplaySize.m_y * 0.2f };

	const Color kChoiceBoxColor = Color::kMainAccentColor;
}

TitleScene::TitleScene() :
	Scene(),
	m_uiManager(UIManager::GetInstance()),
	m_sceneManager(SceneManager::GetInstance()),
	m_playerInput(PlayerInput::GetInstance())
{}

TitleScene::~TitleScene()
{}

void TitleScene::Init()
{
	Scene::Init();

	BuildUI();
}

void TitleScene::Update()
{
	Scene::Update();

	UpdateUI();
}

void TitleScene::BuildUI()
{
	m_mainUIPanel = m_uiManager.GetOrCreateScreen("titleSceneMain", UILayer::Screen);

	UIFactory::MakeUIToPanel<UIImage>(m_mainUIPanel, kTitleUIPos, kTitleUISize, GraphicId::kTitleImage);

	auto pos = kChoiceBoxFirstPos;
	auto color = static_cast<unsigned int>(kChoiceBoxColor);
	for (auto& [square, text] : m_choiceBox) 
	{
		square = UIFactory::MakeUIToPanel<UISquare>(m_mainUIPanel,pos,kChoiceBoxSize,color,0);
		square.lock()->SubscribeOnClick([this]() 
			{
				m_sceneManager.SetNextScene(std::make_shared<MainScene>());
			});
		text = UIFactory::MakeUIToPanel<UIText>(m_mainUIPanel,
			TextArgs{
			pos
			});

		pos += Vector{0, kChoiceBoxSize.m_y} + kChoiceBoxOffset;
		text.lock()->SetText("こんにちは");

	}
}

void TitleScene::UpdateUI()
{}

void TitleScene::InputAction()
{
	if (m_playerInput.GetAction(""));
}
