#pragma once
#include "Base_UIElement.h"
#include <vector>

class UIPanel :
    public Base_UIElement
{
public:

    UIPanel(const Vector& position = {}, const Vector& size = {}) :
        Base_UIElement(position, size)
    {}

    virtual ~UIPanel() override = default;

    /// <summary>
    /// 子要素のUpdateも呼び出す。
    /// </summary>
    /// <param name="deltaTime"></param>
    void Update(float deltaTime) override;

    void Draw() const override;

    void SetVisible(bool isVisible) override;

    bool HitTest(const Vector& screenPos) const override;

    std::shared_ptr<Base_UIElement> HitTestTopMost(const Vector& screenPos);

    /// <summary>
    /// 子要素を追加する。あとから追加した要素ほど手前に描画され、
    /// ヒットテスト/クリックの優先度も高くなる(前面の要素が入力を先取りする)。
    /// </summary>
    void AddChild(const std::shared_ptr<Base_UIElement>& child) { m_children.push_back(child); }
    void RemoveChild(const std::shared_ptr<Base_UIElement>& child);
    void ClearChildren() { m_children.clear(); }

    bool DispatchDragBegin(const Vector& screenPos);

    bool DispatchDragEnd(const Vector& screenPos);

private:
    // 子要素のリスト。末尾ほど手前に描画される。
    std::vector<std::shared_ptr<Base_UIElement>> m_children;

};