#include "Timer.h"
#include <DxLib.h>


Timer::Timer(Second durationSec):
	m_duration(ToMiliSecond(durationSec)),
	m_startTime(GetNowCount())
{
}

bool Timer::IsTimeOver()
{
	if (!m_isEnable) return false;
	const MiliSecond currentTime = GetNowCount();
	if (currentTime < m_startTime + m_duration)
	{
		m_isTimerOverLastFrame = false;
		return false;
	}

	if (!m_isTimerOverLastFrame) {
		m_onFinished.Invoke();
	}


	m_isTimerOverLastFrame = true;
	return true;
}

void Timer::ResetStartTime()
{
	m_startTime = GetNowCount();
}

Second Timer::GetElapsedTime() const
{
	const MiliSecond currentTime = GetNowCount();
	const MiliSecond erapsedMiliSec = currentTime - m_startTime;
	return ToSecond(erapsedMiliSec);
}
