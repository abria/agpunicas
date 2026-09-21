// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "geometryUtils.h"
#include "mathUtils.h"
#include "fileUtils.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#ifdef WITH_TTF
    #include <SDL3_ttf/SDL_ttf.h>
#endif

namespace agp
{
    static inline void DrawThickRect(SDL_Renderer* renderer, SDL_FRect rect, float thickness) 
    {
        // Top side
        SDL_FRect top = { rect.x, rect.y, rect.w, thickness };
        SDL_RenderFillRect(renderer, &top);

        // Bottom side
        SDL_FRect bottom = { rect.x, rect.y + rect.h - thickness, rect.w, thickness };
        SDL_RenderFillRect(renderer, &bottom);

        // Left side
        SDL_FRect left = { rect.x, rect.y + thickness, thickness, rect.h - 2 * thickness };
        SDL_RenderFillRect(renderer, &left);

        // Right side
        SDL_FRect right = { rect.x + rect.w - thickness, rect.y + thickness, thickness, rect.h - 2 * thickness };
        SDL_RenderFillRect(renderer, &right);
    }

    static inline void DrawCircle(SDL_Renderer* renderer, const PointF& center, float radius, const Color& color, int nSegments = 100, float angleStart = 0, float angleEnd = 2 * PI)
    {
        float angleStep = (angleEnd - angleStart) / nSegments;

        for (int i = 0; i < nSegments; ++i)
        {
            float angle = angleStart + i * angleStep;
            float angleNext = angleStart + (i + 1) * angleStep;

            SDL_FPoint a = { center.x + radius * cosf(angle),      center.y + radius * sinf(angle) };
            SDL_FPoint b = { center.x + radius * cosf(angleNext),  center.y + radius * sinf(angleNext) };

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderLine(renderer, a.x, a.y, b.x, b.y);
        }
    }

    static inline void DrawCapsule(SDL_Renderer* renderer, const PointF& centerDown, const PointF& centerUp, float radius, const Color& color, int nSegments = 100)
    {

        float angle = std::atan2(centerUp.y - centerDown.y, centerUp.x - centerDown.x) + PI / 2;
        DrawCircle(renderer, centerUp, radius, color, nSegments, angle - PI, angle);
        DrawCircle(renderer, centerDown, radius, color, nSegments, angle, PI + angle);

        SDL_FPoint aUp = { centerUp.x + radius * cosf(angle - PI), centerUp.y + radius * sinf(angle - PI) };
        SDL_FPoint bUp = { centerUp.x + radius * cosf(angle), centerUp.y + radius * sinf(angle) };

        SDL_FPoint bDown = { centerDown.x + radius * cosf(angle), centerDown.y + radius * sinf(angle) };
        SDL_FPoint aDown = { centerDown.x + radius * cosf(PI + angle), centerDown.y + radius * sinf(PI + angle) };

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderLine(renderer, aUp.x, aUp.y, aDown.x, aDown.y);
        SDL_RenderLine(renderer, bUp.x, bUp.y, bDown.x, bDown.y);
    }

    static inline void DrawOBB(SDL_Renderer* renderer, const RotatedRectF& obb, const Color& color)
    {
        auto vertices = obb.vertices();
        for (int k = 0; k < 4; k++)
        {
            SDL_FPoint a = vertices[k].toSDLf();
            SDL_FPoint b = vertices[(k + 1) % 4].toSDLf();
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderLine(renderer, a.x, a.y, b.x, b.y);
        }
    }

    static inline void DrawOBB(SDL_Renderer* renderer, std::array < PointF, 4> obb, const Color& color)
    {
        for (int k = 0; k < 4; k++)
        {
            SDL_FPoint a = obb[k].toSDLf();
            SDL_FPoint b = obb[(k + 1) % 4].toSDLf();
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderLine(renderer, a.x, a.y, b.x, b.y);
        }
    }

