#pragma once
#include "Scene.h"
#include "Square3D.h"
#include "vector.h"

class Animation3D;
class Model;
class CharactorStateManager;
class MapManager;

class MainScene : public Scene
{
public:
	MainScene();
	~MainScene();
	void Init() override;
	void Update() override;
	void Draw() const override;

private:
	MapManager& m_mapManager;

	std::weak_ptr<Object> m_player;

	std::weak_ptr<Object> m_skyDome;

	std::weak_ptr<CharactorStateManager> m_stateManager;


};
