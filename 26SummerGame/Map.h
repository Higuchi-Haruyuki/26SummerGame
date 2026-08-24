#pragma once
#include "Game.h"
#include <array>
#include <cassert>

enum class Item;

using Chunk = std::array<std::array<Item, Game::kChankSize>, Game::kChankSize>;

namespace
{
	/// <summary>
	/// ワールドのチャンクの一辺の数
	/// </summary>
	constexpr int kWorldSize = 5;

	/// <summary>
	/// ワールドのチャンクの数
	/// </summary>
	constexpr int kChunkCount = kWorldSize * kWorldSize;
}

class Map
{
public:
	Map() {};
	~Map() {};

	std::array<Chunk, kChunkCount>& GetMap() { return m_map; }

	Chunk& GetChunk(int chunkX, int chunkZ)
	{ 
		return m_map[ToIndex(chunkX, chunkZ)];
	}

	const Chunk& GetChunk(int chunkX, int chunkZ) const
	{
		return m_map[ToIndex(chunkX,chunkZ)];
	}

	void SetMap(const std::array<Chunk, kChunkCount>& map)
	{
		m_map = map;
	}

	// チャンク座標が有効範囲内かどうかを判定する
	bool IsInBounds(int chunkX, int chunkZ) const {
		return chunkX >= 0 && chunkX < kWorldSize &&
			chunkZ >= 0 && chunkZ < kWorldSize;
	}

	int ToIndex(int chunkX, int chunkZ) const
	{
		assert(IsInBounds(chunkX, chunkZ) && "チャンク範囲外");
		return  chunkX + chunkZ * kWorldSize;
	}

	void ToChunkPos(int index, int* chunkX, int* chunkZ) const
	{
		*chunkX = index % kWorldSize;
		*chunkZ = index / kWorldSize;
	}

private:
	//マップの状態の配列
	std::array<Chunk, kChunkCount> m_map = {};

};