    static inline void DrawThickOBB(SDL_Renderer* renderer, std::array < PointF, 4> obb, float thickness, const Color& color) 
    {
        // Prepare vertices and indices
        SDL_Vertex vertices[16];
        int vertexCount = 0;
        int indices[24];
        int indexCount = 0;

        // For each edge
        for (int i = 0; i < 4; ++i) 
        {
            int next = (i + 1) % 4;
            PointF p1 = obb[i];
            PointF p2 = obb[next];

            // Compute the edge direction
            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            float length = sqrtf(dx * dx + dy * dy);

            // Normalize
            float dx_norm = dx / length;
            float dy_norm = dy / length;

            // Perpendicular vector
            float nx = -dy_norm;
            float ny = dx_norm;

            // Offset for thickness
            float halfThickness = thickness / 2.0f;
            float offsetX = nx * halfThickness;
            float offsetY = ny * halfThickness;

            // Quad vertices
            PointF quad[4];
            quad[0] = { p1.x + offsetX, p1.y + offsetY };
            quad[1] = { p1.x - offsetX, p1.y - offsetY };
            quad[2] = { p2.x - offsetX, p2.y - offsetY };
            quad[3] = { p2.x + offsetX, p2.y + offsetY };

            // Add vertices
            for (int j = 0; j < 4; ++j) 
            {
                vertices[vertexCount].position.x = quad[j].x;
                vertices[vertexCount].position.y = quad[j].y;
                vertices[vertexCount].color = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
                vertexCount++;
            }

            // Define indices for two triangles
            indices[indexCount++] = vertexCount - 4; // First triangle
            indices[indexCount++] = vertexCount - 3;
            indices[indexCount++] = vertexCount - 2;

            indices[indexCount++] = vertexCount - 4; // Second triangle
            indices[indexCount++] = vertexCount - 2;
            indices[indexCount++] = vertexCount - 1;
        }

        // Render the geometry
        SDL_RenderGeometry(renderer, nullptr, vertices, vertexCount, indices, indexCount);
    }

    static inline void FillOBB(SDL_Renderer* renderer, std::array < PointF, 4> obb, const Color& color)
    {
        // use SDL_RenderGeometry to draw 2 filled triangles per rect
        std::array< SDL_Vertex, 4> SDL_vertices;
        for (int i = 0; i < 4; i++)
            SDL_vertices[i] =
        {
            SDL_FPoint{obb[i].x,obb[i].y},
            SDL_FColor {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
            SDL_FPoint {0}
        };
        std::array< int, 6> SDL_indices = { 0, 1, 2, 2, 3, 0 };
        SDL_RenderGeometry(renderer, nullptr, &SDL_vertices[0], 4, &SDL_indices[0], 6);
    }

    // load image from file into texture
    static inline SDL_Texture* loadTexture(
        SDL_Renderer* renderer,
        const std::string& filepath,
        const Color& mask = { 0,0,0,0 })
    {
        // load from file
        SDL_Surface* surf = IMG_Load(filepath.c_str());
        if (!surf)
            throw strprintf("Failed to load texture file %s: %s", filepath.c_str(), SDL_GetError());

        // set transparent color
        if (mask.a)
            SDL_SetSurfaceColorKey(surf, true, SDL_MapSurfaceRGB(surf, mask.r, mask.g, mask.b));

        // create texture from surf
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_DestroySurface(surf);
        if (!tex)
            throw strprintf("Failed to convert surf to texture for %s: %s", filepath.c_str(), SDL_GetError());
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);

        return tex;
    }

