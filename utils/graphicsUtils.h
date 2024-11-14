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

    inline static Color distinctColor(int n)
    {
        if(n == 0)
            return { 255, 255, 25 };  // yellow
        else if (n == 1)
            return { 0, 130, 200 };  // blue
        else if (n == 2)
            return { 60, 180, 75 };  // green
        else if (n ==  3)
            return { 245, 130, 48 }; // orange
        else if (n ==  4)
            return { 70, 240, 240 }; // cyan
        else if (n ==  5)
            return { 145, 30, 180 }; // purple
        else if (n ==  6)
            return { 230, 25, 75 };  // red
        else if (n ==  7)
            return { 210, 245, 60 }; // lime
        else if (n ==  8)
            return { 240, 50, 230 }; // magenta
        else if (n ==  9)
            return { 0, 0, 128 };    // navy
        else if (n ==  10)
            return { 128, 0, 0 };    // maroon
        else if (n ==  11)
            return { 170, 110, 40 }; // brown
        else if (n ==  12)
            return { 128, 128, 0 };  // olive
        else if (n ==  13)
            return { 0, 128, 128 };  // teal
        else if (n == 14)
            return { 0, 0, 0 };      // black
        else
        {
            std::cerr << "Cannot generate " << n << "-th distinct color\n";
            return { 0, 0, 0 };      // black
        }
    }
}