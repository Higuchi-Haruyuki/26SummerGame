#include "QuestUI.h"

#include "Base_Quest.h"
#include "Color.h"
#include "Game.h"
#include "QuestManager.h"
#include "ResourceType.h"
#include "PlayerInput.h"
#include "UIFactory.h"
#include "UIManager.h"
#include "UIPanel.h"
#include "UIImage.h"
#include "UIProgressBar.h"
#include "UISquare.h"
#include "UIText.h"
#include "vector.h"

#include <array>

namespace
{
	const Vector kQuestUIPos = { 150.0f, Game::kDisplaySize.m_y / 2 - 100 };
	const Vector kQuestUISize = { 300.0f, 100.0f };

	//ヘルプボタン
	const Vector kHelpButtonSize = Vector{100,25};
	const Vector kHelpButtonPosOffset = Vector{kQuestUISize.m_x * 0.5f, 10} - Vector{kHelpButtonSize.m_x * 0.5f};
	const Vector kHelpButtonPos = kQuestUIPos + Vector{0,kQuestUISize.m_y * 0.5f} + Vector{0,kHelpButtonSize.m_y * 0.5f} + kHelpButtonPosOffset;
	//モーダルウィンドウ
	const Vector kModalWindowPos = Game::kDisplaySize*0.5f;
	const Vector kModalWindowSize = Vector{1280,720};
	const Vector kModalWindowRightUp = Vector{kModalWindowPos.m_x + kModalWindowSize.m_x * 0.5f,kModalWindowPos.m_y - kModalWindowSize.m_y * 0.5f};

	//上部タブバー
	const Vector kTabBarSize = Vector{1280,80};
	const Vector kTabBarPos = Vector{kModalWindowPos.m_x ,kModalWindowPos.m_y - kModalWindowSize.m_y* 0.5f + kTabBarSize.m_y * 0.5f};

	const Vector kTabSize = {200,50};
	const Vector kTabOffset = {20,0};
	const Vector kTabFirstPos = kTabBarPos - Vector{kTabBarSize.m_x * 0.5f} + kTabOffset + Vector{kTabSize.m_x * 0.5f};

	const Vector kPanelSize = kModalWindowSize - Vector{0,kTabBarSize.m_y};
	const Vector kPanelPos = kModalWindowPos + Vector{0,kTabBarSize.m_y};

	constexpr int kTabCount = 3;
	const std::array<std::string, kTabCount> kTabTitle = {"基本操作","インベントリ","設置モード"};
}

QuestUI::QuestUI(std::weak_ptr<Object> parentObject) :
	Component(parentObject),
	m_uiManager(UIManager::GetInstance()),
	m_questManager(QuestManager::GetInstance()),
	m_playerInput(PlayerInput::GetInstance())
{}

