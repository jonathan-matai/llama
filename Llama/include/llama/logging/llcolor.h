/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llcolor.h
 Predefined Colors
*/

#pragma once

namespace llama
{
    struct Color
    {
        // Default constructor
        constexpr Color() :
            r(0), g(0), b(0)
        { }

        // Create color with indiviudal values
        constexpr Color(uint8_t _r, uint8_t _g, uint8_t _b) :
            r(_r), g(_g), b(_b)
        { }

        // Create color from color code (0xrrggbb)
        constexpr Color(uint32_t code) :
            r((code >> 16) & 0xff),
            g((code >> 8) & 0xff),
            b(code & 0xff)
        { }

        uint8_t r, g, b;
    };

    namespace Colors
    {
        constexpr Color RED(0xff0000);
        constexpr Color GREEN(0x00a000);
        constexpr Color BLUE(0x0040ff);
        constexpr Color WHITE(0xffffff);
        constexpr Color GREY(0x808080);
        constexpr Color YELLOW(0xffff00);
        constexpr Color MAGENTA(0xff00ff);
        constexpr Color CYAN(0x00ffff);
        constexpr Color ORANGE(0xff6000);
    }
}