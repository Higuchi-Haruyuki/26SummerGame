#pragma once
#include <map>
#include <string>
#include <functional>
#include "BeltConveyor.h"
#include "Base_UIElement.h"
#include "UIPointer.h"
#include "vector.h"

class UIPanel;
class ItemSlot;
class ItemStack;

enum class Item;

/// <summary>
/// UIのレイヤー
/// </summary>
enum class UILayer
{
	Hud = 0,
	Screen = 1,
	Popup = 2,
};

/// <summary>
/// UI要素の描画をまとめて処理するクラス
/// </summary>
class UIManager
{
public:

	static UIManager& GetInstance()
	{
		static UIManager instance;
		return instance;
	}

	void Update(float deltaTime);

	void Draw() const;

	Item GetSelectedItemType() const;

	/// <summary>
	/// 指定した名前のスクリーンを取得する。
	/// 存在しなければ新規作成する。
	/// </summary>
	/// <param name="screenName"></param>
	/// <param name="layer"></param>
	/// <returns></returns>
	std::shared_ptr<UIPanel> GetOrCreateScreen(const std::string& screenName, UILayer layer = UILayer::Hud);

	bool IsPointerHoverUI() const { return m_isPointerOverUI; }

	void ItemSelect(std::weak_ptr<ItemSlot> itemSlot, int idx);

	/// <summary>
	/// スロットのインデックスの位置に選択中のアイテムを移動する。
	/// </summary>
	/// <param name="itemSlot"></param>
	/// <param name="idx"></param>
	/// <returns></returns>
	bool MoveItem(std::weak_ptr<ItemSlot> itemSlot, int idx);

private:
	UIManager() = default;
	~UIManager() = default;
	
	UIManager(const UIManager&) = delete;
	UIManager& operator = (const UIManager&) = delete;

	UIManager(UIManager&&) = delete;
	UIManager& operator = (UIManager&&) = delete;

	/// <summary>
	/// 指定した名前のスクリーンを取得する。
	/// 存在しなければnullptrを返す。
	/// </summary>
	/// <param name="screenName"></param>
	/// <param name="layer"></param>
	/// <returns></returns>
	std::shared_ptr<UIPanel> GetScreen(const std::string& screenName);

	/// <summary>
	/// 指定した名前のスクリーンを新規作成する。
	/// </summary>
	/// <param name="screenName"></param>
	/// <param name="layer"></param>
	/// <returns></returns>
	std::shared_ptr<UIPanel> CreateScreen(const std::string& screenName, UILayer layer = UILayer::Hud);

	void ResetDrag();
private:

	/// <summary>
	/// レイヤー番号の降順(手前優先)に並べたスクリーン一覧を作る
	/// </summary>
	/// <returns></returns>
	std::vector<std::pair<std::string, std::shared_ptr<UIPanel>>> MakeOrderedScreen() const;

	std::shared_ptr<Base_UIElement> FindHitElement(
		const std::vector<std::pair<std::string, std::shared_ptr<UIPanel>>>& orderedScreens, 
		const Vector& nowPo) const;

	/// <summary>
	/// 移動の閾値を用いて、ドラッグしているかをメンバ変数に代入する。
	/// </summary>
	/// <param name="nowPos"></param>
	void SetIsDraggingByThreshould(const Vector& nowPos);

private:

	/// <summary>
	/// 名前とUIPanelのポインタのマップ
	/// </summary>
	std::map<std::string, std::shared_ptr<UIPanel>> m_screens;
	
	/// <summary>
	/// UIPanelの名前とレイヤーのマップ
	/// </summary>
	std::map<std::string, UILayer> m_screenLayers;

	UIPointer m_pointer;

	bool m_isPointerOverUI = false;

	/*ドラッグ関連変数*/

	//押下したUI要素
	std::weak_ptr<Base_UIElement> m_capturedElement;

	//押したときのスクリーン座標
	Vector m_pressScreenPos;

	//さいごのふれーむのスクリーン座標
	Vector m_lastScreenPos;

	//現在ドラッグ中か
	bool m_isDragging = false;

	//参照元のアイテムスロット
	std::weak_ptr<ItemSlot> m_referenceItemSlot;

	//参照しているアイテムスロットにおけるインデックス
	int m_referenceIndex = -1;

};

