#pragma once
#include "Game.h"
#include <array>
#include <cassert>

enum class Item;

using Chunk = std::array<std::array<Item, Game::kChankSize>, Game::kChankSize>;

namespace Map
{
	/// <summary>
	/// ワールドのチャンクの一辺の数
	/// </summary>
	constexpr int kWorldSize = 5;

	/// <summary>
	/// ワールドのチャンクの数
	/// </summary>
	constexpr int kChunkCount = kWorldSize * kWorldSize;

	// チャンク座標が有効範囲内かどうかを判定する
	inline bool IsInBounds(int chunkX, int chunkZ)
	{
		return chunkX >= 0 && chunkX < kWorldSize &&
			chunkZ >= 0 && chunkZ < kWorldSize;
	}

	inline int ToIndex(int chunkX, int chunkZ)
	{
		assert(IsInBounds(chunkX, chunkZ) && "チャンク範囲外");
		return  chunkX + chunkZ * kWorldSize;
	}

	inline void ToChunkPos(int index, int* chunkX, int* chunkZ)
	{
		*chunkX = index % kWorldSize;
		*chunkZ = index / kWorldSize;
	}
}