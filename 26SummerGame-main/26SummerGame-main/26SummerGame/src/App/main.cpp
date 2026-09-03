#include "DxLib.h"
#include "MainScene.h"
#include "SceneManager.h"
#include "PlayerInput.h"
#include "ResourceManager.h"
#include "Game.h"
#include "UIManager.h"
#include "TitleScene.h"
#include <memory>
#include "TitleScene.h"
#include "resource.h"
// プログラムは WinMain から始まります
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{

	SetWindowText("てのひらファクトリー");
	SetGraphMode(static_cast<int>(Game::kDisplaySize.m_x), static_cast<int>(Game::kDisplaySize.m_y), 32);	// ウィンドウの大きさとカラービット数の指定

#if _DEBUG
	ChangeWindowMode(1);
#else
	ChangeWindowMode(0);
#endif

	SetWindowIconID(IDI_ICON1);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}
	
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);	// 描画面を裏画面にする

	//フォーカスを外しても動き続けるようにする。
	SetAlwaysRunFlag(1);

	auto& resourceManager = ResourceManager::GetInstance();
	//リソースマネージャーの初期化処理
	resourceManager.Init();

	auto& sceneManager = SceneManager::GetInstance();
	//シーンをタイトルシーンに設定する
	sceneManager.SetNextScene(std::make_shared<TitleScene>());
	sceneManager.CheckChangeScene();

	auto& uiManager = UIManager::GetInstance();

	PlayerInput::GetInstance().Init();

	while (1) {
		ClearDrawScreen(); // 画面をクリアする

		LONGLONG startTime = GetNowHiPerformanceCount();

		//Update処理
		PlayerInput::GetInstance().Update(1);

		//シーンの更新処理
		sceneManager.GetCurrentScene()->Update();

		//UIの更新処理
		uiManager.Update(1);

		//update後の処理
		PlayerInput::GetInstance().LateUpdate();
		sceneManager.GetCurrentScene()->LateUpdate();

		//描画処理
		sceneManager.GetCurrentScene()->Draw();

		uiManager.Draw();

		ScreenFlip();	// 裏画面の内容を表画面に反映させる

		//次のシーンが設定されているかチェックする
		sceneManager.CheckChangeScene();

		//経過時間が指定した時間以下のとき待つ
		while ((GetNowHiPerformanceCount() - startTime) <= Game::kTimePerFrame) continue;

		if (ProcessMessage() == -1) break;	// Windowsから情報を受けとり、エラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_F1) == 1) break;
	}				
	sceneManager.GetCurrentScene()->Finalize();
	resourceManager.Finalize();

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}