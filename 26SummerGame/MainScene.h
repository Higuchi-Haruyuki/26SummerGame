#pragma once
#include "Scene.h"
#include "Square3D.h"
#include "vector.h"
class Animation3D;
class Model;
class MainScene : public Scene
{
public:
	MainScene() : Scene() {};
	void Init() override;
	void Update(float deltaTime) override;

private:
	std::shared_ptr<Object> m_player;
	std::shared_ptr<Collider> m_playerCollider;

	std::weak_ptr<Object> m_skyDome;
};
