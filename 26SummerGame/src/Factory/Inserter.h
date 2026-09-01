#pragma once
#include "FactoryComponent.h"

class TransportSystem;
class UIItemBox;

/// <summary>
/// 工業オブジェクト間のアイテムの受け渡しをする
/// </summary>
class Inserter :
    public FactoryComponent
{

public:
    Inserter(std::weak_ptr<Object> parentObject);
    virtual ~Inserter() {};

    void Init() override;
    void Update() override;

    void UpdateUIPanel() override;

    //SETTER
    void SetRotationAngle(Radian angle) override;

    void SetIsPreviewMode(bool b, int alpha) override;

    //GETTER
    Radian GetRotationAngle() const override;

    VectorInt GetOutputDir() const override;

    /// <summary>
   /// inputのFactoryComponentから呼び出し、このコンポーネントにアイテムを入れようを試みる
   /// </summary>
   /// <param name="item">入れるアイテムスタック</param>
   /// <param name="count">入れる個数</param>
   /// <returns></returns>
    bool TryInsert(ItemStack* item, int count) override;

protected:

    void BuildUIPanel() override;

private:

    std::shared_ptr<UIItemBox> m_itemUI;

    //コンポーネントへのポインタ
    std::weak_ptr<TransportSystem> m_transport;
};

