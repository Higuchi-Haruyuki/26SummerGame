#pragma once
#include <string>
#include "vector.h"
#include <functional>
#include <memory>
#include <concepts>
#include "ObjectFactory.h"

using Radian = float;

class VectorInt;
class Object;
class FactoryComponent;
class ItemManager;

enum class Item;
enum class GraphicId;
enum class ModelId;

class ItemStack
{
public:
	ItemStack(Item itemType, int itemCount);
	ItemStack(Item itemType, int itemCount,
		const std::function<std::shared_ptr<Object>(const VectorInt& gridPos, float rotationAngle)>& installationFunc,
		const std::function<std::shared_ptr<Object>()>& previewFunc
	);
	
	~ItemStack() {};

	/// <summary>
	/// ItemStack型のポインタを作成する。
	/// </summary>
	/// <typeparam name="">作成したいFactoryComponentの型</typeparam>
	/// <param name="objTag"></param>
	/// <param name="itemName"></param>
	/// <param name="itemCount"></param>
	/// <returns>ItemStack型のポインタ</returns>
	template<std::derived_from<FactoryComponent> T>
	static std::unique_ptr<ItemStack> MakeItemStack(
	const std::string& objTag, Item itemType, int itemCount)
	{
		auto installationFunc = [objTag](const VectorInt& gridPos, float rotationAngle)
			{
				return ObjectFactory::CreateObject<T>(gridPos, objTag, rotationAngle);
			};
		auto previewFunc = [objTag]()
			{
				return ObjectFactory::CreateObject<T>({0,0,0}, objTag);
			};

		return std::make_unique<ItemStack>(itemType, itemCount, installationFunc, previewFunc);
	}

	static std::unique_ptr<ItemStack> MakeItemStack(
		Item itemType, int itemCount)
	{
		return std::make_unique<ItemStack>(itemType, itemCount);
	}

	//GETTER
	Item GetItemType() const { return m_itemType; }
	int GetItemCount() const { return m_itemCount; }
	int GetModelHandle() const { return m_modelHandle; }
	GraphicId GetItemIconGraphicID() const { return m_itemIconResourceName; }

	//SETTER
	void SetItemType(Item itemType);
	void SetItemCount(int itemCount);


	/// <summary>
	/// アイテムをcount個移動する。
	/// </summary>
	/// <param name="from">移動元のポインタ</param>
	/// <param name="count"></param>
	/// <returns></returns>
	bool MoveItemStack(ItemStack* from, int count);

	/// <summary>
	/// このアイテムスタックにアイテムを追加し、追加に成功した数を返す。
	/// </summary>
	int AddItemCount(int itemCount);

	/// <summary>
	/// 引数分の個数を追加するとしたとき、何個追加可能かを返す。
	/// </summary>
	/// <param name="itemCount"></param>
	/// <returns></returns>
	int CheckAddItemCount(int itemCount) const;

	int MinusItemCount(int itemCount);

	/// <summary>
	/// 引数分の個数を減らすとき、何個減少するかを返す。
	/// </summary>
	/// <param name="itemCOunt"></param>
	int CheckMinusItemCount(int itemCount) const;


	void DrawModel(const Vector& v) const;

	/// <summary>
	/// このアイテムを設置する。成功したらそのポインタを返し、失敗したらnullptrを返す。
	/// </summary>
	const std::weak_ptr<Object>& Installation(const VectorInt& gridPos, float rotationAngle);

	/// <summary>
	/// このアイテムのプレビューを表示する。成功したらそのポインタを返し、失敗したらnullptrを返す。
	/// </summary>
	const std::weak_ptr<Object>& GeneratePreviewObject();

	void RemovePreviewObject();
	
	/// <summary>
	/// プレビューオブジェクトを回転させる。
	/// </summary>
	/// <param name="rotationAngle"></param>
	void RotationPreviewObject(Radian rotationAngle);

	void SetPositionPreviewObject(const Vector& pos);
private:

	void SetHandle(Item itemType);

	void SetModelHandle(ModelId modelHandle);
private:

	ItemManager& m_itemManager;

	Item m_itemType;

	GraphicId m_itemIconResourceName;

	int m_itemCount;
	/// <summary>
	/// アイテムが設置されるときに呼ばれる関数
	/// </summary>
	std::function<std::weak_ptr<Object>(const VectorInt& gridPos,float rotationAngle)> m_installationItem;

	/// <summary>
	/// アイテムのプレビューを表示するときに呼ばれる関数。
	/// オブジェクトの生ポインタを返す。プレビューオブジェクトはこの関数内で生成され、m_previewObjectにスマートポインタとして保持される。
	/// </summary>
	std::function<std::weak_ptr<Object>()> m_generatePreviewObject;

	/// <summary>
	/// プレビューオブジェクトのスマートポインタ。プレビューオブジェクトが存在しないときはnullptr。
	/// </summary>
	std::weak_ptr<Object> m_previewObject;

	int m_modelHandle;
};

