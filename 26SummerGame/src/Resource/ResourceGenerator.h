#pragma once
#include <unordered_map>
#include "Map.h"

enum class Item;

/// <summary>
/// 資源の設定
/// </summary>
struct ResourceSetting
{
	//出現率(%)
	//0.01%単位まで想定
	float m_spawnRate;

	//最小範囲
	int m_minExtent;

	//さいだいはんい
	int m_maxExtent;
	
};

/// <summary>
///	グリッド座標と存在する資源を保存する構造体
/// </summary>
struct GridResource
{
	VectorInt m_gridPos;
	Item m_mapResource;
};


/// <summary>
/// 資源生成に関する処理をするクラス
/// </summary>
class ResourceGenerator
{
public:
	ResourceGenerator() {};
	~ResourceGenerator() {};

	const std::unordered_map<Item, ResourceSetting>& GetResourceSettingMap() const ; 


	ResourceSetting GetResourceSetting(const Item& mapResource) const; 


	/// <summary>
	/// あらかじめ設定された出現率と資源の設定からMapResourceを出力する
	/// <param name="resourceSetting">設定された資源のresourceSettingを格納する。資源がないときはnullptrを格納する。</param>
	/// </summary>
	/// <returns></returns>
	Item GetRandomResource(ResourceSetting* resourceSetting) const;

	/// <summary>
	/// 資源を設定したデータ上のチャンクを生成する。
	/// 引数にとった配列に資源が配置されたグリッドを保存する。
	/// </summary>
	/// <param name="chunkPos">チャンク座標</param>
	/// <param name="gridResources">資源が配置されたグリッドの配列</param>
	/// <returns></returns>
	Chunk GenerateChunk(const VectorInt& chunkPos, std::vector<GridResource>* gridResources) const;

private:

	/// <summary>
	/// 指定した座標にマップ資源を配置する。
	/// 受け取ったチャンクのポインタ変数を変更する。
	/// </summary>
	/// <param name="chunk">チャンクのポインタ</param>
	/// <param name="chunkPos">チャンクの座標</param>
	/// <param name="gridPos">チャンク内のローカル座標</param>
	/// <param name="mapResource">配置するマップ資源</param>
	/// <param name="gridResource">グリッド座標と資源を保存する配列のポインタ</param>
	void PutResource(Chunk* chunk, const VectorInt& chunkPos, const VectorInt& gridPos, Item mapResource, std::vector<GridResource>* gridResources) const;

	/// <summary>
	/// 指定した座標を中心として、バッファ分ずれた場所にマップ資源を配置する。
	/// </summary>
	/// <param name="chunk">チャンクのポインタ</param>
	/// <param name="chunkPos">チャンクの座標</param>
	/// <param name="gridPos">チャンク内のローカル座標</param>
	/// <param name="buffer">座標からずらす幅</param>
	/// <param name="mapResource">配置するマップ資源</param>
	/// <param name="gridResource">グリッド座標と資源を保存する配列のポインタ</param>
	void PutResourceAtCenterPlusBuffer(Chunk* chunk, const VectorInt& chunkPos, const VectorInt& gridPos, const VectorInt& buffer, Item mapResource, std::vector<GridResource>* gridResources) const;


};

