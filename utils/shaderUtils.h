// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

namespace agp
{
    static inline float clamp(float value, float min, float max) {
        return (value < min) ? min : (value > max) ? max : value;
    }

    static inline void circleMaskShader(
        Uint32* pixels,
        int width,
        int height,
        int pitch,
        float centerX,
        float centerY,
        float radius    // relative factor in [0,1] of std::min(width, height)
    )
    {
        int rowPixels = pitch / 4;

        radius *= std::min(width, height);

        for (int y = 0; y < height; ++y)
        {
            float py = static_cast<float>(y) + 0.5f;
            for (int x = 0; x < width; ++x)
            {
                float px = static_cast<float>(x) + 0.5f;

                float dx = px - centerX;
                float dy = py - centerY;
                float distance = std::sqrt(dx * dx + dy * dy);

                float edgeDistance = distance - radius;
                float factor;
                if (edgeDistance <= -0.5f)
                    // Deep inside the circle
                    factor = 0.0f;
                else if (edgeDistance >= 0.5f)
                    // Well outside the circle
                    factor = 1.0f;
                else
                    // On the edge, transition smoothly from 0 to 1
                    // When edgeDistance = -0.5 -> factor = 0
                    // When edgeDistance = 0.5 -> factor = 1
                    factor = clamp((edgeDistance + 0.5f), 0.0f, 1.0f);

                // Fetch the original pixel in RGBA8888
                Uint32 pixel = pixels[y * rowPixels + x];
                Uint8 a = (pixel >> 0) & 0xFF;
                Uint8 b = (pixel >> 8) & 0xFF;
                Uint8 g = (pixel >> 16) & 0xFF;
                Uint8 r = (pixel >> 24) & 0xFF;

                // Blend the original pixel toward black using factor
                // factor=0 => original pixel fully
                // factor=1 => fully black
                float invFactor = 1.0f - factor;
                Uint8 r_new = static_cast<Uint8>(r * invFactor);
                Uint8 g_new = static_cast<Uint8>(g * invFactor);
                Uint8 b_new = static_cast<Uint8>(b * invFactor);

                // Set alpha to 255 to keep the image fully opaque
                Uint8 a_new = 255;

                pixels[y * rowPixels + x] = (r_new << 24) | (g_new << 16) | (b_new << 8) | a_new;
            }
        }
    }

    static inline void lightShader(
        Uint32* pixels, 
        int width, 
        int height, 
        int pitch, 
        float centerX, 
        float centerY)
    {
        // each row has 'pitch' bytes, and each pixel is 4 bytes (RGBA)
        int rowPixels = pitch / 4;

        // define the radius of the light effect
        float maxRadius = std::min(width, height) / 4.0f;
        float radius = maxRadius * 0.75f; // light radius (adjust as needed)

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                // calculate distance from center
                float dx = x - centerX;
                float dy = y - centerY;
                float distance = std::sqrt(dx * dx + dy * dy);

                // determine brightness factor based on distance
                float factor = 1.0f;
                if (distance > radius)
                {
                    // beyond the light radius, apply darkening with a gradient
                    float excess = distance - radius;
                    float gradient = excess / (maxRadius - radius); // Normalize between 0 and 1
                    gradient = std::min(gradient, 1.0f); // Clamp to 1.0f

                    // darken the pixel: factor decreases from 1 to 0.2
                    factor = 1.0f - (0.8f * gradient); // Adjust 0.8f for intensity
                }

                // fetch the pixel
                Uint32 pixel = pixels[y * rowPixels + x];
                Uint8 a = (pixel & 0xFF);
                Uint8 b = (pixel >> 8) & 0xFF;
                Uint8 g = (pixel >> 16) & 0xFF;
                Uint8 r = (pixel >> 24) & 0xFF;

                // apply the brightness factor
                r = static_cast<Uint8>(clamp(r * factor, 0.0f, 255.0f));
                g = static_cast<Uint8>(clamp(g * factor, 0.0f, 255.0f));
                b = static_cast<Uint8>(clamp(b * factor, 0.0f, 255.0f));

                // reassemble the pixel
                pixels[y * rowPixels + x] = (r << 24) | (g << 16) | (b << 8) | a;
            }
        }
    }
}