void QuestUI::Init()
{
	Component::Init();
	m_questUIPanel = m_uiManager.GetOrCreateScreen("quest", UILayer::Hud);
	UIFactory::MakeUIToPanel<UISquare>(m_questUIPanel, 
		kQuestUIPos, kQuestUISize, 
		static_cast<unsigned int>(Color::kMainColor), 200);

	m_questText = UIFactory::MakeUIToPanel<UIText>(m_questUIPanel, TextArgs{
				kQuestUIPos + Vector{0,kQuestUISize.m_y * 0.2f},
				TextPivot::CenterBottom,
				FontId::kQuestUIText,
				"",
				static_cast<unsigned int>(Color::kMainCharColor),
				static_cast<unsigned int>(Color::kSubCharColor),
				255
		});
	UIFactory::MakeUIToPanel<UIText>(m_questUIPanel, TextArgs{
				kQuestUIPos - Vector{0,kQuestUISize.m_y * 0.4f},
				TextPivot::CenterTop,
				FontId::kFactoryUIText,
				"<クエスト>",
				static_cast<unsigned int>(Color::kSubCharColor),
				static_cast<unsigned int>(Color::kMainCharColor),
				255
		});

	const Vector kQuestProgressBarSize = {kQuestUISize.m_x,10};

	m_questProgressBar = UIFactory::MakeUIToPanel<UIProgressBar>(m_questUIPanel, 
		kQuestUIPos + Vector{0,kQuestUISize.m_y * 0.45f}, kQuestProgressBarSize,
		static_cast<unsigned int>(Color::kMainAccentColor), 200);

	m_questProgressText = UIFactory::MakeUIToPanel<UIText>(m_questUIPanel, 
		TextArgs{
			kQuestUIPos + Vector{kQuestUISize.m_x * 0.5f ,kQuestUISize.m_y * 0.5f},
			TextPivot::RightBottom
		});

	/*ヘルプUIの初期化処理*/
	
	const auto& helpButtonSquare = UIFactory::MakeUIToPanel<UISquare>(m_questUIPanel,
		kHelpButtonPos, kHelpButtonSize,
		static_cast<unsigned int>(Color::kMainColor), 200);
	helpButtonSquare.lock()->SubscribeOnClick([this]()
	{
		const auto& safePanel = m_helpPanel.lock();
		if(!safePanel) return;
		safePanel->SetVisible(true);
		m_playerInput.SetInputEnable(false);
	});
	helpButtonSquare.lock()->SubscribeOnHover([helpButtonSquare]()
	{
		helpButtonSquare.lock()->SetColor(static_cast<unsigned int>(Color::kMainAccentColor));
	}
	);
	
	const auto& helpButtonText = UIFactory::MakeUIToPanel<UIText>(m_questUIPanel, 
		TextArgs{
			kHelpButtonPos
		});
	helpButtonText.lock()->SetText("ヘルプ！");
	m_helpVisibleButton = std::make_pair(helpButtonSquare,helpButtonText);
	
	//ヘルプパネル
	m_helpPanel = UIFactory::MakeUIToPanel<UIPanel>(m_questUIPanel,
		Game::kDisplaySize * 0.5f,Game::kDisplaySize);

	//起動時にヘルプパネルだしとく
	m_helpPanel.lock()->SetVisible(true);
	//入力を無効にする
	m_playerInput.SetInputEnable(false);

	//背景をちょっと薄暗くする
	const auto& backGround = UIFactory::MakeUIToPanel<UISquare>(m_helpPanel,
		Game::kDisplaySize * 0.5f,Game::kDisplaySize,
		static_cast<unsigned int>(Color::kMainColor),200
	);

	const auto& modalWindow = UIFactory::MakeUIToPanel<UISquare>(m_helpPanel,
		kModalWindowPos, kModalWindowSize,
		static_cast<unsigned int>(Color::kMainColor)
	);

	const auto& tabBar = UIFactory::MakeUIToPanel<UISquare>(m_helpPanel,
		kTabBarPos, kTabBarSize,
		static_cast<unsigned int>(Color::kSubColor)
	);

	const Vector kCloseImageSize = Vector{50,50};
	const auto& closeImage = UIFactory::MakeUIToPanel<UIImage>(m_helpPanel,
		kModalWindowRightUp + Vector{-kCloseImageSize.m_x * 0.5f,kCloseImageSize.m_y * 0.5f} + Vector{-15,15},
		kCloseImageSize,GraphicId::kUIClose
	);
	closeImage.lock()->SubscribeOnClick([this]()
	{
		m_helpPanel.lock()->SetVisible(false);
		m_playerInput.SetInputEnable(true);

	});

	//タブパネルの作成と初期処理
	InitBaseContorollTab();
	InitInventoryContorollTab();
	InitInstallationTab();
	InitDestroyTab();

	for (size_t i = 0; i < kTabCount; i++)
	{
		const auto pos = kTabFirstPos + (Vector{kTabSize.m_x} + kTabOffset) * static_cast<float>(i);
		const auto panel = m_tabPanels.at(i);

		const auto& square = UIFactory::MakeUIToPanel<UISquare>(m_helpPanel,
		pos,kTabSize,static_cast<unsigned int>(Color::kMainColor)
		);
		square.lock()->SubscribeOnClick([this,panel](){
			SetPanelVisible(panel,true);
		});
		square.lock()->SubscribeOnHover([square]()
		{
			square.lock()->SetColor(static_cast<unsigned int>(Color::kMainAccentColor));
		});
		m_tabSquares.push_back(square);

		const auto& text = UIFactory::MakeUIToPanel<UIText>(m_helpPanel,
			TextArgs{
				pos
			}
		);
		text.lock()->SetText(kTabTitle.at(i));
	}
	SetPanelVisible(0,true);
	

}

