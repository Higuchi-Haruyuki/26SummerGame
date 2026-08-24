#include "Component.h"
#include "Object.h"
void Component::Init()
{
	m_timer = 0;
	m_isEnable = true;
}
void Component::Update(float deltaTime) {
	m_timer += deltaTime;
	if (!m_isEnable) return;
}
void Component::LateUpdate()
{
	if (!m_isEnable) return;
}
void Component::Finalize() { m_parentObject.reset(); }
void Component::OnCollisionStay(const HitPoint& hitPoint, const std::shared_ptr<Object> object)
{

}
void Component::OnCollisionEnter(const HitPoint& hitPoint, const std::shared_ptr<Object> object)
{

}
void Component::OnCollisionExit(const HitPoint& hitPoint, const std::shared_ptr<Object> object)
{

}