#pragma once
#include <array>
#include <string>
#include <unordered_map>

#include "ResourceType.h"

class ResourceManager
{
public:
	/// <summary>
	/// インスタンスを取得する
	/// </summary>
	/// <returns></returns>
	static ResourceManager& GetInstance()
	{
		static ResourceManager instance;
		return instance;
	}

	void Init();
	void Finalize();

	/// <summary>
	/// フォントのハンドルを取得する
	/// </summary>
	int GetHandle(FontId id) const 
	{ 
		if (id == FontId::kNone) return -1;
		return m_fontHandles[static_cast<size_t>(id)]; 
	}

	/// <summary>
	/// グラフィックのハンドルを取得する
	/// </summary>
	int GetHandle(GraphicId id) const 
	{ 
		if (id == GraphicId::kNone) return -1;
		return m_graphicHandles[static_cast<size_t>(id)]; 
	}

	/// <summary>
	/// モデルのハンドルを取得する
	/// </summary>
	int GetHandle(ModelId id) const 
	{ 
		if (id == ModelId::kNone) return -1;
		return m_modelHandles[static_cast<size_t>(id)]; 
	}

	int GetHandle(SoundId id) const
	{
		if (id == SoundId::kNone) return -1;
		return m_soundHandles[static_cast<size_t>(id)];
	}

	/// <summary>
	/// グラフィックのサイズを取得する
	/// </summary>
	int GetGraphicSize(GraphicId id, int* width, int* height) const;

private:
	//シングルトン
	ResourceManager() = default;
	~ResourceManager() = default;

	//コピー禁止
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator = (const ResourceManager&) = delete;

	//ムーブ禁止
	ResourceManager(ResourceManager&&) = delete;
	ResourceManager& operator = (ResourceManager&&) = delete;

private:
	std::array<int, static_cast<size_t>(FontId::kCount)> m_fontHandles{};
	std::array<int, static_cast<size_t>(GraphicId::kCount)> m_graphicHandles{};
	std::array<int, static_cast<size_t>(ModelId::kCount)> m_modelHandles{};
	std::array<int, static_cast<size_t>(SoundId::kCount)> m_soundHandles{};

};
