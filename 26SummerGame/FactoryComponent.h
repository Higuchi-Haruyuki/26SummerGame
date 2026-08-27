#pragma once

#include "Component.h"
#include <memory>
#include "vector.h"
#include <vector>
#include <functional>
#include <string>
#include "ItemType.h"

class Timer;
class ItemStack;
class Object;
class VectorInt;
class RecipeManager;
class FactoryManager;
class Recipe;
class ResourceManager;
class Square3D;
class Collider;
class PlayerInput;
class Shape;
class UIManager;
class ItemSlot;
class UIPanel;
class UIManager;

using Radian = float;
using Second = float;

class FactoryComponent :
	public Component
{
public:
	FactoryComponent(std::weak_ptr<Object> parentObject);
	virtual ~FactoryComponent();

	void Init() override;
	void Update() override;
	void Finalize() override;

	/// <summary>
	/// inputのFactoryComponentから呼び出し、このコンポーネントにアイテムを入れようを試みる
	/// </summary>
	/// <param name="item">入れるアイテムスタック</param>
	/// <param name="count">入れる個数</param>
	/// <returns></returns>
	virtual bool TryInsert(ItemStack* item, int count);

	/// <summary>
	/// 工業オブジェクト特有のUIパネルのポインタを返す。
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<UIPanel> GetOrBuildUIPanel();

	/// <summary>
	/// UI展開時にUI要素の更新処理を行う。
	/// </summary>
	virtual void UpdateUIPanel() {};

	//SETTER

	virtual void SetRotationAngle(Radian angle);

	virtual void SetIsPreviewMode(bool b, int alpha);

	//GETTER

	virtual VectorInt GetOutputDir() const { return { -1,-1,-1 }; }

	virtual Radian GetRotationAngle() const { return 0.0f; };

	bool GetIsPreviewMode()const {return m_isPreviewMode; }

	Item GetItemType() const { return m_itemType; }

	virtual std::unique_ptr<ItemStack> MakeItemStackFromThisComponent();

	std::weak_ptr<ItemSlot> GetInputItemSlot() const { return m_inputSlot; }

	std::weak_ptr<ItemSlot> GetOutputItemSlot() const { return m_outputSlot; }

	ItemStack* GetInputItemStack(int index) const;

	ItemStack* GetOutputItemStack(int index) const;

protected:

	/// <summary>
	/// UIパネルにUI要素を追加する。初呼び出し時に呼び出される。
	/// </summary>
	virtual void BuildUIPanel() {};

	/// <summary>
	/// 大きさと色と入力、出力スロットの最大数を設定する。
	/// </summary>
	/// <param name="siz"></param>
	/// <param name="col"></param>
	/// <param name="maxInputSlot"></param>
	/// <param name="maxOutputSlot"></param>
	void SetSizeAndColorAndMaxSlot(const Vector& siz, unsigned int col, int maxInputSlot, int maxOutputSlot);

	void SetItemName(Item itemType)
	{
		m_itemType = itemType;
	}

	void SetTimerDuration(Second sec);
	
	/// <summary>
	/// アイテムの所有権を移動して挿入する。
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	bool TryInsert(ItemSlot* itemSlot, std::unique_ptr<ItemStack> item);

	/// <summary>
	/// このコンポーネントの引数に指定されたスロットにアイテムを入れようを試みる
	/// </summary>
	/// <param name="itemSlot"></param>
	/// <param name="item"></param>
	/// <param name="count"></param>
	/// <returns></returns>
	bool TryInsert(ItemSlot* itemSlot, ItemStack* item, int count);
protected:

	//<シングルトンへの参照>
	//FactoryManagerクラスへの参照
	FactoryManager& m_factoryManager;
	//RecipeManagerクラスへの参照
	RecipeManager& m_recipeManager;

	//コンポーネントへのポインタ
	std::weak_ptr<Shape> m_shape;
	std::weak_ptr<Collider> m_collider;

	//プレビューモードかのフラグ
	bool m_isPreviewMode = false;

	std::shared_ptr<ItemSlot> m_inputSlot;

	std::shared_ptr<ItemSlot> m_outputSlot;

	std::unique_ptr<Timer> m_timer;

	//UI用のパネル (PlayerUIクラスのパネルの子要素として使う)
	std::shared_ptr<UIPanel> m_uiPanel;

private:
	//アイテムとしての名前
	Item m_itemType = Item::kNone;

};

