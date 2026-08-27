#pragma once
#include <memory>
#include <string>
class Collider;
class Object;
class Shape;
class Scene;
class SceneManager
{
public:
	/// <summary>
	/// シングルトンインスタンスを取得
	/// </summary>
	/// <returns></returns>
	static SceneManager& GetInstance()
	{
		static SceneManager instance;
		return instance;
	}
	//現在のシーンを取得する
	std::shared_ptr<Scene> GetCurrentScene() { return m_currentScene; }

	/// <summary>
	/// 次のシーンに設定する。そのフレームの終わりでそのシーンがロードされる。
	/// </summary>
	/// <param name="scene"></param>
	void SetNextScene(std::shared_ptr<Scene> scene);

	/// <summary>
	/// 現在のシーンのオブジェクトの配列にオブジェクトを追加する
	/// </summary>
	void AddObjectToCurrentScene(std::shared_ptr<Object> obj);

	/// <summary>
	/// 現在のシーンのColliderの配列にColliderを追加する
	/// </summary>
	void AddColliderToCurrentScene(std::shared_ptr<Collider> col);

	/// <summary>
	/// 現在のシーンのShapeの配列にShapeを追加する
	/// </summary>
	void AddShapeToCurrentScene(std::shared_ptr<Shape> shp);

	/// <summary>
	/// 次のシーンに設定されているかチェックする
	/// </summary>
	void CheckChangeScene();
private:
	//シングルトン
	SceneManager() = default;
	~SceneManager() = default;

	//コピー禁止
	SceneManager(const Scene&) = delete;
	SceneManager& operator = (const SceneManager&) = delete;

	//ムーブ禁止
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator = (SceneManager&&) = delete;

	/// <summary>
	/// シーンを変更する
	/// </summary>
	void ChangeScene();

private:
	//今のシーン
	std::shared_ptr<Scene> m_currentScene;
	//次のシーン
	std::shared_ptr<Scene> m_nextScene;
};