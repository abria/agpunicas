#pragma once
#include "graphicsUtils.h"
#include <array>

// Editor-only typography and colors: game/debug palettes keep their existing meaning.
namespace agp { namespace editorStyle
{
    constexpr int CATEGORY_COUNT = 30;
    constexpr const char* FONT = "RobotoCondensed-Medium";
    const Color PANEL = { 18, 22, 30 };
    const Color TEXT = { 248, 250, 252 };

    inline Color categoryColor(int category)
    {
        // Interleave hues and lightness so neighboring IDs remain distinct.
        // Every accent has at least 7:1 contrast against PANEL.
        static const std::array<Color, CATEGORY_COUNT> palette = {{
            { 255, 120, 130 }, { 255, 184, 77 }, { 95, 210, 155 },
            { 100, 180, 255 }, { 198, 155, 255 }, { 255, 137, 210 },
            { 229, 220, 90 }, { 64, 214, 220 }, { 185, 213, 120 },
            { 255, 164, 130 }, { 151, 169, 255 }, { 222, 183, 145 },
            { 239, 153, 174 }, { 106, 226, 185 }, { 205, 215, 232 },
            { 255, 198, 205 }, { 240, 213, 164 }, { 167, 237, 191 },
            { 171, 222, 255 }, { 226, 201, 255 }, { 255, 192, 236 },
            { 250, 240, 157 }, { 160, 239, 237 }, { 214, 238, 163 },
            { 255, 211, 179 }, { 193, 203, 255 }, { 215, 201, 177 },
            { 218, 168, 222 }, { 140, 209, 203 }, { 242, 244, 248 }
        }};
        return palette[static_cast<unsigned int>(category) % palette.size()];
    }
}}
