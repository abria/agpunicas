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
#include <string>

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

		// string
		const std::string str() const { return std::string("(") + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a) + ")"; }
		friend std::ostream& operator << (std::ostream& os, const Color& c) { os << c.str(); return os; }
	};
}