void QuestUI::Update()
{
	if (!m_isEnable) return;

	//ヘルプボタンを通常の色にする
	const auto& [square,text] = m_helpVisibleButton;
	square.lock()->SetColor(static_cast<unsigned int>(Color::kMainColor));

	//タブの色を通常の色にする
	for(const auto& tabSquare : m_tabSquares)
	{
		tabSquare.lock()->SetColor(static_cast<unsigned int>(Color::kMainColor));
	}

	const auto& safeQuestDesc = m_questText.lock();
	if (!safeQuestDesc) return;

	const Base_Quest* quest = m_questManager.GetCurrentQuest();
	if (quest)
	{
		safeQuestDesc->SetText(quest->GetDescription());

		auto progress = quest->GetProgress();

		m_questProgressBar.lock()->SetProgress(progress);
		m_questProgressText.lock()->SetText("{} %", static_cast<int>(progress * 100.0f));
	}
	else
	{
		safeQuestDesc->SetText("楽しもう！！");
		m_questProgressBar.lock()->SetVisible(false);
		m_questProgressText.lock()->SetVisible(false);
	}

}

void QuestUI::Finalize()
{
	if (m_questUIPanel)
	{
		m_questUIPanel->SetVisible(false);
		m_uiManager.RemoveScreen("quest");
		m_questUIPanel.reset();
	}
	m_questText.reset();
	m_questProgressBar.reset();
	m_questProgressText.reset();

	Component::Finalize();
}

std::shared_ptr<UIPanel> QuestUI::GenerateTabPanel(int index)
{
	if(index < 0 || index >= kTabCount) return nullptr;
	const auto& panel = UIFactory::MakeUIToPanel<UIPanel>(m_helpPanel,
		kPanelPos,kPanelSize
		);
	panel.lock()->SetVisible(false);

	const auto textPos = kTabBarPos + Vector{0,kTabBarSize.m_y * 0.5f} + Vector{0,50};

	const auto& panelText = UIFactory::MakeUIToPanel<UIText>(panel,
		TextArgs{
			textPos,
			TextPivot::Center,
			FontId::kFactoryUITitle
		}
	);
	panelText.lock()->SetText("~ " + kTabTitle.at(index) + " ~");

	m_tabPanels.push_back(panel);
	return panel.lock();
}

void QuestUI::InitBaseContorollTab()
{
	const int index = 0;
	const auto& panel = GenerateTabPanel(index);
	const int count = 5;
	const Vector kFirstPos = Vector{kModalWindowPos.m_x - 200,400};
	const Vector kImageSize = Vector{60,60};
	const Vector kTextOffset = Vector{250};
	const Vector kOffset = Vector{0,20};
	
	const std::array<std::pair<GraphicId,std::string>,count> graphicAndDesc = 
	{
		std::make_pair(GraphicId::kKeyboardWASD,"カメラ移動"),
		std::make_pair(GraphicId::kMouseMoveWheel,"カメラ回転") ,
		std::make_pair(GraphicId::kMouseScrollVertical,"ズームイン・ズームアウト") ,
		std::make_pair(GraphicId::kKeyboardTab,"インベントリを開く"),
		std::make_pair(GraphicId::kMouseRight,"資源を取得する")
	};

	for (size_t i = 0; i < count; i++)
	{
		const auto pos = kFirstPos + (Vector{0,kImageSize.m_y} + kOffset) * static_cast<float>(i);

		const auto& [graphic, desc] = graphicAndDesc.at(i); 

		const auto& image = UIFactory::MakeUIToPanel<UIImage>(panel,
		pos,kImageSize,graphic
		);
		const auto& text = UIFactory::MakeUIToPanel<UIText>(panel,
			TextArgs{
			pos + kTextOffset
			}
		);
		text.lock()->SetText(desc);

	}

}

