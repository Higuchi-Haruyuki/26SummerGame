#pragma once
#include "FactoryComponent.h"
#include "ItemStack.h"
#include <vector>
#include <functional>
#include <memory>
#include <string>
#include <deque>

class VectorInt;
class Object;
class Square3D;
class PlayerInput;
class ItemStack;
class Model;
class TransportSystem;
class UIImage;
class UIText;
class UIItemBox;

struct BeltItem
{
    std::unique_ptr<ItemStack> m_item;
    float m_progress = 0.0f;
};

class BeltConveyor :
    public FactoryComponent
{
public:
    BeltConveyor(std::weak_ptr<Object> parentObject);
    virtual ~BeltConveyor() {};

    void Init() override;
    void Update() override;

    /// <summary>
    /// inputのFactoryComponentから呼び出し、このコンポーネントにアイテムを入れようを試みる
	/// </summary>
	/// <param name="item">入れるアイテムスタック</param>
	/// <param name="count">入れる個数</param>
	/// <returns></returns>
    bool TryInsert(ItemStack* item, int count) override;

    void UpdateUIPanel() override;

    std::unique_ptr<ItemStack> MakeItemStackFromThisComponent() override;

    //SETTER
    void SetRotationAngle(Radian angle) override;

    void SetIsPreviewMode(bool b, int alpha) override;

    //GETTER
    Radian GetRotationAngle() const override;

    VectorInt GetOutputDir() const override;

protected:

    void BuildUIPanel() override;

private:
    
    void AdvanceItems(float deltaTime);

    void TryOutput();

    void DrawItems() const;

    Vector GetItemWorldPos(float progress) const;

private:

    std::deque<BeltItem> m_beltItems;

    std::shared_ptr<UIItemBox> m_itemUI;

    //コンポーネントへのポインタ
    std::weak_ptr<Model> m_model;
    std::weak_ptr<TransportSystem> m_transport;

};

