#include "Scene.h"
#include <format>
#include <string.h>
#include <algorithm>
#include <memory>
#include <string>
#include <DxLib.h>
#include "Shape.h"
#include "Debug.h"
#include "Object.h"
#include "Collider.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Game.h"
#include "SquareCollider3D.h"
#include "FactoryManager.h"
#include "vector.h"
#include "Camera.h"
#include "ObjectFactory.h"
#include "StringUtil.h"

namespace
{
	constexpr int kGridNum = 159;
}
void Scene::Init()
{
	//カメラの初期化処理
	const auto& camera = ObjectFactory::CreateObject().lock();
	m_camera = camera->AddComponent<Camera>();

}
void Scene::Update()
{
	Debug::Log(std::format("FPS: {}", GetFPS()));

	if (m_sceneObjects.size() == 0) return;

	//ステージオブジェクトのUpdate処理
	for (const auto& object : m_sceneObjects)
	{
		if (IsRemoveReserved(object)) continue;
		object->Update();
	}


	auto objectCount = GetSceneObjects().size();
	Debug::Log("Object数: " + StringUtil::IntToString(objectCount));

	auto colliderCount = m_sceneColliders.size();
	Debug::Log("Collider数: " + StringUtil::IntToString(colliderCount));

	auto shapeCount = m_sceneShapes.size();
	Debug::Log("Shape数: " + StringUtil::IntToString(shapeCount));

	auto factoryCount = FactoryManager::GetInstance().GetFactoryComponentCount();
	Debug::Log("Factory数: " + StringUtil::IntToString(factoryCount));

}
void Scene::LateUpdate()
{
	//動的にUpdateで追加予約されたObjectを追加する。
	AddToSceneObjectsFromReserved();

	//ステージオブジェクトのLateUpdate処理
	for (const auto& object : m_sceneObjects)
	{
		if (IsRemoveReserved(object)) continue;
		object->LateUpdate();
	}

	//削除する配列にあるオブジェクトとコンポーネントを削除する。
	RemoveObjectsAndComponents();

	FactoryManager::GetInstance().RemoveFactoryComponents();
}
void Scene::Draw() const
{
	if (m_sceneShapes.size() != 0)
	{
		//ステージオブジェクトの描画処理
		for (const auto& shape : m_sceneShapes)
		{
			if (IsRemoveReserved(shape)) continue;
			shape->Draw();
		}
	}

	//デバック用のログを描画
	Debug::Draw();
}
void Scene::Finalize()
{
	for (const auto& obj : m_sceneObjects)
	{
		obj->Finalize();
	}
	m_sceneColliders.clear();
	m_sceneShapes.clear();
	m_sceneObjects.clear();
}

void Scene::SetCameraControll(bool isEnable)
{
	const auto& safeCamera = m_camera.lock();
	if (!safeCamera) return;
	safeCamera->SetEnableMovement(isEnable);
	safeCamera->SetEnableRotation(isEnable);
}

void Scene::AddToSceneObjectsFromReserved()
{
	for (auto& obj : m_reservedObjects)
	{
		if (!obj) continue;
		m_sceneObjects.push_back(std::move(obj));
	}
	m_reservedObjects.clear();
}

void Scene::AddToSceneColliders(const std::shared_ptr<Collider>& collider)
{
	//親オブジェクトがシーンの管理下におかれていないなら追加しない。
	if (!SceneManager::GetInstance().GetCurrentScene()->HasSceneObjects(collider->GetParentObject())) return;

	if (HasSceneColliders(collider)) return;
	m_sceneColliders.push_back(collider);
}
void Scene::AddToSceneShapes(const std::shared_ptr<Shape>& shape)
{
	//親オブジェクトがシーンの管理下におかれていないなら追加しない。
	if (!SceneManager::GetInstance().GetCurrentScene()->HasSceneObjects(shape->GetParentObject())) return;

	if (HasSceneShapes(shape)) return;
	m_sceneShapes.push_back(shape);
}

void Scene::AddToReservedObjects(const std::shared_ptr<Object>& object)
{
	if (HasSceneObjects(object)) return;
	m_reservedObjects.push_back(object);
	//そのオブジェクトが持つ子オブジェクトまで再帰的に追加する
	for (const auto& childObj : object->GetChildObjects())
	{
		AddToReservedObjects(childObj);
	}
}

void Scene::AddToRemoveObjects(std::shared_ptr<Object> object)
{
	if (!HasSceneObjects(object)) return;
	m_removeSceneObjects.emplace(object);
}

void Scene::AddToRemoveColliders(std::shared_ptr<Collider> collider)
{
	if (!HasSceneColliders(collider)) return;
	m_removeSceneColliders.emplace(collider);
}

void Scene::AddToRemoveShapes(std::shared_ptr<Shape> shape)
{
	if (!HasSceneShapes(shape)) return;
	m_removeSceneShapes.emplace(shape);
}

