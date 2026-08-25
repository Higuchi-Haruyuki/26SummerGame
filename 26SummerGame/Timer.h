#pragma once
#include "Event.h"

using Second = float;
using MiliSecond = int;

class Timer
{
public:
	Timer(Second durationSec);
	~Timer() {};

	bool IsTimeOver();

	MiliSecond ToMiliSecond(Second sec) const
	{
		return static_cast<int>(sec * 1000);
	}
	Second ToSecond(MiliSecond miliSec) const
	{
		return static_cast<float>(miliSec) * 0.001f;
	}

	/// <summary>
	/// 開始時間を今の時間に設定する。
	/// </summary>
	void ResetStartTime();

	void SetDuration(Second sec)
	{
		m_duration = ToMiliSecond(sec);
		ResetStartTime();
	}

	void SetEnable(bool isEnable) 
	{ 
		if (!m_isEnable && isEnable)
			ResetStartTime();

		m_isEnable = isEnable; 
	}

	bool IsEnable() const { return m_isEnable; }

	/// <summary>
	/// 現在の経過秒を取得
	/// </summary>
	/// <returns></returns>
	Second GetElapsedTime() const;

	void SubscribeOnFinished(const std::function<void()>& onBegin)
	{
		m_onFinishedConnection.emplace_back(m_onFinished.AddListener(onBegin));
	}

private:
	bool m_isEnable = true;
	MiliSecond m_startTime = 0;
	MiliSecond m_duration = 0;

	bool m_isTimerOverLastFrame = false;

	//終了時のコールバック
	std::vector<Event<>::Connection> m_onFinishedConnection;
	Event<> m_onFinished;

};

