#include "Quest_AxisInput.h"
#include "PlayerInput.h"

Quest_AxisInput::Quest_AxisInput(
    const std::string &name, const std::string &description, 
    const std::vector<std::string>& inputActions, float clearThreshold):
    Base_Quest(name,description),
    m_playerInput(PlayerInput::GetInstance()),
    m_inputActions(inputActions),
    m_clearThreshold(clearThreshold)
{
}

Quest_AxisInput::~Quest_AxisInput() = default;

bool Quest_AxisInput::ClearCheck()
{
    for(const auto& inputAction: m_inputActions)
    {
        if(const auto inputValue = m_playerInput.GetAction(inputAction)->GetValue().AsAxis())
        {
            const auto abs = std::abs(inputValue);
            m_clearProgress += abs;
        }
        if(m_clearProgress >= m_clearThreshold) 
        {
            return true;
        }  
    }

    return false;
}

float Quest_AxisInput::GetProgress() const
{
    return m_clearProgress / m_clearThreshold;
}

