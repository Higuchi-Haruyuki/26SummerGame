#include "Object.h"
#include "Component.h"
#include "HitPoint.h"
#include "SceneManager.h"
#include "Game.h"
#include "Scene.h"


void Object::Init() {
	m_isEnable = true;

	m_gridPosition = Game::WorldPosToGridPos(GetPosition());
}
void Object::Update() {
	if (!m_isEnable) return;
	for (const auto& component : m_components) 
	{
		component->Update();
	}

}
void Object::Finalize()
{
	for (const auto& component : m_components)
	{
		component->Finalize();
	}
	m_components.clear();
	m_childObjects.clear();
	SceneManager::GetInstance().GetCurrentScene()->AddToRemoveObjects(shared_from_this());
}
void Object::Destroy()
{
	Finalize();
}

void Object::SetPosition(const Vector& position)
{
	m_position = position;
	m_gridPosition = Game::WorldPosToGridPos(GetPosition());
}
void Object::LateUpdate()
{
	if (!m_isEnable) return;
	for (const auto& component : m_components)
	{
		component->LateUpdate();
	}
}
void Object::OnCollisionStay(const HitPoint& hitPoint, const std::weak_ptr<Object> object)
{
	//コンポーネントの該当関数を呼び出す
	for (const auto& component : m_components)
	{
		component->OnCollisionStay(hitPoint,object);
	}
}
void Object::OnCollisionEnter(const HitPoint& hitPoint, const std::weak_ptr<Object> object)
{
	//コンポーネントの該当関数を呼び出す
	for (const auto& component : m_components)
	{
		component->OnCollisionEnter(hitPoint, object);
	}
}
void Object::OnCollisionExit(const HitPoint& hitPoint, const std::weak_ptr<Object> object)
{
	//コンポーネントの該当関数を呼び出す
	for (const auto& component : m_components)
	{
		component->OnCollisionExit(hitPoint, object);
	}
}
