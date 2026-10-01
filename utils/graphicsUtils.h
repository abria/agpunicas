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
#include <algorithm>
#include <cmath>

namespace agp
{
    // Helper function to convert RGB to HSL
    inline static void RGBtoHSL(unsigned char r, unsigned char g, unsigned char b, float& h, float& s, float& l)
    {
        float rf = r / 255.0f;
        float gf = g / 255.0f;
        float bf = b / 255.0f;

        float max = std::max({ rf, gf, bf });
        float min = std::min({ rf, gf, bf });
        float delta = max - min;

        l = (max + min) / 2.0f;

        if (delta == 0.0f)
        {
            h = s = 0.0f; // Achromatic
        }
        else
        {
            s = l < 0.5f ? delta / (max + min) : delta / (2.0f - max - min);

            if (max == rf)
                h = (gf - bf) / delta + (gf < bf ? 6.0f : 0.0f);
            else if (max == gf)
                h = (bf - rf) / delta + 2.0f;
            else // max == bf
                h = (rf - gf) / delta + 4.0f;

            h /= 6.0f;
        }
    }

    // Helper function used in HSL to RGB conversion
    inline static float HueToRGB(float p, float q, float t)
    {
        if (t < 0.0f) t += 1.0f;
        if (t > 1.0f) t -= 1.0f;

        if (t < 1.0f / 6.0f)
            return p + (q - p) * 6.0f * t;
        if (t < 1.0f / 2.0f)
            return q;
        if (t < 2.0f / 3.0f)
            return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
        return p;
    }

    // Helper function to convert HSL back to RGB
    inline static void HSLtoRGB(float h, float s, float l, unsigned char& r, unsigned char& g, unsigned char& b)
    {
        float rf, gf, bf;

        if (s == 0.0f)
        {
            rf = gf = bf = l; // Achromatic
        }
        else
        {
            float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
            float p = 2.0f * l - q;

            rf = HueToRGB(p, q, h + 1.0f / 3.0f);
            gf = HueToRGB(p, q, h);
            bf = HueToRGB(p, q, h - 1.0f / 3.0f);
        }

        r = static_cast<unsigned char>(std::round(rf * 255.0f));
        g = static_cast<unsigned char>(std::round(gf * 255.0f));
        b = static_cast<unsigned char>(std::round(bf * 255.0f));
    }

    // Convert HSV to RGB. All inputs must be finite.
    // h: hue in [0, 1], representing one full turn (h = degrees / 360).
    //    0 and 1 are red, 1/3 is green, 2/3 is blue; other values wrap modulo 1.
    // s: saturation in [0, 1], from grayscale (0) to fully saturated (1).
    // v: value in [0, 1], from black (0) to maximum brightness (1).
    // s and v are clamped to [0, 1]; output channels are rounded to [0, 255].
    inline static void HSVtoRGB(float h, float s, float v, unsigned char& r, unsigned char& g, unsigned char& b)
    {
        h = (h - std::floor(h)) * 6.0f;
        s = std::max(0.0f, std::min(s, 1.0f));
        v = std::max(0.0f, std::min(v, 1.0f));

        // Split the hue wheel into six sectors (red, yellow, green, cyan, blue, magenta).
        // Chroma C = V*S is the distance between the largest and smallest RGB channels.
        // X interpolates the intermediate channel; m = V-C sets the minimum channel.
        float c = v * s;
        float x = c * (1.0f - std::abs(std::fmod(h, 2.0f) - 1.0f));
        float m = v - c;
        float rf = 0.0f, gf = 0.0f, bf = 0.0f;

        if (h < 1.0f)      { rf = c; gf = x; }
        else if (h < 2.0f) { rf = x; gf = c; }
        else if (h < 3.0f) { gf = c; bf = x; }
        else if (h < 4.0f) { gf = x; bf = c; }
        else if (h < 5.0f) { rf = x; bf = c; }
        else              { rf = c; bf = x; }

        r = static_cast<unsigned char>(std::round((rf + m) * 255.0f));
        g = static_cast<unsigned char>(std::round((gf + m) * 255.0f));
        b = static_cast<unsigned char>(std::round((bf + m) * 255.0f));
    }

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

        Color adjustAlpha(int newAlpha) const
        { 
            Color newColor = *this;
            newAlpha = (std::min)(newAlpha, 255);
            newAlpha = (std::max)(newAlpha, 0);
            newColor.a = newAlpha;
            return newColor;
        }

        Color adjustBrightness(float factor) 
        {
            Color newColor = *this;

            if (factor < 0.0f) factor = 0.0f;

            newColor.r = (unsigned char)(std::min(r * factor, 255.0f));
            newColor.g = (unsigned char)(std::min(g * factor, 255.0f));
            newColor.b = (unsigned char)(std::min(b * factor, 255.0f));
            return newColor;
        }

        Color brighter()
        {
            float h, s, l;
            RGBtoHSL(r, g, b, h, s, l);

            // Adjust the lightness
            if (l < 0.5f)
                l = 0.7f; // Set lightness to 70% for darker colors
            else
                l = std::min(l + 0.3f, 1.0f); // Increase lightness for lighter colors

            // Convert back to RGB
            unsigned char newR, newG, newB;
            HSLtoRGB(h, s, l, newR, newG, newB);

            return Color(newR, newG, newB, a);
        }

		// string
		const std::string str() const { return std::string("(") + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a) + ")"; }
		friend std::ostream& operator << (std::ostream& os, const Color& c) { os << c.str(); return os; }
	};

    // Same normalized HSV inputs as above; returns an opaque RGB color (alpha = 255).
    inline static Color HSVtoRGB(float h, float s, float v)
    {
        unsigned char r, g, b;
        HSVtoRGB(h, s, v, r, g, b);
        return { r, g, b };
    }

    // Fixed palette, indexed from 0 to 14, ordered by increasing hue.
    // Black has no defined hue and is kept last.
    inline static Color distinctColor(int n)
    {
        static const Color colors[] = {
            { 128, 0, 0 },     // maroon
            { 245, 130, 48 },  // orange
            { 170, 110, 40 },  // brown
            { 255, 255, 25 },  // yellow
            { 128, 128, 0 },   // olive
            { 210, 245, 60 },  // lime
            { 60, 180, 75 },   // green
            { 70, 240, 240 },  // cyan
            { 0, 128, 128 },   // teal
            { 0, 130, 200 },   // blue
            { 0, 0, 128 },     // navy
            { 145, 30, 180 },  // purple
            { 240, 50, 230 },  // magenta
            { 230, 25, 75 },   // red
            { 0, 0, 0 }       // black
        };
        constexpr int colorCount = sizeof(colors) / sizeof(colors[0]);
        if (n >= 0 && n < colorCount)
            return colors[n];

        std::cerr << "Cannot generate " << n << "-th distinct color\n";
        return { 0, 0, 0 };
    }
}