    // load texture sequence from multiple files into a single texture, row-wise
    static inline SDL_Texture* loadTextureSequence(
        SDL_Renderer* renderer,
        const std::string& folderPath,
        std::vector< RectI>& rects,             // output autotiles
        const Point& adjustPos = Point(),
        const Point& adjustSize = Point(),
        const std::string& filter = "*.png",
        const Color& mask = { 0,0,0,0 })
    {
        // load images and ensure they have the same dimensions
        std::vector<SDL_Surface*> surfaces;
        surfaces.reserve(100);
        int image_width = 0, image_height = 0;
        auto files = getFilesInDirectory(folderPath, filter);
        for (int k = 0; k < files.size(); k++)
        {
            std::string filepath = folderPath + "/" + files[k];
            SDL_Surface* surf = IMG_Load(filepath.c_str());
            if (!surf)
            {
                SDL_Log("Failed to load texture file %s: %s", filepath.c_str(), SDL_GetError());
                continue;
            }

            // set transparent color
            if (mask.a)
                SDL_SetSurfaceColorKey(surf, true, SDL_MapSurfaceRGB(surf, mask.r, mask.g, mask.b));

            // set image dimensions based on the first image
            if (k == 0)
            {
                image_width = surf->w;
                image_height = surf->h;
            }
            // verify that all images have the same dimensions
            else if (surf->w != image_width || surf->h != image_height)
            {
                printf("loadTextureSequence(): Image %s size (%d, %d) does not match expected size (%d, %d), SKIPPED\n", filepath.c_str(), surf->w, surf->h, image_width, image_height);
                SDL_DestroySurface(surf);
                continue;
            }

            surfaces.push_back(surf);
        }

        if (surfaces.empty())
            throw strprintf("loadTextureSequence(): No images loaded.\n");

        // retrieve renderer's maximum texture size
        SDL_PropertiesID props = SDL_GetRendererProperties(renderer);
        int max_texture_width = int(SDL_GetNumberProperty(props, SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 0));
        int max_texture_height = max_texture_width;
        if (!max_texture_width || !max_texture_height)
        {
            max_texture_width = image_width * int(surfaces.size());
            max_texture_height = image_height * int(surfaces.size());
        }

        // calculate the number of images per row and the total number of rows
        int images_per_row = max_texture_width / image_width;
        int total_images = int(surfaces.size());
        int num_rows = total_images / images_per_row + 1;

        // check total height does not exceed max_texture_height
        int total_height = num_rows * image_height;
        if (total_height > max_texture_height)
            throw strprintf("loadTextureSequence(): Cannot fit images within maximum texture size.\n");

        // correct total width if there is just one row
        int total_width = images_per_row * image_width;
        if (num_rows == 1)
            total_width = image_width * total_images;

        // Create a big surf to hold all images in a grid
        SDL_Surface* big_surface = SDL_CreateSurface(total_width, total_height, surfaces[0]->format);
        if (!big_surface)
            throw strprintf("Failed to create big surf: %s", SDL_GetError());

        // blit each image onto the big surf at the correct grid position
        for (int i = 0; i < surfaces.size(); ++i)
        {
            int row = i / images_per_row;
            int col = i % images_per_row;

            SDL_Rect dest_rect = { col * image_width, row * image_height, image_width, image_height };
            SDL_BlitSurface(surfaces[i], NULL, big_surface, &dest_rect);

            RectI rect(dest_rect.x, dest_rect.y, dest_rect.w, dest_rect.h);
            rect.adjust(adjustPos.x, adjustPos.y, adjustSize.x, adjustSize.y);
            rects.push_back(rect);

            SDL_DestroySurface(surfaces[i]);
        }
        surfaces.clear();

        // set transparent color
        if (mask.a)
            SDL_SetSurfaceColorKey(big_surface, true, SDL_MapSurfaceRGB(big_surface, mask.r, mask.g, mask.b));

        // create a texture from the big surf
        SDL_Texture* result = SDL_CreateTextureFromSurface(renderer, big_surface);
        if (!result)
            throw strprintf("Failed to convert big surf to big texture for %s: %s", folderPath.c_str(), SDL_GetError());
        SDL_SetTextureScaleMode(result, SDL_SCALEMODE_NEAREST);

        SDL_DestroySurface(big_surface);

        return result;
    }

