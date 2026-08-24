#pragma once
#include <vector>
#include <string>
class Vector;
class Debug
{
public : 
	static void Draw();
	static void Log(const std::string& text) 
	{ 
		if (GetSize() > m_debugLogMaxLine)
		{
			m_debugLog.erase(m_debugLog.begin());
		}
		m_debugLog.push_back(text);
	}
	static std::string GetLog(int idx) 
	{ 
		return idx < GetSize() ? m_debugLog[idx] : "NULL";
	}
	static size_t GetSize() { return m_debugLog.size(); }
	static void SetDebugLogMaxLine(int num) { m_debugLogMaxLine = num; }
	static void DrawLine();

private:
	//デバッグログを描画する関数
	static void DrawDebugLog();
	static void ClearLog()
	{
		m_debugLog.clear();
	}
private:
	static std::vector<std::string> m_debugLog;
	static int m_debugLogMaxLine;
};