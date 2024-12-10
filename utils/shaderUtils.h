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

    // Function to generate a mask texture with anti-aliased circle using float inputs
    //static inline SDL_Texture* generateMaskTexture(SDL_Renderer* renderer, float centerX, float centerY, float radius, int screenWidth, int screenHeight) 
    //{
    //    // Create the texture
    //    SDL_Texture* maskTexture = SDL_CreateTexture(
    //        renderer,
    //        SDL_PIXELFORMAT_RGBA8888,
    //        SDL_TEXTUREACCESS_STREAMING,
    //        screenWidth,
    //        screenHeight
    //    );

    //    if (!maskTexture) 
    //    {
    //        SDL_Log("Failed to create mask texture: %s", SDL_GetError());
    //        return nullptr;
    //    }

    //    // Ensure the texture's blend mode is set to blend
    //    SDL_SetTextureBlendMode(maskTexture, SDL_BLENDMODE_BLEND);

    //    // Lock the texture to access pixel data
    //    void* pixels;
    //    int pitch;
    //    if (SDL_LockTexture(maskTexture, NULL, &pixels, &pitch) != 0) 
    //    {
    //        SDL_Log("Failed to lock texture: %s", SDL_GetError());
    //        SDL_DestroyTexture(maskTexture);
    //        return nullptr;
    //    }

    //    // Cast the pixels to a 32-bit integer pointer
    //    Uint32* pixelData = static_cast<Uint32*>(pixels);
    //    int pixelsPerRow = pitch / 4;

    //    // Prepare color values
    //    SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    //    Uint32 opaqueBlack = SDL_MapRGBA(format, 0, 0, 0, 255);

    //    // Iterate over each pixel
    //    for (int y = 0; y < screenHeight; ++y) 
    //    {
    //        float py = static_cast<float>(y) + 0.5f; // Pixel center y-coordinate
    //        for (int x = 0; x < screenWidth; ++x) 
    //        {
    //            float px = static_cast<float>(x) + 0.5f; // Pixel center x-coordinate

    //            float dx = px - centerX;
    //            float dy = py - centerY;
    //            float distance = sqrtf(dx * dx + dy * dy);

    //            // Calculate alpha for anti-aliasing
    //            float edgeDistance = distance - radius;
    //            float alpha;
    //            if (edgeDistance <= -0.5f) 
    //            {
    //                // Inside the circle: fully transparent
    //                alpha = 0.0f;
    //            }
    //            else if (edgeDistance >= 0.5f) 
    //            {
    //                // Outside the circle: fully opaque
    //                alpha = 255.0f;
    //            }
    //            else 
    //            {
    //                // Edge pixels: interpolate alpha
    //                alpha = clamp((edgeDistance + 0.5f) * 255.0f, 0.0f, 255.0f);
    //            }

    //            Uint8 alphaByte = static_cast<Uint8>(alpha);
    //            Uint32 color = SDL_MapRGBA(format, 0, 0, 0, alphaByte);
    //            pixelData[y * pixelsPerRow + x] = color;
    //        }
    //    }

    //    // Unlock the texture
    //    SDL_UnlockTexture(maskTexture);

    //    // Free the pixel format
    //    SDL_FreeFormat(format);

    //    return maskTexture;
    //}

    static inline void lightShader(Uint32* pixels, int width, int height, int pitch, float centerX, float centerY)
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
