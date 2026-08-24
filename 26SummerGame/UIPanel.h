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

    /// <summary>
	/// 手前の子要素から順にOnPointerDownをディスパッチする。
	/// いずれかの子要素がtrueを返した(=消費した)時点で処理を打ち切る。
	/// </summary>
	/// <returns>いずれかの子要素が入力を消費したらtrue</returns>
    bool DispatchPointerDown(const Vector& screenPos);

    /// <summary>
    /// 手前の子要素から順にOnPointerUpをディスパッチする。DispatchPointerDownと対になる処理。
    /// </summary>
    bool DispatchPointerUp(const Vector& screenPos);

    bool DispatchDragBegin(const Vector& screenPos);

    bool DispatchDragEnd(const Vector& screenPos);

private:
    // 子要素のリスト。末尾ほど手前に描画される。
    std::vector<std::shared_ptr<Base_UIElement>> m_children;

};