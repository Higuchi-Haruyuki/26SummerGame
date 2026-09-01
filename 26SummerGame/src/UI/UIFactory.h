#pragma once
#include "Base_UIElement.h"
#include "UIPanel.h"

namespace UIFactory
{
    /// <summary>
    /// UIを指定したパネルに生成する。
    /// </summary>
    /// <typeparam name="T">Base_UIElementを継承したクラス</typeparam>
    /// <typeparam name="...Args"></typeparam>
    /// <param name="...args"></param>
    /// <param name="addPanel">追加するパネル</param>
    /// <returns>生成したUIへの弱参照</returns>
    template <typename T, typename... Args>
        requires std::is_base_of_v<class Base_UIElement, T>
    inline std::weak_ptr<T>MakeUIToPanel(std::weak_ptr<UIPanel> addPanel, Args&&... args)
    {
        const auto& ui = std::make_shared<T>(std::forward<Args>(args)...);

        //生成したUIを指定したパネルに追加する。
        const auto& safePanel = addPanel.lock();
        if (!safePanel) return std::weak_ptr<T>();
        safePanel->AddChild(ui);

        //値を返す。
        return ui;
    }
}