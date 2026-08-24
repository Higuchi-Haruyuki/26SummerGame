#pragma once
#include "Object.h"
#include "SceneManager.h"
#include <concepts>
#include <memory>
#include <string>
#include "FactoryComponent.h"
#include "vector.h"
#include "VectorInt.h"
#include "Game.h"
namespace ObjectFactory
{
	/// <summary>
	/// オブジェクトを作成してそのインスタンスのポインタを返す
	/// </summary>
	/// <returns>オブジェクトのインスタンスのポインタ</returns>
	inline std::shared_ptr<Object> CreateObject()
	{
		std::shared_ptr<Object> newObject = std::make_shared<Object>();
		newObject->Init();
		SceneManager::GetInstance().AddObjectToCurrentScene(newObject);
		return newObject;
	}

	/// <summary>
	/// オブジェクトを作成してそのインスタンスのポインタを返す
	/// </summary>
	/// <param name="pos">作成する座標</param>
	/// <returns>オブジェクトのインスタンスのポインタ</returns>
	inline std::shared_ptr<Object> CreateObject(const Vector& pos)
	{
		std::shared_ptr<Object> newObject = CreateObject();
		newObject->SetPosition(pos);
		return newObject;
	}

	/// <summary>
	/// オブジェクトを作成してそのインスタンスのポインタを返す
	/// </summary>
	/// <param name="pos">作成する座標</param>
	/// <param name="tag">オブジェクトにつけるタグ</param>
	/// <returns>オブジェクトのインスタンスのポインタ</returns>
	inline std::shared_ptr<Object> CreateObject(const Vector& pos, const std::string& tag)
	{
		std::shared_ptr<Object> newObject = CreateObject(pos);
		newObject->SetTag(tag);
		return newObject;
	}

	/// <summary>
	/// ファクトリーコンポーネントを持つオブジェクトを生成する。
	/// </summary>
	/// <param name="gridPos">グリッド座標での位置</param>
	/// <returns>そのオブジェクトの共有ポインタ</returns>
	template<std::derived_from<FactoryComponent> T>
	inline std::shared_ptr<Object> CreateObject(const VectorInt& gridPos)
	{
		const auto& obj = ObjectFactory::CreateObject(Game::GridPosToWorldPos(gridPos));
		obj->AddComponent<T>();
		return obj;
	}

	/// <summary>
	/// ファクトリーコンポーネントを持つオブジェクトを生成する。
	/// </summary>
	/// <param name="gridPos">グリッド座標での位置</param>
	/// <param name="objTag">オブジェクトにつけるタグ</param>
	/// <returns>そのオブジェクトの共有ポインタ</returns>
	template<std::derived_from<FactoryComponent> T>
	inline std::shared_ptr<Object> CreateObject(const VectorInt& gridPos, const std::string& objTag)
	{
		const auto& obj = ObjectFactory::CreateObject(Game::GridPosToWorldPos(gridPos), objTag);
		obj->AddComponent<T>();
		return obj;
	}

	/// <summary>
	/// ファクトリーコンポーネントを持つオブジェクトを生成する。
	/// </summary>
	/// <param name="gridPos">グリッド座標での位置</param>
	/// <param name="objTag">オブジェクトにつけるタグ</param>
	/// <param name="rotationAngle">回転させる角度(単位はラジアン)</param>
	/// <returns>そのオブジェクトの共有ポインタ</returns>
	template<std::derived_from<FactoryComponent> T>
	inline std::shared_ptr<Object> CreateObject(const VectorInt& gridPos, const std::string& objTag, float rotationAngle)
	{
		const auto& obj = ObjectFactory::CreateObject(Game::GridPosToWorldPos(gridPos), objTag);
		const auto& facCom = obj->AddComponent<T>();
		facCom->SetRotationAngle(rotationAngle);
		return obj;
	}

//シーンが管理しないオブジェクトを作る

