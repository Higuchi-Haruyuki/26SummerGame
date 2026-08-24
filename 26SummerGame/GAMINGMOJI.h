#pragma once
#include <DxLib.h>
#include <string>
#include <format>

namespace GAMINGMOJI
{
    // 構造体でRGBを管理
    struct RGB {
        int r;
        int g;
        int b;
    };
    // 色相(0~359)からRGBを計算する関数
    inline RGB hueToRGB(int hue) {
        hue = hue % 360; // 360度を超えたらループするように安全対策

        int r = 0, g = 0, b = 0;
        int sector = hue / 60;     // 60度ごとの領域（0~5）
        int value = (hue % 60) * 255 / 60; // 領域内での変化量 (0~255)

        switch (sector) {
        case 0: // 赤 -> 黄
            r = 255; g = value; b = 0; break;
        case 1: // 黄 -> 緑
            r = 255 - value; g = 255; b = 0; break;
        case 2: // 緑 -> シアン
            r = 0; g = 255; b = value; break;
        case 3: // シアン -> 青
            r = 0; g = 255 - value; b = 255; break;
        case 4: // 青 -> マゼンタ
            r = value; g = 0; b = 255; break;
        case 5: // マゼンタ -> 赤
            r = 255; g = 0; b = 255 - value; break;
        }
        return RGB{ r, g, b };
    }

    inline unsigned int GetRainbowColor(int frameCount, int index)
    {
        GAMINGMOJI::RGB color = GAMINGMOJI::hueToRGB((frameCount + index * 10) % 360);
        return GetColor(color.r, color.g, color.b);
    }
}

/// <summary>
/// 指定されたフレームカウントに基づいて虹色を取得する
/// </summary>
/// <param name="frameCount">現在のフレームカウント</param>
/// <returns></returns>
inline unsigned int GetRainbowColor(int frameCount)
{
    GAMINGMOJI::RGB color = GAMINGMOJI::hueToRGB((frameCount) % 360);
    return GetColor(color.r, color.g, color.b);
}

/// <summary>
/// 虹色でフォーマットされた文字列を描画する
/// </summary>
/// <typeparam name="...Args"></typeparam>
/// <param name="m_x">左上座標X</param>
/// <param name="m_y">左上座標Y</param>
/// <param name="frameCount">現在のフレームカウント</param>
/// <param name="fmt">フォーマット文字列(%dではなく{}で変数の埋め込みをしてください)</param>
/// <param name="...args">フォーマット引数</param>
template <typename... Args>
inline void DrawRainbowFormatString(int x, int y, int frameCount, const std::format_string<Args...> fmt, Args&&... args)
{
    std::string str = std::vformat(fmt.get(), std::make_format_args(args...));
    int length = static_cast<int>(str.length());
    for (int i = 0; i < length; i++)
    {
        auto c = str.at(i);
        DrawFormatString(x + i * 8, y, GAMINGMOJI::GetRainbowColor(frameCount, i), "%c", c);
    }
}