    // load image from file into texture and detect rects row-wise automatically
    static inline SDL_Texture* loadTextureAutoDetect(
        SDL_Renderer* renderer,
        const std::string& filepath,
        std::vector< std::vector < RectI > >& rects,// output: detected sprite rectangles grouped by rows 
        const Color& backgroundMask,                // color considered as background/empty area in the spritesheet
        const Color& spriteMask,                    // color used to mark sprite pixels and as transparency key
        int yDistanceThreshold = 5,                 // max vertical distance to consider sprites as belonging to the same row
        bool detectCornerWithBackgroundOnly = false,// if true, detect sprite corners by any non-background pixel
        bool alignYCenters = true,                  // if true, group rects by comparing their vertical centers instead of top edges
        bool verbose = false)
    {
        SDL_Surface* surf = IMG_Load(filepath.c_str());
        if (!surf)
            throw strprintf("Failed to load texture file %s: %s", filepath.c_str(), SDL_GetError());

        const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(surf->format);

        if (format->bytes_per_pixel != 3 && format->bytes_per_pixel != 4)
            throw strprintf("Unsupported image format in %s. Only 24-bit and 32-bit images are supported.", filepath.c_str());

        if (verbose)
        {
            printf("\n\nImage loaded from %s\n", filepath.c_str());
            printf("   format->bytes_per_pixel = %d\n", format->bytes_per_pixel);
            printf("   format->bits_per_pixel = %d\n", format->bits_per_pixel);
            printf("   format->format = %d\n", format->format);
            printf("   format->SDL_PIXELFORMAT_ABGR8888 = %s\n", format->format == SDL_PIXELFORMAT_ABGR8888 ? "YES" : "no");
            printf("   format->SDL_PIXELFORMAT_BGRA8888 = %s\n", format->format == SDL_PIXELFORMAT_BGRA8888 ? "YES" : "no");
            
        }

        // Helper function to get pixel color
        auto getPixelColor = [format](Uint8* pixelPtr) -> Color
        {
            Uint32 pixelValue;
            if (format->bytes_per_pixel == 3)
            {
                // For 24-bit images, read 3 bytes
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                    pixelValue = pixelPtr[0] << 16 | pixelPtr[1] << 8 | pixelPtr[2];
                else
                    pixelValue = pixelPtr[0] | pixelPtr[1] << 8 | pixelPtr[2] << 16;
            }
            else // format->bytes_per_pixel == 4
            {
                pixelValue = *(Uint32*)pixelPtr;
            }

            Uint8 r = (pixelValue & format->Rmask) >> format->Rshift;
            Uint8 g = (pixelValue & format->Gmask) >> format->Gshift;
            Uint8 b = (pixelValue & format->Bmask) >> format->Bshift;
            Uint8 a = format->Amask ? (pixelValue & format->Amask) >> format->Ashift : 255;

            // Adjust component sizes if necessary
            if ((8 - format->Rbits))
                r = (r << (8 - format->Rbits)) + (r >> (8 - (8 - format->Rbits)));
            if ((8 - format->Gbits))
                g = (g << (8 - format->Gbits)) + (g >> (8 - (8 - format->Gbits)));
            if ((8 - format->Bbits))
                b = (b << (8 - format->Bbits)) + (b >> (8 - (8 - format->Bbits)));
            if ((8 - format->Abits))
                a = (a << (8 - format->Abits)) + (a >> (8 - (8 - format->Abits)));

            return Color(r, g, b, a);
        };

        // Helper function to set pixel color
        auto setPixelColor = [format](Uint8* pixelPtr, const Color& color)
        {
            Uint32 pixelValue =
                ((color.r >> (8 - format->Rbits)) << format->Rshift) |
                ((color.g >> (8 - format->Gbits)) << format->Gshift) |
                ((color.b >> (8 - format->Bbits)) << format->Bshift) |
                (format->Amask ? ((color.a >> (8 - format->Abits)) << format->Ashift) : 0);

            if (format->bytes_per_pixel == 3)
            {
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                {
                    pixelPtr[0] = (pixelValue >> 16) & 0xFF;
                    pixelPtr[1] = (pixelValue >> 8) & 0xFF;
                    pixelPtr[2] = pixelValue & 0xFF;
                }
                else
                {
                    pixelPtr[0] = pixelValue & 0xFF;
                    pixelPtr[1] = (pixelValue >> 8) & 0xFF;
                    pixelPtr[2] = (pixelValue >> 16) & 0xFF;
                }
            }
            else // format->bytes_per_pixel == 4
            {
                *(Uint32*)pixelPtr = pixelValue;
            }
        };

        // Corner-based rectangles detection
        std::vector<RectI> allRects;
        SDL_LockSurface(surf);
        Uint8* pixels = static_cast<Uint8*>(surf->pixels);
        int width = surf->w;
        int height = surf->h;
        int pitch = surf->pitch; // Number of bytes in a row (may include padding)
        Uint8 bpp = format->bytes_per_pixel;

        for (int y = 0; y < height; y++)
        {
            Uint8* row = pixels + y * pitch;
            Uint8* rowPrev = y > 0 ? pixels + (y - 1) * pitch : nullptr;
            for (int x = 0; x < width; x++)
            {
                Uint8* pixelPtr = row + x * bpp;
                Color pixel = getPixelColor(pixelPtr);

                Color pixelN = y > 0 ? getPixelColor(rowPrev + x * bpp) : backgroundMask;
                Color pixelNW = (y > 0 && x > 0) ? getPixelColor(rowPrev + (x - 1) * bpp) : backgroundMask;
                Color pixelW = x > 0 ? getPixelColor(pixelPtr - bpp) : backgroundMask;

                // Up-left corner detection
                if ((detectCornerWithBackgroundOnly ? pixel != backgroundMask : pixel == spriteMask) &&
                    pixelN == backgroundMask &&
                    pixelNW == backgroundMask &&
                    pixelW == backgroundMask)
                {
                    // Up-right corner detection
                    int right = x;
                    for (; right < width; right++)
                    {
                        Color currentPixel = getPixelColor(row + right * bpp);
                        if (currentPixel == backgroundMask)
                            break;
                    }

                    // Bottom-right corner detection
                    int bottom = y;
                    for (; bottom < height; bottom++)
                    {
                        Uint8* bottomRow = pixels + bottom * pitch;
                        Color currentPixel = getPixelColor(bottomRow + (right - 1) * bpp);
                        if (currentPixel == backgroundMask)
                            break;
                    }

                    allRects.push_back(RectI(x, y, right - x, bottom - y));
                }
            }
        }

        if (allRects.empty())
        {
            SDL_UnlockSurface(surf);
            SDL_DestroySurface(surf);
            throw strprintf("Unable to extract auto tiles from texture file %s", filepath.c_str());
        }

        // Eliminate background to allow rect minor adjustments
        for (int y = 0; y < height; y++)
        {
            Uint8* row = pixels + y * pitch;
            for (int x = 0; x < width; x++)
            {
                Uint8* pixelPtr = row + x * bpp;
                Color currentPixel = getPixelColor(pixelPtr);
                if (currentPixel == backgroundMask)
                    setPixelColor(pixelPtr, spriteMask);
            }
        }
        SDL_UnlockSurface(surf);

        // Group rects row-wise
        std::sort(allRects.begin(), allRects.end(),
            [yDistanceThreshold, alignYCenters](const RectI& a, const RectI& b)
            {
                return std::abs(alignYCenters ? a.center().y - b.center().y : a.pos.y - b.pos.y) > yDistanceThreshold
                    ? a.center().y < b.center().y
                    : a.pos.x < b.pos.x;
            });
        rects.push_back(std::vector<RectI>());
        for (size_t k = 0; k < allRects.size(); k++)
        {
            if (k && allRects[k].left() < allRects[k - 1].right())
                rects.push_back(std::vector<RectI>());
            rects.back().push_back(allRects[k]);
        }

        if (verbose)
        {
            printf("\nloadTextureAutoDetect:\n");
            printf("extracted %d rects in total\n", static_cast<int>(allRects.size()));
            for (size_t i = 0; i < rects.size(); i++)
            {
                printf("row %02d: %d rects\n", static_cast<int>(i), static_cast<int>(rects[i].size()));
                for (size_t j = 0; j < rects[i].size(); j++)
                    printf("[%d %d %d %d] ", rects[i][j].pos.x, rects[i][j].pos.y, rects[i][j].size.x, rects[i][j].size.y);
                printf("\n");
            }
        }

        // Set transparent color
        Uint32 spriteMaskPixelValue =
            ((spriteMask.r >> (8 - format->Rbits)) << format->Rshift) |
            ((spriteMask.g >> (8 - format->Gbits)) << format->Gshift) |
            ((spriteMask.b >> (8 - format->Bbits)) << format->Bshift);

        SDL_SetSurfaceColorKey(surf, true, spriteMaskPixelValue);

        // Create texture from surface
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_DestroySurface(surf);
        if (!tex)
            throw strprintf("Failed to convert surface to texture for %s: %s", filepath.c_str(), SDL_GetError());
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);