	/// <summary>
	/// オブジェクトを作成してそのインスタンスのポインタを返す。
	/// このオブジェクトはシーンの管理下におかれない。
	/// </summary>
	/// <returns>オブジェクトのインスタンスのポインタ</returns>
	inline std::shared_ptr<Object> CreateObjectNoSceneManage()
	{
		std::shared_ptr<Object> newObject = std::make_shared<Object>();
		newObject->Init();
		return newObject;
	}

	/// <summary>
	/// オブジェクトを作成してそのインスタンスのポインタを返す。
	/// このオブジェクトはシーンの管理下におかれない。
	/// </summary>
	/// <param name="pos">作成する座標</param>
	/// <returns>オブジェクトのインスタンスのポインタ</returns>
	inline std::shared_ptr<Object> CreateObjectNoSceneManage(const Vector& pos)
	{
		std::shared_ptr<Object> newObject = CreateObjectNoSceneManage();
		newObject->SetPosition(pos);
		return newObject;
	}

	/// <summary>
	/// オブジェクトを作成してそのインスタンスのポインタを返す。
	/// このオブジェクトはシーンの管理下におかれない。
	/// </summary>
	/// <param name="pos">作成する座標</param>
	/// <param name="tag">オブジェクトにつけるタグ</param>
	/// <returns>オブジェクトのインスタンスのポインタ</returns>
	inline std::shared_ptr<Object> CreateObjectNoSceneManage(const Vector& pos, const std::string& tag)
	{
		std::shared_ptr<Object> newObject = CreateObjectNoSceneManage(pos);
		newObject->SetTag(tag);
		return newObject;
	}

	/// <summary>
	/// ファクトリーコンポーネントを持つオブジェクトを生成する。
	/// このオブジェクトはシーンの管理下におかれない。
	/// </summary>
	/// <param name="gridPos">グリッド座標での位置</param>
	/// <returns>そのオブジェクトの共有ポインタ</returns>
	template<std::derived_from<FactoryComponent> T>
	inline std::shared_ptr<Object> CreateObjectNoSceneManage(const VectorInt& gridPos)
	{
		const auto& obj = ObjectFactory::CreateObjectNoSceneManage(Game::GridPosToWorldPos(gridPos));
		obj->AddComponent<T>();
		return obj;
	}

	/// <summary>
	/// ファクトリーコンポーネントを持つオブジェクトを生成する。
	/// このオブジェクトはシーンの管理下におかれない。
	/// </summary>
	/// <param name="gridPos">グリッド座標での位置</param>
	/// <param name="objTag">オブジェクトにつけるタグ</param>
	/// <returns>そのオブジェクトの共有ポインタ</returns>
	template<std::derived_from<FactoryComponent> T>
	inline std::shared_ptr<Object> CreateObjectNoSceneManage(const VectorInt& gridPos, const std::string& objTag)
	{
		const auto& obj = ObjectFactory::CreateObjectNoSceneManage(Game::GridPosToWorldPos(gridPos), objTag);
		obj->AddComponent<T>();
		return obj;
	}

	/// <summary>
	/// ファクトリーコンポーネントを持つオブジェクトを生成する。
	/// このオブジェクトはシーンの管理下におかれない。
	/// </summary>
	/// <param name="gridPos">グリッド座標での位置</param>
	/// <param name="objTag">オブジェクトにつけるタグ</param>
	/// <param name="rotationAngle">回転させる角度(単位はラジアン)</param>
	/// <returns>そのオブジェクトの共有ポインタ</returns>
	template<std::derived_from<FactoryComponent> T>
	inline std::shared_ptr<Object> CreateObjectNoSceneManage(const VectorInt& gridPos, const std::string& objTag, float rotationAngle)
	{
		const auto& obj = ObjectFactory::CreateObjectNoSceneManage(Game::GridPosToWorldPos(gridPos), objTag);
		const auto& facCom = obj->AddComponent<T>();
		facCom->SetRotationAngle(rotationAngle);
		return obj;
	}
}