#include "Shape.h"
#include "SceneManager.h"
#include "Scene.h"


void Shape::Init() {
	Component::Init();
	SceneManager::GetInstance().AddShapeToCurrentScene(std::dynamic_pointer_cast<Shape>(shared_from_this()));
}
void Shape::Update() {
	Component::Update();
}
void Shape::Finalize() {
	Component::Finalize();
	SceneManager::GetInstance().GetCurrentScene()->AddToRemoveShapes(std::dynamic_pointer_cast<Shape>(shared_from_this()));
}
void Shape::Draw() const {
}