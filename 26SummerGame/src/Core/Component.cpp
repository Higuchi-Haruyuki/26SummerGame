#include "Component.h"
#include "Object.h"
#include "vector.h"

void Component::Init()
{
	m_isEnable = true;
}
void Component::Update() {
	if (!m_isEnable) return;
}
void Component::LateUpdate()
{
	if (!m_isEnable) return;
}
void Component::Finalize() { m_parentObject.reset(); }

Vector Component::GetPosition() const
{
	const auto& parent = m_parentObject.lock();
	if (!parent) {
		assert(false && "親オブジェクトないです");
		return Vector{};
	}
	return parent->GetPosition();
}

void Component::SetPosition(const Vector& newPos)
{
	const auto& parent = m_parentObject.lock();
	if (!parent) {
		assert(false && "親オブジェクトないです");
		return;
	}
	return parent->SetPosition(newPos);
}

void Component::OnCollisionStay(const HitPoint& hitPoint, const std::weak_ptr<Object> object)
{

}

void Component::OnCollisionEnter(const HitPoint& hitPoint, const std::weak_ptr<Object> object)
{

}

void Component::OnCollisionExit(const HitPoint& hitPoint, const std::weak_ptr<Object> object)
{

}