#pragma once
#include "Component.h"
#include "ItemStack.h"
#include <string>

class ItemSlot;

enum class Item;

class FuelSystem :
    public Component
{
public:
	FuelSystem(std::shared_ptr<Object> parentObject);
	virtual ~FuelSystem() {};

	void Init() override;

	void Update(float deltaTime);

	void SetFuelItem(Item itemType)
	{
		m_usableFuelType = itemType;
	}

	/// <summary>
	/// 燃料を追加する。失敗したら、falseを返し、成功したらtrueを返す。
	/// 成功したら、引数で与えたアイテムは使用されて個数が減少する。
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	bool AddFuel(std::shared_ptr<ItemStack> name);

	/// <summary>
	/// 燃料を使用する。失敗したら、falseを返し、成功したらtrueを返す。
	/// </summary>
	/// <returns></returns>
	bool UseFuel();

	/// <summary>
	/// 燃料の残りを返す。
	/// </summary>
	/// <returns></returns>
	int GetFuelRemain() const;

	std::weak_ptr<ItemSlot> GetFuelSlot() const { return m_fuelSlot; }

	void GetAllItemOwnership(std::vector<std::pair<std::shared_ptr<ItemStack>, int>>* result);

private:
	/// <summary>
	/// 引数で与えられたアイテム名と現在の燃料名が同一かを返す。
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	bool IsEqualFuelType(Item itemType) const;

	/// <summary>
	/// 引数で与えられたアイテムを追加するときに新規追加する必要があるかを返す。
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	bool IsNeedAddNew() const;

	/// <summary>
	/// 燃料スロットに新規追加する。
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	bool AddNewFuel(std::shared_ptr<ItemStack> name);

	/// <summary>
	/// 燃料スロットに燃料の個数を追加する。
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	bool AddFuelCount(const std::shared_ptr<ItemStack>& name);

private:
	//燃料スロット
	std::shared_ptr<ItemSlot> m_fuelSlot;

	//燃料として使用できるアイテムの名前
	Item m_usableFuelType = Item::kNone;

	int m_useCount = 0;
};

