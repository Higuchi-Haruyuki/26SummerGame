#pragma once
#include "vector.h"
#include "VectorInt.h"
namespace Game
{
	//定数の定義
	constexpr int kTargetFrameRate = 60;
	constexpr int kTimePerFrame = 1000000 / kTargetFrameRate;
	const Vector kFULLHD = { 1920, 1080 };
	const Vector kHD = { 1280, 720 };
	const Vector kDisplaySize = kFULLHD;
	const Vector kMapSize = { 4000,2000 };

	constexpr float kGravity = -1.0f;
	constexpr int kGridSize = 200;

	//一つのチャンクのサイズ
	constexpr int kChankSize = 16;

	inline VectorInt WorldPosToGridPos(const Vector& worldPos)
	{
		auto gridPos = ((worldPos + Vector{ 100,0,100 }) / kGridSize).ToVectorInt();
		if (worldPos.m_x < -(kGridSize / 2)) gridPos.m_x += -1;
		if (worldPos.m_z < -(kGridSize / 2)) gridPos.m_z += -1;
		return gridPos;
	}
	inline Vector GridPosToWorldPos(const VectorInt& gridPos)
	{
		return (gridPos * kGridSize).ToVector();
	}

	inline VectorInt GridPosToChankPos(const VectorInt& gridPos)
	{
		auto chankPos = ((gridPos) / kChankSize);
		if (gridPos.m_x < 0) chankPos.m_x += -1;
		if (gridPos.m_z < 0) chankPos.m_z += -1;
		return chankPos;
	}
	inline VectorInt ChankPosToGridPos(const VectorInt& chankPos)
	{
		return chankPos * kChankSize;
	}
}