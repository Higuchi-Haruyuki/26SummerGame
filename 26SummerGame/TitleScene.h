#pragma once
#include "Scene.h"
#include <memory>
#include <array>

class UIPanel;
class UIManager;
class UISquare;
class UIText;
class SceneManager;
class PlayerInput;


class TitleScene :
    public Scene
{
public:
    TitleScene();
    ~TitleScene();

    void Init() override;
    void Update() override;

    

private:
    void BuildUI();

    void UpdateUI();

    void InputAction();

private:
    //シングルトンへの参照
    UIManager& m_uiManager;
    SceneManager& m_sceneManager;
    PlayerInput& m_playerInput;
    
    std::shared_ptr<UIPanel> m_mainUIPanel;
 

    const static int kChoiceCount = 2;
    std::array<std::pair<std::weak_ptr<UISquare>, std::weak_ptr<UIText>>, kChoiceCount> m_choiceBox;
};