        return tex;
    }

    // move rect within spritesheet
    static inline RectI moveBy(
        RectI srcRect,
        int x, int y,
        int dx = 16, int dy = 16,
        int border_x = 1, int border_y = 1)
    {
        RectI dstRect = srcRect;
        dstRect.pos = { srcRect.pos.x + x * dx + x * border_x, srcRect.pos.y + y * dy + y * border_y };
        return dstRect;
    }

    // Union-Find data structure for connected component labeling
    class UnionFind
    {
        private:

            std::vector<int> parent;

        public:

            UnionFind(int size) : parent(size)
            {
                for (int i = 0; i < size; ++i)
                    parent[i] = i;
            }

            int Find(int x)
            {
                if (parent[x] != x)
                    parent[x] = Find(parent[x]); // Path compression
                return parent[x];
            }

            void Union(int x, int y)
            {
                int xroot = Find(x);
                int yroot = Find(y);
                if (xroot != yroot)
                    parent[yroot] = xroot;
            }
    };

    // load image from file into texture and detect rects with connected component labeling
    static inline SDL_Texture* loadTextureConnectedComponents(
        SDL_Renderer* renderer,
        const std::string& filepath,
        std::vector< RectI >& rects,    // output: bounding boxes of detected connected components (sprites)
        const Color& backgroundMask,    // color considered as background to separate foreground components
        int yDistanceThreshold = 5,     // max vertical distance to consider components as belonging to the same row
        bool alignYCenters = true,      // if true, group rects by comparing their vertical centers instead of top edges
        bool verbose = false)
    {
        // load image
        SDL_Surface* surface = IMG_Load(filepath.c_str());
        if (!surface)
            throw strprintf("Failed to load texture file %s: %s", filepath.c_str(), SDL_GetError());

        // retrieve basic metadata
        SDL_LockSurface(surface);
        int width = surface->w;
        int height = surface->h;
        int pitch = surface->pitch;
        Uint8* pixels = (Uint8*)surface->pixels;
        const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(surface->format);
        SDL_Palette* palette = SDL_GetSurfacePalette(surface);
        int BytesPerPixel = format->bytes_per_pixel;

        // check format
        if (BytesPerPixel < 3 || BytesPerPixel > 4)
            throw strprintf("Unsupported bitdepth (must be 24 or 32 bits per pixel).\n");

        // initialize labels and union-find structure
        int nextLabel = 1;
        std::vector<int> labels(width * height, 0);
        UnionFind uf(width * height);

        // data structure to store bounding boxes: label -> (minX, minY, maxX, maxY)
        std::unordered_map<int, RectI> boundingBoxes;

        // FIRST PASS: assign provisional labels and record equivalences
        for (int y = 0; y < height; ++y)
        {
            Uint8* row = pixels + y * pitch;
            for (int x = 0; x < width; ++x)
            {
                Uint8* pixel = row + x * BytesPerPixel;
                Uint8 r, g, b;

                Uint32 pixelValue = 0;
                memcpy(&pixelValue, pixel, BytesPerPixel);
                SDL_GetRGB(pixelValue, format, palette, &r, &g, &b);

                // determine if the pixel is foreground or background
                bool isForeground = (r != backgroundMask.r && g != backgroundMask.g && b != backgroundMask.b);
                int currentIdx = y * width + x;
                std::vector<int> neighborLabels;

                // define neighbor offsets based on connectivity
                std::vector<std::pair<int, int>> neighborOffsets;

                if (isForeground)
                {
                    // Use 8-connectivity for foreground pixels
                    neighborOffsets =
                    {
                        {-1, -1}, {-1, 0}, {-1, 1}, { 0, -1}
                    };
                }
                else
                {
                    // Use 4-connectivity for background pixels
                    neighborOffsets =
                    {
                        {-1, 0}, { 0, -1}
                    };
                }

                // Check neighbors
                for (const auto& offset : neighborOffsets)
                {
                    int ny = y + offset.first;
                    int nx = x + offset.second;
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height)
                    {
                        int neighborIdx = ny * width + nx;
                        int neighborLabel = labels[neighborIdx];

                        // Check if neighbor is of the same type (foreground or background)
                        Uint8* neighborPixel = pixels + ny * pitch + nx * BytesPerPixel;
                        Uint8 nr, ng, nb;

                        Uint32 neighborPixelValue = 0;
                        memcpy(&neighborPixelValue, neighborPixel, BytesPerPixel);
                        SDL_GetRGB(neighborPixelValue, format, palette, &nr, &ng, &nb);

                        bool neighborIsForeground = (nr != backgroundMask.r && ng != backgroundMask.g && nb != backgroundMask.b);

                        if (neighborIsForeground == isForeground && neighborLabel != 0)
                            neighborLabels.push_back(neighborLabel);
                    }
                }

                if (neighborLabels.empty())
                {
                    // Assign a new label
                    labels[currentIdx] = nextLabel;
                    nextLabel++;

                    // Initialize bounding box for the new label
                    boundingBoxes[labels[currentIdx]] = { x, y, x, y };
                }
                else
                {
                    // Assign the smallest neighbor label
                    int minLabel = *std::min_element(neighborLabels.begin(), neighborLabels.end());
                    labels[currentIdx] = minLabel;

                    // Union the labels
                    for (int l : neighborLabels)
                        uf.Union(minLabel, l);
                }

                // Update bounding box for the current label
                int label = labels[currentIdx];
                if (boundingBoxes.find(label) == boundingBoxes.end())
                    boundingBoxes[label] = { x, y, x, y };
                else
                {
                    RectI& bbox = boundingBoxes[label];
                    if (x < bbox.pos.x) bbox.pos.x = x;
                    if (y < bbox.pos.y) bbox.pos.y = y;
                    if (x > bbox.size.x) bbox.size.x = x;
                    if (y > bbox.size.y) bbox.size.y = y;
                }
            }
        }

        // SECOND PASS: Resolve labels and update bounding boxes
        std::unordered_map<int, int> labelMap;
        std::unordered_map<int, RectI> resolvedBoundingBoxes;
        int currentForegroundLabel = 1;
        int currentBackgroundLabel = 1;

        for (int i = 0; i < width * height; ++i)
        {
            int label = labels[i];
            if (label != 0)
            {
                int rootLabel = uf.Find(label);

                // Determine if the pixel is foreground or background
                int y = i / width;
                int x = i % width;
                Uint8* pixel = pixels + y * pitch + x * BytesPerPixel;
                Uint8 r, g, b;

                Uint32 pixelValue = 0;
                memcpy(&pixelValue, pixel, BytesPerPixel);
                SDL_GetRGB(pixelValue, format, palette, &r, &g, &b);

                bool isForeground = (r != backgroundMask.r && g != backgroundMask.g && b != backgroundMask.b);

                // Map labels separately for foreground and background
                int resolvedLabel;
                if (isForeground)
                {
                    if (labelMap.find(rootLabel) == labelMap.end())
                    {
                        labelMap[rootLabel] = currentForegroundLabel;
                        resolvedLabel = currentForegroundLabel;
                        currentForegroundLabel++;
                    }
                    else
                        resolvedLabel = labelMap[rootLabel];
                    labels[i] = resolvedLabel;
                }
                else
                {
                    if (labelMap.find(rootLabel) == labelMap.end())
                    {
                        labelMap[rootLabel] = -currentBackgroundLabel; // Negative labels for background
                        resolvedLabel = -currentBackgroundLabel;
                        currentBackgroundLabel++;
                    }
                    else
                        resolvedLabel = labelMap[rootLabel];
                    labels[i] = resolvedLabel;
                }

                // Update bounding boxes with resolved labels
                if (resolvedBoundingBoxes.find(resolvedLabel) == resolvedBoundingBoxes.end())
                    resolvedBoundingBoxes[resolvedLabel] = boundingBoxes[label];
                else
                {
                    RectI& bbox = resolvedBoundingBoxes[resolvedLabel];
                    RectI& originalBbox = boundingBoxes[label];
                    if (originalBbox.pos.x < bbox.pos.x) bbox.pos.x = originalBbox.pos.x;
                    if (originalBbox.pos.y < bbox.pos.y) bbox.pos.y = originalBbox.pos.y;
                    if (originalBbox.size.x > bbox.size.x) bbox.size.x = originalBbox.size.x;
                    if (originalBbox.size.y > bbox.size.y) bbox.size.y = originalBbox.size.y;
                }
            }
        }

        // retrieve rects (label >= 0)
        std::map<int, RectI> orderedComponents(resolvedBoundingBoxes.begin(), resolvedBoundingBoxes.end());
        for (auto& elem : orderedComponents)
            if (elem.first >= 0)
                rects.push_back(RectI(elem.second.pos.x, elem.second.pos.y, elem.second.size.x - elem.second.pos.x + 1, elem.second.size.y - elem.second.pos.y + 1));

        if (rects.empty())
            throw(strprintf("Unable to extract connected components from texture file %s", filepath.c_str()));

        // group row-wise
        std::sort(rects.begin(), rects.end(), [yDistanceThreshold, alignYCenters](const RectI& a, const RectI& b)
            {
                return std::abs(alignYCenters ? a.center().y - b.center().y : a.pos.y - b.pos.y) > yDistanceThreshold ? a.center().y < b.center().y : a.pos.x < b.pos.x;
            });

        if (verbose)
        {
            printf("Extracted %zu components\n", rects.size());
            for (int i = 0; i < rects.size(); i++)
                printf("%d: %s\n", i, rects[i].str().c_str());
        }

        SDL_UnlockSurface(surface);

        // set transparent color
        SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface, backgroundMask.r, backgroundMask.g, backgroundMask.b));

        // create texture from surf
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        if (!tex)
            throw strprintf("Failed to convert surf to texture for %s: %s", filepath.c_str(), SDL_GetError());
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);

        return tex;
    }

