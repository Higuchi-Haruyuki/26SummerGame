#include "DxLib.h"
#include "MainScene.h"
#include "SceneManager.h"
#include "PlayerInput.h"
#include "ResourceManager.h"
#include "Game.h"
#include "UIManager.h"
#include <memory>
// プログラムは WinMain から始まります
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{

#if false
	// exe自身のパスを取得し、そのディレクトリをカレントディレクトリに設定する。
  // これにより「exeがどこに置かれても」相対パスでのアセット読み込みが安定する。
	char exePath[MAX_PATH];
	GetModuleFileNameA(nullptr, exePath, MAX_PATH);

	std::string exeDir(exePath);
	size_t pos = exeDir.find_last_of("\\/");
	if (pos != std::string::npos) {
		exeDir = exeDir.substr(0, pos);
	}
	SetCurrentDirectoryA(exeDir.c_str());
#endif

	SetWindowText("夏制作");
	SetGraphMode(static_cast<int>(Game::kDisplaySize.m_x), static_cast<int>(Game::kDisplaySize.m_y), 32);	// ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true);
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
	sceneManager.SetNextScene(std::make_shared<MainScene>());
	sceneManager.CheckChangeScene();

	auto& uiManager = UIManager::GetInstance();

	PlayerInput::GetInstance().Init();

	while (1) {
		ClearDrawScreen(); // 画面をクリアする

		LONGLONG startTime = GetNowHiPerformanceCount();

		//Update処理
		PlayerInput::GetInstance().Update(1);
		//UIの更新処理
		uiManager.Update(1);

		//シーンの更新処理
		sceneManager.GetCurrentScene()->Update();

		//update後の処理
		PlayerInput::GetInstance().LateUpdate();
		sceneManager.GetCurrentScene()->LateUpdate();

		//描画処理
		sceneManager.GetCurrentScene()->Draw();

		uiManager.Draw();

		ScreenFlip();	// 裏画面の内容を表画面に反映させる

		//次のシーンが設定されているかチェックする
		//sceneManager.CheckChangeScene();

		//経過時間が指定した時間以下のとき待つ
		while ((GetNowHiPerformanceCount() - startTime) <= Game::kTimePerFrame) continue;

		if (ProcessMessage() == -1) break;	// Windowsから情報を受けとり、エラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
	}				
	sceneManager.GetCurrentScene()->Finalize();
	resourceManager.Finalize();

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}