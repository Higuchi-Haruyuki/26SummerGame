#include "ResourceManager.h"
#include "ResourceTable.h"
#include <DxLib.h>
#include <cassert>
#include <algorithm>

void ResourceManager::Init()
{
	for (const auto& info : ResourceTable::kFontTable)
	{
		int handle = CreateFontToHandle(info.FontName, info.Size, info.Thick, info.FontType);
		assert(handle != -1 && "フォントの読み込みに失敗しました");
		m_fontHandles[static_cast<size_t>(info.Id)] = handle;
	}

	for (const auto& info : ResourceTable::kGraphicTable)
	{
		int handle = LoadGraph(info.Path);
		assert(handle != -1 && "グラフィックの読み込みに失敗しました");
		m_graphicHandles[static_cast<size_t>(info.Id)] = handle;
	}

	for (const auto& info : ResourceTable::kModelTable)
	{
		int handle = MV1LoadModel(info.Path);
		assert(handle != -1 && "モデルの読み込みに失敗しました");
		MV1SetScale(handle, VGet(info.Scale, info.Scale, info.Scale));
		m_modelHandles[static_cast<size_t>(info.Id)] = handle;
	}

	for (const auto& info : ResourceTable::kSoundTable)
	{
		int handle = LoadSoundMem(info.Path);
		assert(handle != -1 && "サウンドの読み込みに失敗しました");

		auto volume = 255 * std::clamp(info.Volume,0.0f,1.0f) / 1.0f;

		ChangeVolumeSoundMem(static_cast<int>(volume), handle);
		m_soundHandles[static_cast<size_t>(info.Id)] = handle;
	}
}

void ResourceManager::Finalize()
{
	for (int handle : m_fontHandles)
	{
		DeleteFontToHandle(handle);
	}
	for (int handle : m_graphicHandles)
	{
		DeleteGraph(handle);
	}
	for (int handle : m_modelHandles)
	{
		MV1DeleteModel(handle);
	}
	for (int handle : m_soundHandles)
	{
		DeleteSoundMem(handle);
	}
}

int ResourceManager::GetGraphicSize(GraphicId id, int* width, int* height) const
{
	return GetGraphSize(GetHandle(id), width, height);
}
