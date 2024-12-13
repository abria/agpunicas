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

    // Performs a 1D box blur horizontally on the image stored in `src` and puts the result in `dest`.
    // kernelSize must be odd.
    // This uses a sliding window approach for O(width) complexity per row.
    static void horizontalBlurPass(
        const Uint32* src, Uint32* dest,
        int width, int height, int pitch,
        int kernelSize
    ) 
    {
        int half = kernelSize / 2;
        int rowPixels = pitch / 4;

        for (int y = 0; y < height; ++y) 
        {
            const Uint32* srcRow = &src[y * rowPixels];
            Uint32* destRow = &dest[y * rowPixels];

            // Initialize running sums for the first window
            int rSum = 0, gSum = 0, bSum = 0, aSum = 0;
            for (int i = -half; i <= half; i++) 
            {
                int x = std::max(0, std::min(i, width - 1));
                Uint32 p = srcRow[x];
                Uint8 a = (p >> 0) & 0xFF;
                Uint8 b = (p >> 8) & 0xFF;
                Uint8 g = (p >> 16) & 0xFF;
                Uint8 r = (p >> 24) & 0xFF;

                rSum += r;
                gSum += g;
                bSum += b;
                aSum += a;
            }

            destRow[0] = 
                ((rSum / kernelSize) << 24) |
                ((gSum / kernelSize) << 16) |
                ((bSum / kernelSize) << 8)  |
                (aSum / kernelSize);

            // Slide the kernel across the row
            for (int x = 1; x < width; x++) 
            {
                // Remove the pixel leaving the window
                int x_old = x - half - 1;
                if (x_old < 0) x_old = 0;

                Uint32 p_old = srcRow[x_old];
                Uint8 aO = (p_old >> 0) & 0xFF;
                Uint8 bO = (p_old >> 8) & 0xFF;
                Uint8 gO = (p_old >> 16) & 0xFF;
                Uint8 rO = (p_old >> 24) & 0xFF;

                rSum -= rO; gSum -= gO; bSum -= bO; aSum -= aO;

                // Add the new pixel entering the window
                int x_new = x + half;
                if (x_new >= width) x_new = width - 1;

                Uint32 p_new = srcRow[x_new];
                Uint8 aN = (p_new >> 0) & 0xFF;
                Uint8 bN = (p_new >> 8) & 0xFF;
                Uint8 gN = (p_new >> 16) & 0xFF;
                Uint8 rN = (p_new >> 24) & 0xFF;

                rSum += rN; gSum += gN; bSum += bN; aSum += aN;

                // Compute average
                destRow[x] = 
                    ((rSum / kernelSize) << 24) |
                    ((gSum / kernelSize) << 16) |
                    ((bSum / kernelSize) << 8)  |
                    (aSum / kernelSize);
            }
        }
    }

    // Performs a 1D box blur vertically on the image stored in `src` and puts the result into `dest`.
    // kernelSize must be odd.
    static void verticalBlurPass(
        const Uint32* src, Uint32* dest,
        int width, int height, int pitch,
        int kernelSize
    ) 
    {
        int half = kernelSize / 2;
        int rowPixels = pitch / 4;

        for (int x = 0; x < width; ++x) 
        {
            // Initialize running sums for the first window in the column
            int rSum = 0, gSum = 0, bSum = 0, aSum = 0;
            for (int i = -half; i <= half; i++) 
            {
                int y = std::max(0, std::min(i, height - 1));
                Uint32 p = src[y * rowPixels + x];
                Uint8 a = (p >> 0) & 0xFF;
                Uint8 b = (p >> 8) & 0xFF;
                Uint8 g = (p >> 16) & 0xFF;
                Uint8 r = (p >> 24) & 0xFF;

                rSum += r;
                gSum += g;
                bSum += b;
                aSum += a;
            }

            dest[0 * rowPixels + x] = 
                ((rSum / kernelSize) << 24) |
                ((gSum / kernelSize) << 16) |
                ((bSum / kernelSize) << 8)  |
                (aSum / kernelSize);

            // Slide the window down the column
            for (int y = 1; y < height; y++) 
            {
                int y_old = y - half - 1;
                if (y_old < 0) y_old = 0;

                Uint32 p_old = src[y_old * rowPixels + x];
                Uint8 aO = (p_old >> 0) & 0xFF;
                Uint8 bO = (p_old >> 8) & 0xFF;
                Uint8 gO = (p_old >> 16) & 0xFF;
                Uint8 rO = (p_old >> 24) & 0xFF;

                rSum -= rO; gSum -= gO; bSum -= bO; aSum -= aO;

                int y_new = y + half;
                if (y_new >= height) y_new = height - 1;

                Uint32 p_new = src[y_new * rowPixels + x];
                Uint8 aN = (p_new >> 0) & 0xFF;
                Uint8 bN = (p_new >> 8) & 0xFF;
                Uint8 gN = (p_new >> 16) & 0xFF;
                Uint8 rN = (p_new >> 24) & 0xFF;

                rSum += rN; gSum += gN; bSum += bN; aSum += aN;

                dest[y * rowPixels + x] = 
                    ((rSum / kernelSize) << 24) |
                    ((gSum / kernelSize) << 16) |
                    ((bSum / kernelSize) << 8)  |
                    (aSum / kernelSize);
            }
        }
    }

    static inline void fadingShader(
        Uint32* pixels,
        int width,
        int height,
        int pitch,
        float factor
    )
    {
        // Clamp factor
        if (factor < 0.0f) factor = 0.0f;
        if (factor > 1.0f) factor = 1.0f;

        // Determine kernel size based on factor. More factor => larger kernel.
        // For example:
        // factor=0 -> kernelSize=1 (no blur)
        // factor=1 -> kernelSize=11 (heavy blur)
        // Adjust this as you like for performance/quality tradeoffs.
        int minKernelSize = 1;
        int maxKernelSize = 31;
        int kernelSize = minKernelSize + static_cast<int>((maxKernelSize - minKernelSize) * factor);
        if (kernelSize < 1) kernelSize = 1;
        if (kernelSize % 2 == 0) kernelSize += 1; // ensure odd kernel size

        if (kernelSize == 1 && factor == 0.0f)
            // No blur needed.
            return;

        int rowPixels = pitch / 4;

        // If we need blending (factor < 1), store original
        Uint32* original = nullptr;
        if (factor < 1.0f) 
        {
            original = new Uint32[width * height];
            for (int y = 0; y < height; ++y)
                for (int x = 0; x < width; ++x)
                    original[y * width + x] = pixels[y * rowPixels + x];
        }

        // If kernelSize == 1, no blur is done, just blend factor<1 means no effect anyway.
        if (kernelSize == 1) 
        {
            // Just blend original and pixels (which are the same), no change:
            if (original) delete[] original;
            return;
        }

        // Temp buffers for intermediate passes
        Uint32* temp = new Uint32[width * height];
        Uint32* temp2 = new Uint32[width * height];

        // Horizontal blur
        horizontalBlurPass(pixels, temp, width, height, pitch, kernelSize);

        // Vertical blur
        verticalBlurPass(temp, temp2, width, height, pitch, kernelSize);

        // Write result back to pixels
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                pixels[y * rowPixels + x] = temp2[y * rowPixels + x];

        // Blend if factor < 1.0
        if (factor < 1.0f) 
        {
            float invFactor = 1.0f - factor;
            for (int y = 0; y < height; ++y) 
            {
                for (int x = 0; x < width; ++x) 
                {
                    Uint32 orig = original[y * width + x];
                    Uint8 aO = (orig >> 0) & 0xFF;
                    Uint8 bO = (orig >> 8) & 0xFF;
                    Uint8 gO = (orig >> 16) & 0xFF;
                    Uint8 rO = (orig >> 24) & 0xFF;

                    Uint32 blurP = pixels[y * rowPixels + x];
                    Uint8 aB = (blurP >> 0) & 0xFF;
                    Uint8 bB = (blurP >> 8) & 0xFF;
                    Uint8 gB = (blurP >> 16) & 0xFF;
                    Uint8 rB = (blurP >> 24) & 0xFF;

                    // Blend: result = original * invFactor + blurred * factor
                    Uint8 rF = (Uint8)(rO * invFactor + rB * factor);
                    Uint8 gF = (Uint8)(gO * invFactor + gB * factor);
                    Uint8 bF = (Uint8)(bO * invFactor + bB * factor);
                    Uint8 aF = (Uint8)(aO * invFactor + aB * factor);

                    pixels[y * rowPixels + x] = (rF << 24) | (gF << 16) | (bF << 8) | aF;
                }
            }
            delete[] original;
        }

        delete[] temp;
        delete[] temp2;
    }
}
