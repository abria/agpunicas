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
		Color(const Color& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}

		// string
		const std::string str() const { return std::string("(") + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a) + ")"; }
		friend std::ostream& operator << (std::ostream& os, const Color& c) { os << c.str(); return os; }

		// operator overloads
	};
}