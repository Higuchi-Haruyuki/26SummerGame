#pragma once
#include "Component.h"
#include <memory>
#include <functional>
#include <vector>
#include <string>
#include "RayCastResult.h"
#include "Event.h"

class Object;
class PlayerInput;
class UIManager;
class ItemStack;
class Shape;
class FactoryComponent;
class CharactorStateManager;
class Collider;
class ItemSlot;

enum class Item;

/// <summary>
/// プレイヤーが所持しているアイテムの管理とプレイヤーのアイテムの選択状態の管理
/// </summary>
class PlayerItem : public Component
{
public:
	PlayerItem(std::weak_ptr<Object> parentObject);
	virtual ~PlayerItem() {}

	void Init() override;
	void Update() override;

	/*ItemBar関連関数*/

	/// <summary>
	/// アイテムバーを取得
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<ItemSlot> GetItemBar() const { return m_itemSlot; }

	/// <summary>
	/// 選択中のItemStackへの弱参照を返す。
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<ItemStack> GetSelectedItemBar() const
	{
		return m_selectedItemBar;
	}

	/// <summary>
	/// 現在選択中のインデックスを返す。
	/// </summary>
	/// <returns></returns>
	int GetItemBarChoiceIndex() const { return m_itemBarChoiceIndex; }

	/// <summary>
	/// 指定されたインデックスのアイテムバーのアイテムのポインタを返す。
	/// </summary>
	/// <param name="idx"></param>
	/// <returns></returns>
	ItemStack* GetItemFromItemBar(int idx) const;

	/// <summary>
	/// アイテムバーの選択中のインデックスを変更する。
	/// 失敗時は変更されない。
	/// </summary>
	/// <param name="index"></param>
	void SetItemBarChoiceIndex(int index);


	/*Inventory関連関数*/

	std::weak_ptr<ItemSlot> GetInventory() const { return m_inventory; }

	/// <summary>
	/// 選択中のItemStackへの弱参照を返す。
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<ItemStack> GetSelectedInventoryItem() const
	{
		return m_selectedInventoryItem;
	}

	/// <summary>
	/// 現在選択中のインデックスを返す。
	/// </summary>
	/// <returns></returns>
	int GetInventoryChoiceIndex() const { return m_inventoryChoiceIndex; }

	/// <summary>
	/// 指定されたインデックスのアイテムバーのアイテムのポインタを返す。
	/// </summary>
	/// <param name="idx"></param>
	/// <returns></returns>
	ItemStack* GetItemFromInventory(int idx) const;

	/// <summary>
	/// アイテムバーの選択中のインデックスを変更する。
	/// 失敗時は変更されない。
	/// </summary>
	/// <param name="index"></param>
	void SetInventoryChoiceIndex(int index);

	/// <summary>
	/// プレイヤーにアイテムを追加する。
	/// アイテムバー優先で、溢れたらインベントリに追加する。
	/// </summary>
	/// <param name="item"></param>
	/// <param name="count"></param>
	/// <returns></returns>
	std::shared_ptr<ItemStack> AddItem(std::shared_ptr<ItemStack> item, int count);

	bool CanAddItem(Item itemType, int count) const;

	void SubscribeOnAddItem(std::function<void(Item, int)> func)
	{
		m_onAddItemConnections.push_back(m_onAddItem.AddListener(func));
	}

private:

	/// <summary>
	/// アイテムバーでの選択肢の変更を行う。
	/// </summary>
	void ChoiceItemSlot();

	void ChoiceInventorySlot();

	/// <summary>
	/// アイテムバーにアイテムをcount個追加する。
	/// 成功時はnullptrを返す。
	/// 失敗時と溢れたときはそのアイテムのポインタのunique_ptrを返す。
	/// </summary>
	/// <param name="item"></param>
	/// <param name="count"></param>
	/// <returns></returns>
	std::shared_ptr<ItemStack> AddItemToItemBar(std::shared_ptr<ItemStack> item, int count);

	/// <summary>
	/// アイテムバーにアイテムをcount個追加する。
	/// 成功時はnullptrを返す。
	/// 失敗時と溢れたときはそのアイテムのポインタのunique_ptrを返す。
	/// </summary>
	/// <param name="item"></param>
	/// <param name="count"></param>
	/// <returns></returns>
	std::shared_ptr<ItemStack> AddItemToInventory(std::shared_ptr<ItemStack> item, int count);

	bool CanAddToItemSlot(const std::weak_ptr<ItemSlot> itemSlot, Item itemType, int count) const;

	bool CanAddToItemBar(Item itemType, int count) const;

	bool CanAddToInventory(Item itemType, int count) const;

private:
	//シングルトンへの参照
	PlayerInput& m_input;
	UIManager& m_uiManager;

	//コンポーネントへのポインタ
	std::weak_ptr<Collider> m_collider;

	int m_itemBarChoiceIndex = 0;

	int m_inventoryChoiceIndex = 0;

	std::shared_ptr<ItemSlot> m_itemSlot;

	std::shared_ptr<ItemSlot> m_inventory;

	std::weak_ptr<CharactorStateManager> m_state;

	std::weak_ptr<ItemStack> m_selectedItemBar;

	std::weak_ptr<ItemStack> m_selectedInventoryItem;

	Event<Item,int> m_onAddItem;
	std::vector<Event<Item, int>::Connection> m_onAddItemConnections;

};