#ifdef WITH_TTF
    static inline SDL_Texture* generateText(
        const std::string& text,
        SDL_Renderer* renderer,
        TTF_Font* font,
        const Color& fontColor,
        int fontStyle,
        int wrapLength = 0)
    {
        TTF_SetFontStyle(font, fontStyle);
        TTF_SetFontHinting(font, TTF_HINTING_NORMAL);
        SDL_Color textColor = { fontColor.r, fontColor.g, fontColor.b, fontColor.a };
        SDL_Surface* textSurface = nullptr;
        textSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), text.size(), textColor, wrapLength);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_SetTextureScaleMode(textTexture, SDL_SCALEMODE_LINEAR);
        SDL_DestroySurface(textSurface);

        return textTexture;
    }
#endif

    // Function to get the perpendicular cursor, considering yUp
    static inline SDL_SystemCursor getPerpendicularCursor(const Vec2Df& p1, const Vec2Df& p2, bool yUp)
    {
        // Compute the direction vector of the segment
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;

        // Adjust dy based on yUp
        if (!yUp)
            dy = -dy;

        // Compute the angle of the segment in degrees
        float theta = std::atan2(dy, dx) * 180.0f / std::acos(-1.0f);

        // Compute the angle perpendicular to the segment
        float theta_perp = theta + 90.0f;

        // Normalize angle to the range [0, 360)
        if (theta_perp < 0.0f)
            theta_perp += 360.0f;
        else if (theta_perp >= 360.0f)
            theta_perp -= 360.0f;

        // Map the angle to the nearest 45-degree increment
        int angle_index = static_cast<int>((theta_perp + 22.5f) / 45.0f) % 8;

        // Map angle index to cursors
        switch (angle_index)
        {
            case 0: // 0 degrees (East)
            case 4: // 180 degrees (West)
                return SDL_SYSTEM_CURSOR_EW_RESIZE;    // East-West cursor
            case 1: // 45 degrees (Northeast)
            case 5: // 225 degrees (Southwest)
                return SDL_SYSTEM_CURSOR_NESW_RESIZE;  // Northeast-Southwest cursor
            case 2: // 90 degrees (North)
            case 6: // 270 degrees (South)
                return SDL_SYSTEM_CURSOR_NS_RESIZE;    // North-South cursor
            case 3: // 135 degrees (Northwest)
            case 7: // 315 degrees (Southeast)
                return SDL_SYSTEM_CURSOR_NWSE_RESIZE;  // Northwest-Southeast cursor
            default:
                return SDL_SYSTEM_CURSOR_DEFAULT;     // Default arrow cursor (should not reach here)
        }
    }
}