void Scene::MoveSceneObject(int targetIndex, std::shared_ptr<Object> object)
{
	if (targetIndex >= m_sceneObjects.size() || targetIndex < 0) return;
	int index = -1;
	for (int i = 0; i < m_sceneObjects.size(); i++)
	{
		//目的のオブジェクトでないとき次のループにする
		if (m_sceneObjects[i] != object) continue;

		//目的のオブジェクトのインデックスを保存してループを終了
		index = i;
		break;
	}

	//見つからなかったとき関数を終了
	if (index == -1) return;
	//移動前と移動先のインデックスが同じ時関数を終了
	if (index == targetIndex) return;

	// 要素を「後ろ」に移動する場合
	if (index < targetIndex)
	{
		std::rotate(
			m_sceneObjects.begin() + index,
			m_sceneObjects.begin() + index + 1,
			m_sceneObjects.begin() + targetIndex + 1
		);
	}
	// 要素を「前」に移動する場合
	else
	{
		auto first = m_sceneObjects.begin() + targetIndex;
		auto middle = m_sceneObjects.begin() + index;
		auto end = m_sceneObjects.begin() + index + 1;
		std::rotate(
			first,
			middle,
			end
		);
	}
	MoveSceneCollider(targetIndex, m_sceneObjects[targetIndex]->GetComponent<Collider>().lock());
	MoveSceneShape(targetIndex, m_sceneObjects[targetIndex]->GetComponent<Shape>().lock());
}

void Scene::RemoveObjectsAndComponents()
{
	std::erase_if(m_sceneObjects, [this](const auto& obj) {return m_removeSceneObjects.contains(obj); });
	std::erase_if(m_sceneColliders, [this](const auto& col) {return m_removeSceneColliders.contains(col); });
	std::erase_if(m_sceneShapes, [this](const auto& shape) {return m_removeSceneShapes.contains(shape); });

	m_removeSceneObjects.clear();
	m_removeSceneColliders.clear();
	m_removeSceneShapes.clear();
}

void Scene::MoveSceneCollider(int targetIndex, std::shared_ptr<Collider> collider)
{
	if (targetIndex >= m_sceneColliders.size() || targetIndex < 0) return;
	int index = -1;
	for (int i = 0; i < m_sceneColliders.size(); i++)
	{
		//目的のオブジェクトでないとき次のループにする
		if (m_sceneColliders[i] != collider) continue;

		//目的のオブジェクトのインデックスを保存してループを終了
		index = i;
		break;
	}

	//見つからなかったとき関数を終了
	if (index == -1) return;
	//移動前と移動先のインデックスが同じ時関数を終了
	if (index == targetIndex) return;

	// 要素を「後ろ」に移動する場合
	if (index < targetIndex)
	{
		std::rotate(
			m_sceneColliders.begin() + index,
			m_sceneColliders.begin() + index + 1,
			m_sceneColliders.begin() + targetIndex + 1
		);
	}
	// 要素を「前」に移動する場合
	else
	{
		auto first = m_sceneColliders.begin() + targetIndex;
		auto middle = m_sceneColliders.begin() + index;
		auto end = m_sceneColliders.begin() + index + 1;
		std::rotate(
			first,
			middle,
			end
		);
	}
}
void Scene::MoveSceneShape(int targetIndex, std::shared_ptr<Shape> shape)
{
	if (targetIndex >= m_sceneShapes.size() || targetIndex < 0) return;
	int index = -1;
	for (int i = 0; i < m_sceneShapes.size(); i++)
	{
		//目的のオブジェクトでないとき次のループにする
		if (m_sceneShapes[i] != shape) continue;

		//目的のオブジェクトのインデックスを保存してループを終了
		index = i;
		break;
	}

	//見つからなかったとき関数を終了
	if (index == -1) return;
	//移動前と移動先のインデックスが同じ時関数を終了
	if (index == targetIndex) return;

	// 要素を「後ろ」に移動する場合
	if (index < targetIndex)
	{
		std::rotate(
			m_sceneShapes.begin() + index,
			m_sceneShapes.begin() + index + 1,
			m_sceneShapes.begin() + targetIndex + 1
		);
	}
	// 要素を「前」に移動する場合
	else
	{
		auto first = m_sceneShapes.begin() + targetIndex;
		auto middle = m_sceneShapes.begin() + index;
		auto end = m_sceneShapes.begin() + index + 1;
		std::rotate(
			first,
			middle,
			end
		);
	}
}

void Scene::DrawGrid() const
{
	//Z軸方向の線
	const float lineStartX = Game::kGridSize * -(kGridNum * 0.5f);
	const float lineEndX = -lineStartX;
	for (int z = 0; z <= kGridNum; z++)
	{
		const float lineZ = Game::kGridSize * z - Game::kGridSize * kGridNum * 0.5f;
		DrawLine3D(VGet(lineStartX, 5, lineZ), VGet(lineEndX, 5, lineZ),0xffffff);
	}

	//X軸方向の線
	const float lineStartZ = Game::kGridSize * -(kGridNum * 0.5f);
	const float lineEndZ = -lineStartZ;
	for (int x = 0; x <= kGridNum; x++)
	{
		const float lineX = Game::kGridSize * x - Game::kGridSize * kGridNum * 0.5f;
		DrawLine3D(VGet(lineX, 5, lineStartZ), VGet(lineX, 5, lineEndZ), 0xffffff);
	}
}
