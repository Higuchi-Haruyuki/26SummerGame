#pragma once
#include "vector.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

class Shape;
class Collider;
class Camera;
class Object;
class TileMap;
class Scene : public std::enable_shared_from_this<Scene>
{
public:
	Scene(){};
	//コンストラクタとは別に作成時に実行される
	virtual void Init();

	//毎フレーム実行
	virtual void Update(float deltaTime);

	//毎フレームUpdateのあとに実行
	virtual void LateUpdate();

	//毎フレーム実行され、描画処理
	virtual void Draw() const;

	//デストラクタとは別にオブジェクト破棄じに実行される
	virtual void Finalize();

	//GETTER
	float GetTimer() const { return m_timer; }
	std::vector<std::shared_ptr<Object>> GetSceneObjects() const { return m_sceneObjects; }
	std::vector<std::shared_ptr<Shape>> GetSceneShapes()  const { return m_sceneShapes; }
	std::vector<std::shared_ptr<Collider>> GetSceneColliders() const { return m_sceneColliders; }

	//ADD

	/// <summary>
	/// Reserved配列にあるObjectをSceneObject配列に移動する。
	/// </summary>
	void AddToSceneObjectsFromReserved();

	void AddToSceneColliders(const std::shared_ptr<Collider>& collider);

	void AddToSceneShapes(const std::shared_ptr<Shape>& shape);

	void AddToReservedObjects(const std::shared_ptr<Object>& object);
	
	/// <summary>
	/// 削除するオブジェクトを配列に追加する。
	/// </summary>
	/// <param name="object"></param>
	void AddToRemoveObjects(const std::shared_ptr<Object>& object);

	/// <summary>
	/// 削除するColliderを配列に追加する。
	/// </summary>
	/// <param name="object"></param>
	void AddToRemoveColliders(const std::shared_ptr<Collider>& collider);

	/// <summary>
	/// 削除するShapeを配列に追加する。
	/// </summary>
	/// <param name="object"></param>
	void AddToRemoveShapes(const std::shared_ptr<Shape>& shape);

	/// <summary>
	/// 指定したオブジェクトがすでにあれば指定したインデックスに移動する(頻繁に呼ばないこと)
	/// </summary>
	/// <param name="targetIndex">移動先のインデックス</param>
	/// <param name="object">移動させたいオブジェクトのスマートポインタ</param>
	void MoveSceneObject(const int& targetIndex, const std::shared_ptr<Object>& object);

	//HAS
	bool HasSceneObjects(const std::shared_ptr<Object>& object) const
	{
		for (const auto& obj : m_sceneObjects)
		{
			if (object == obj)
			{
				return m_removeSceneObjects.contains(object);
			}
		}
		for (const auto& obj : m_reservedObjects)
			if (object == obj) return true;
		return false;
	}
	bool HasSceneColliders(const std::shared_ptr<Collider>& collider) const
	{
		for (const auto& obj : m_sceneColliders)
		{
			if (collider == obj)
			{
				return m_removeSceneColliders.contains(collider);
			}
		}

		return false;
	}
	bool HasSceneShapes(const std::shared_ptr<Shape>& shape) const
	{
		for (const auto& obj : m_sceneShapes)
		{
			if (shape == obj)
			{
				return m_removeSceneShapes.contains(shape);
			}
		}
			
		return false;
	}

	bool IsRemoveReserved(const std::shared_ptr<Object>& object) const
	{
		return m_removeSceneObjects.contains(object);
	}
	bool IsRemoveReserved(const std::shared_ptr<Shape>& object) const
	{
		return m_removeSceneShapes.contains(object);
	}
	bool IsRemoveReserved(const std::shared_ptr<Collider>& object) const
	{
		return m_removeSceneColliders.contains(object);
	}

	//REMOVE

	/// <summary>
	/// 削除する配列にあるオブジェクトとコンポーネントを削除する。
	/// </summary>
	void RemoveObjectsAndComponents();

protected:
	void DrawStringCenter(const Vector& pos, std::string text, const int& size, const unsigned int& color, const int& fontHandle) const;

	/// <summary>
	/// グリッドを描画する
	/// </summary>
	void DrawGrid() const;

protected:
	//そのシーン内に存在するすべてのShapeが格納されている配列
	std::vector<std::shared_ptr<Shape>> m_sceneShapes;
	//そのシーン内に存在するすべてのColliderが格納されている配列
	std::vector<std::shared_ptr<Collider>> m_sceneColliders;
	float m_timer = 0;

private:

	/// <summary>
	/// 指定したColldierがすでにあれば指定したインデックスに移動する
	/// </summary>
	/// <param name="targetIndex">移動先のインデックス</param>
	/// <param name="collider">移動させたいColliderのスマートポインタ</param>
	void MoveSceneCollider(const int& targetIndex, const std::shared_ptr<Collider>& collider);

	/// <summary>
	/// 指定したShapeがすでにあれば指定したインデックスに移動する
	/// </summary>
	/// <param name="targetIndex">移動先のインデックス</param>
	/// <param name="shape">移動させたいShapeのスマートポインタ</param>
	void MoveSceneShape(const int& targetIndex, const std::shared_ptr<Shape>& shape);


private:
	//そのシーン内に存在するすべてのObjectが格納されている配列
	std::vector<std::shared_ptr<Object>> m_sceneObjects;

	//実行中に新たに追加するObjectの配列
	std::vector<std::shared_ptr<Object>> m_reservedObjects;

	//削除するオブジェクトの配列
	std::unordered_set<std::shared_ptr<Object>> m_removeSceneObjects;

	//削除するShapeの配列
	std::unordered_set<std::shared_ptr<Shape>> m_removeSceneShapes;

	//削除するColliderの配列
	std::unordered_set<std::shared_ptr<Collider>> m_removeSceneColliders;
};