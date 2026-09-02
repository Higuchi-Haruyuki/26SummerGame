#include "ResourceGenerator.h"
#include <unordered_map>
#include "ItemType.h"

namespace
{
	//資源の列挙型と資源の設定をペアとして保存する変数
	const std::unordered_map<Item, ResourceSetting> kResourceMap
	{ 
		{
			Item::kCoalOre,
			{ 0.07f,3,5 }
		},
		{
			Item::kIronOre,
			{ 0.05f,3,5 }
		},
		{
			Item::kRockOre,
			{ 0.05f,3,5 }
		},
		{
			Item::kCopperOre,
			{ 0.05f,3,5 }
		},

	};
}

const std::unordered_map<Item, ResourceSetting>& ResourceGenerator::GetResourceSettingMap() const
{
	return kResourceMap;
}

ResourceSetting ResourceGenerator::GetResourceSetting(const Item& mapResource) const
{
	for (const auto& resourceSetting : kResourceMap)
	{
		if (mapResource == resourceSetting.first) return resourceSetting.second;
	}
	return ResourceSetting{};
}

Item ResourceGenerator::GetRandomResource(ResourceSetting* resourceSetting) const
{

	Item gridResource = Item::kNone;
	//そのグリッドにリソースをランダムで設定する。
	for (const auto& resource : kResourceMap)
	{
		int rand = GetRand(10000);

		//設定した確率未満ならそのリソースを設定する。
		if (rand < resource.second.m_spawnRate * 100)
		{
			gridResource = resource.first;
			*resourceSetting = resource.second;
			return gridResource;
		}
	}
	resourceSetting = nullptr;
	return gridResource;
}

Chunk ResourceGenerator::GenerateChunk(const VectorInt& chunkPos, std::vector<GridResource>* gridResources) const
{
	//生成したチャンクにランダムで資源を置く処理
	Chunk chunk = {};
	for (int z = 0; z < Game::kChankSize; z++)
	{
		for (int x = 0; x < Game::kChankSize; x++)
		{
			//すでに資源があるなら次のグリッドにスキップする。
			if (chunk[z][x] != Item::kNone) continue;

			ResourceSetting resourceSetting = {};

			//ランダムで資源を設定する。
			Item resource = GetRandomResource(&resourceSetting);

			if (resource == Item::kNone) continue;

			PutResource(&chunk, chunkPos, { x,0,z }, resource, gridResources);

			//資源が配置されていないとき
			if (resource == Item::kNone) continue;

			//範囲内でランダムな範囲を取得する
			int extentRand = resourceSetting.m_minExtent + GetRand(resourceSetting.m_maxExtent - resourceSetting.m_minExtent);

			for (int bufferX = 0; bufferX < extentRand; bufferX++)
			{
				for (int bufferZ = 0; bufferZ < extentRand; bufferZ++)
				{
					if (!bufferX && !bufferZ) continue;

					PutResourceAtCenterPlusBuffer(&chunk, chunkPos, { x, 0 ,z }, {bufferX, 0, bufferZ}, resource, gridResources);
				}
			}
		}
	}
	return chunk;
}

void ResourceGenerator::PutResource(Chunk* chunk, const VectorInt& chunkPos, const VectorInt& localGridPos, Item mapResource, std::vector<GridResource>* gridResources) const
{
	//範囲外アクセスを防ぐ
	if (localGridPos.m_x == -1 || localGridPos.m_z == -1) return;

	//置く資源がないときは終了
	if (mapResource == Item::kNone) return;

	//すでに資源があるとき関数を終了
	if (chunk->at(localGridPos.m_z).at(localGridPos.m_x) != Item::kNone) return;

	chunk->at(localGridPos.m_z).at(localGridPos.m_x) = mapResource;

	//このグリッドのグリッド座標を計算する
	auto gridPos = Game::ChankPosToGridPos(chunkPos);
	gridPos += localGridPos;

	//引数の配列に格納する。
	gridResources->push_back({ gridPos, mapResource });
}

void ResourceGenerator::PutResourceAtCenterPlusBuffer
(Chunk* chunk, const VectorInt& chunkPos, const VectorInt & localGridPos, 
	const VectorInt& buffer, Item mapResource, std::vector<GridResource>* gridResources) const
{
	int upperX = (localGridPos.m_x + buffer.m_x < Game::kChankSize) ? localGridPos.m_x + buffer.m_x : -1;
	int lowerX = (localGridPos.m_x - buffer.m_x >= 0) ? localGridPos.m_x - buffer.m_x : -1;
	int upperZ = (localGridPos.m_z + buffer.m_z <  Game::kChankSize) ? localGridPos.m_z + buffer.m_z : -1;
	int lowerZ = (localGridPos.m_z - buffer.m_z >= 0) ? localGridPos.m_z - buffer.m_z : -1;

	PutResource(chunk, chunkPos, { upperX, 0, upperZ }, mapResource ,gridResources);
	PutResource(chunk, chunkPos, { upperX, 0, lowerZ }, mapResource, gridResources);
	PutResource(chunk, chunkPos, { lowerX, 0, lowerZ }, mapResource, gridResources);
	PutResource(chunk, chunkPos, { lowerX, 0, upperZ }, mapResource, gridResources);

}



