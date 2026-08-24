#include "Debug.h"
#include <DxLib.h>
#include "vector.h"
namespace
{
	constexpr float kLineLength = 2000;
}
std::vector<std::string> Debug::m_debugLog;
int Debug::m_debugLogMaxLine = 32;
void Debug::Draw()
{
	DrawDebugLog();
	DrawLine();
}
void Debug::DrawLine()
{
	DrawLine3D(VGet(0, 0, 0), VGet(kLineLength, 0, 0),0xff0000);
	DrawLine3D(VGet(0, 0, 0), VGet(0, kLineLength, 0), 0x00ff00);
	DrawLine3D(VGet(0, 0, 0), VGet(0, 0, kLineLength), 0x0000ff);
}
void Debug::DrawDebugLog()
{
	//デバック用のログを描画
	for (int i = 0; i < Debug::GetSize(); i++)
	{
		DrawString(0, i * 15, Debug::GetLog(i).c_str(), 0xffffff);
	}
	ClearLog();
}