#include "Timer.h"
#include <DxLib.h>


Timer::Timer(Second durationSec):
	m_duration(ToMiliSecond(durationSec)),
	m_startTime(GetNowCount())
{
}

bool Timer::IsTimeOver() const
{
	if (!m_isEnable) return false;
	const MiliSecond currentTime = GetNowCount();
	if (currentTime < m_startTime + m_duration) return false;
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
