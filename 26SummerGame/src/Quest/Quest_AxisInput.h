#pragma once
#include "Base_Quest.h"
#include <string>
#include <vector>


class PlayerInput;

enum class InputValueType;

/// @brief プレイヤーの入力を要求するクエスト
class Quest_AxisInput : public Base_Quest
{
public:
    Quest_AxisInput(const std::string& name, const std::string& description,const std::vector<std::string>& inputActions, float clearThreshold);
    ~Quest_AxisInput();

    /*基底クラスのoverride*/
    
    bool ClearCheck() override;

    float GetProgress() const override;

private:
    /*シングルトンへの参照*/
    PlayerInput& m_playerInput;

    std::vector<std::string> m_inputActions = {};

    float m_clearThreshold = 0.0f;

    float m_clearProgress =0.0f;
};