#include "MapManager.h"
#include "ObjectFactory.h"
#include "ResourceGenerator.h"
#include "Square3D.h"
#include "SquareCollider3D.h"
#include "Map.h"
#include <DxLib.h>

namespace
{
	constexpr int kSquareSize = 200;
	constexpr int kSquareY = -100;
	const Vector kSquareScale = { 1,1,1 };
	constexpr unsigned int kGroundColor = 0x2e8b57;

}


// ローカル静的変数を使った Meyers' singleton 実装
MapManager& MapManager::GetInstance()
{
	static MapManager instance;
	return instance;
}

void MapManager::Init()
{
	m_resourceGenerator = std::make_unique<ResourceGenerator>();
	m_map = std::make_unique<Map>();

	GenerateMap();
}


void MapManager::Draw() const
{
	SetUseLighting(false);

	for (const auto& gridResource : m_gridResources)
	{
		auto worldPos = Game::GridPosToWorldPos(gridResource.m_gridPos) - Vector{0,75,0};
		unsigned int color = 0x000000;
		switch (gridResource.m_mapResource)
		{
		case Item::kCoalOre:
			color = 0x000000;
			break;
		case Item::kIronOre:
			color = 0xffffff;
			break;
		case Item::kRockOre:
			color = 0xf5deb3;
			break;
		case Item::kCopperOre:
			color = 0x553911;
			break;
		default:
			break;
		}

		DrawSphere3D(worldPos.ToVECTOR(), 100, 16, color, color, true);

	}
	SetUseLighting(true);
}

Item MapManager::GetResourceAtGridPos(const VectorInt& gridPos)
{
	for (const auto& gridResource : m_gridResources)
	{
		if (gridResource.m_gridPos == gridPos) return gridResource.m_mapResource;
	}
	return Item::kNone;
}


void MapManager::GenerateMap()
{
	std::array<Chunk, kChunkCount> map = {};

	//チャンク単位での生成ループ
	for (int i = 0; i < kChunkCount; i++)
	{
		//ロープ変数から地面のオブジェクトを置く座標を取得する。
		//取得した座標にオブジェクトを置く。
		int chunkX = 0, chunkZ = 0;
		m_map->ToChunkPos(i, &chunkX, &chunkZ);

		auto worldPos = Game::GridPosToWorldPos(Game::ChankPosToGridPos({chunkX,0,chunkZ}));
		float chankSize = Game::kChankSize * Game::kGridSize;
		CreateStageObject(
			{ worldPos.m_x + chankSize * 0.5f - Game::kGridSize * 0.5f,
			static_cast<float>(kSquareY),
			worldPos.m_z + chankSize * 0.5f - Game::kGridSize * 0.5f },

			{ chankSize, kSquareSize, chankSize },

			kGroundColor
		);

		//このチャンクにランダムで資源を設定する。
		map[i] = m_resourceGenerator->GenerateChunk({chunkX,0,chunkZ},&m_gridResources);

	}
	
	//マップに設定する。
	m_map->SetMap(map);

}

void MapManager::CreateStageObject(const Vector& pos, const Vector& siz, unsigned int col)
{
	const auto& obj = ObjectFactory::CreateObject(pos, "Ground");
	const auto& stageSquare3D = obj->AddComponent<Square3D>();
	const auto& collider = obj->AddComponent<SquareCollider3D>();
	stageSquare3D->SetSize(siz);
	stageSquare3D->SetColor(col);
	collider->SetSize(siz);
	collider->IsVisible(true);
	collider->SetMass(0);
}