void QuestUI::InitInventoryContorollTab()
{
	const int index = 1;
	const auto& panel = GenerateTabPanel(index);

	const int count = 3;
	const Vector kFirstPos = Vector{kModalWindowPos.m_x - 500,300};
	const Vector kTextOffset = Vector{250};
	const Vector kOffset = Vector{0,20};
	
	struct temp{
	Vector imageSize;
	GraphicId graphicId;
	std::string text;
	};

	const std::array<temp,count> graphicAndDesc = 
	{
		temp{Vector{210,210},GraphicId::kHelpInventory,"自由に保管しておけるスペース(インベントリ)"},
		temp{Vector{190,220},GraphicId::kHelpCraft,"クラフト画面、黄色のレシピをクリックするとそのクラフトができる"},
		temp{Vector{240,100},GraphicId::kHelpClickToCancel,"レシピキュー、クラフトをキャンセルしたいときは、アイコンをクリック"}
	};

	auto pos = kFirstPos;
	for (size_t i = 0; i < count; i++)
	{
		const auto& [size, graphic, desc] = graphicAndDesc.at(i); 

		pos += Vector{0,size.m_y * 0.5f};

		const auto& image = UIFactory::MakeUIToPanel<UIImage>(panel,
		pos,size,graphic
		);
		const auto& text = UIFactory::MakeUIToPanel<UIText>(panel,
			TextArgs{
			pos + kTextOffset,
			TextPivot::LeftCenter
			}
		);
		text.lock()->SetText(desc);

		pos += (Vector{0,size.m_y * 0.5f} + kOffset);
	}
}

void QuestUI::InitInstallationTab()
{
	const int index = 2;
	const auto& panel = GenerateTabPanel(index);

	const int count = 2;
	const Vector kFirstPos = Vector{kModalWindowPos.m_x - 500,400};
	const Vector kTextOffset = Vector{250};
	const Vector kOffset = Vector{0,20};
	
	struct temp{
	Vector imageSize;
	GraphicId graphicId;
	std::string text;
	};

	const std::array<temp,count> graphicAndDesc = 
	{
		temp{Vector{250,250},GraphicId::kHelpInstallation0,"アイテムバーで選択状態のアイテムをマップで左クリックで設置できる"},
		temp{Vector{250,220},GraphicId::kHelpInstallation1,"ヒント:向きが重要な施設もあるから注意！"},
	};

	auto pos = kFirstPos;
	for (size_t i = 0; i < count; i++)
	{
		const auto& [size, graphic, desc] = graphicAndDesc.at(i); 

		pos += Vector{0,size.m_y * 0.5f};

		const auto& image = UIFactory::MakeUIToPanel<UIImage>(panel,
		pos,size,graphic
		);
		const auto& text = UIFactory::MakeUIToPanel<UIText>(panel,
			TextArgs{
			pos + kTextOffset,
			TextPivot::LeftCenter
			}
		);
		text.lock()->SetText(desc);

		pos += (Vector{0,size.m_y * 0.5f} + kOffset);
	}

}

void QuestUI::InitDestroyTab()
{
	const int index = 3;
	const auto& panel = GenerateTabPanel(index);
}

void QuestUI::SetAllPanelInvisible()
{
	if(m_tabPanels.empty()) return;
	for(const auto& panel : m_tabPanels)
	{
		if(const auto& safePanel = panel.lock())
			safePanel->SetVisible(false);
	}
}

void QuestUI::SetPanelVisible(int index, bool visible)
{
	if(index < 0 || index >= m_tabPanels.size()) return;
	const auto& panel = m_tabPanels.at(index);
	SetPanelVisible(panel,visible);
}

void QuestUI::SetPanelVisible(std::weak_ptr<UIPanel> panel, bool visible)
{
	SetAllPanelInvisible();
	if(const auto& safe = panel.lock())
	{
		safe->SetVisible(visible);
	}
}
