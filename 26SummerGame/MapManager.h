#pragma once
#include <vector>
#include <memory>
#include <array>
#include "Game.h"
#include "Map.h"

class ResourceGenerator;
enum class Item;
class VectorInt;
class Map;
struct GridResource;


class MapManager
{
public:
	// シングルトンインスタンスを取得
	static MapManager& GetInstance();

	void Init();

	void Draw() const;

	/// <summary>
	/// グリッド座標からそのグリッドの資源を取得する。
	/// </summary>
	/// <param name="gridPos"></param>
	/// <returns></returns>
	Item GetResourceAtGridPos(const VectorInt& gridPos);

private:
	MapManager() = default;
	~MapManager() = default;

	// コピー禁止
	MapManager(const MapManager&) = delete;
	MapManager& operator=(const MapManager&) = delete;
	// ムーブ禁止
	MapManager(MapManager&&) = delete;
	MapManager& operator=(MapManager&&) = delete;


	/// <summary>
	/// 指定したチャンク座標にチャンクを作成する。
	/// </summary>
	void GenerateMap();

	/// <summary>
/// ステージオブジェクトを作る関数
/// </summary>
/// <param name="pos">座標</param>
/// <param name="siz">大きさ</param>
/// <param name="col">色</param>
	void CreateStageObject(const Vector& pos, const Vector& siz, unsigned int col);

private:

	std::unique_ptr<ResourceGenerator> m_resourceGenerator;
	std::unique_ptr<Map> m_map;

	//リソースが配置されたグリッドを保存する。
	std::vector<GridResource> m_gridResources;
};

