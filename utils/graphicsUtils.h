// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2023 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include <ostream>
#include <iostream>
#include <string>
#include <vector>

namespace agp
{
	// color class
	struct Color
	{
		// attributes
		unsigned char r, g, b, a;

		// constructors
		Color() : r(0), g(0), b(0), a(0) {}
		Color(
			unsigned char red,
			unsigned char green,
			unsigned char blue,
			unsigned char alpha = 255) : r(red), g(green), b(blue), a(alpha) {}
        Color(unsigned char* rgbData, bool alpha = false)
        {
            r = rgbData[0];
            g = rgbData[1];
            b = rgbData[2];
            a = alpha? rgbData[3] : 255;
        }
		Color(const Color& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}

        // operator[] for non-const objects
        unsigned char& operator[](std::size_t index)
        {
            switch (index)
            {
                case 0: return r;
                case 1: return g;
                case 2: return b;
                case 3: return a;
                default: throw std::out_of_range("Color index out of range");
            }
        }

        // operator[] for const objects
        const unsigned char& operator[](std::size_t index) const
        {
            switch (index)
            {
                case 0: return r;
                case 1: return g;
                case 2: return b;
                case 3: return a;
                default: throw std::out_of_range("Color index out of range");
            }
        }

        // Equality operator==
        bool operator==(const Color& other) const
        {
            return r == other.r && g == other.g && b == other.b && a == other.a;
        }

        // Inequality operator!=
        bool operator!=(const Color& other) const
        {
            return !(*this == other);
        }

        Color& adjustAlpha(int newAlpha) { a = newAlpha; return *this; }

		// string
		const std::string str() const { return std::string("(") + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a) + ")"; }
		friend std::ostream& operator << (std::ostream& os, const Color& c) { os << c.str(); return os; }
	};

    inline static std::vector<Color> distinctColors(int n)
    {
        std::vector<Color> distinctColors;
        if(n > 0)
            distinctColors.push_back({ 255, 255, 25 }); // yellow
        if (n > 1)
            distinctColors.push_back({ 0, 130, 200 });  // blue
        if (n > 2)
            distinctColors.push_back({ 60, 180, 75 });  // green
        if (n > 3)
            distinctColors.push_back({ 245, 130, 48 }); // orange
        if (n > 4)
            distinctColors.push_back({ 70, 240, 240 }); // cyan
        if (n > 5)
            distinctColors.push_back({ 145, 30, 180 }); // purple
        if (n > 6)
            distinctColors.push_back({ 230, 25, 75 });  // red
        if (n > 7)
            distinctColors.push_back({ 210, 245, 60 }); // lime
        if (n > 8)
            distinctColors.push_back({ 240, 50, 230 }); // magenta
        if (n > 9)
            distinctColors.push_back({ 0, 0, 128 });    // navy
        if (n > 10)
            distinctColors.push_back({ 128, 0, 0 });    // maroon
        if (n > 11)
            distinctColors.push_back({ 170, 110, 40 }); // brown
        if (n > 12)
            distinctColors.push_back({ 128, 128, 0 });  // olive
        if (n > 13)
            distinctColors.push_back({ 0, 128, 128 });  // teal
        if (n > 14)
            std::cerr << "Cannot generate " << n << " distinct colors\n";

        return distinctColors;
    }
}