#include "SceneManager.h"
#include "Scene.h"
#include <assert.h>
void SceneManager::SetNextScene(const std::shared_ptr<Scene>& scene)
{
	m_nextScene = scene;
}
void SceneManager::CheckChangeScene()
{
	//次のシーンに設定されていなかったら処理を終了する
	if (!m_nextScene) return;
	ChangeScene();
}
void SceneManager::ChangeScene()
{
	//今のシーンのfinalize処理を実行する
	if (m_currentScene) m_currentScene->Finalize();

	//今のシーンの変数にムーブする
	m_currentScene = std::move(m_nextScene);

	m_currentScene->Init();

	//追加予約されたObjectを追加する。
	m_currentScene->AddToSceneObjectsFromReserved();
}

void SceneManager::AddObjectToCurrentScene(const std::shared_ptr<Object>& obj)
{
	m_currentScene->AddToReservedObjects(obj);
}
void SceneManager::AddColliderToCurrentScene(const std::shared_ptr<Collider>& col)
{
	m_currentScene->AddToSceneColliders(col);
}
void SceneManager::AddShapeToCurrentScene(const std::shared_ptr<Shape>& shp)
{
	m_currentScene->AddToSceneShapes(shp);
}
