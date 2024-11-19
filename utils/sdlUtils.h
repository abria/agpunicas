// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "geometryUtils.h"
#include "mathUtils.h"
#include "fileUtils.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#ifdef WITH_TTF
#include "SDL_ttf.h"
#endif

namespace agp
{
    static inline void DrawThickRect(SDL_Renderer* renderer, SDL_FRect rect, float thickness) 
    {
        // Top side
        SDL_FRect top = { rect.x, rect.y, rect.w, thickness };
        SDL_RenderFillRectF(renderer, &top);

        // Bottom side
        SDL_FRect bottom = { rect.x, rect.y + rect.h - thickness, rect.w, thickness };
        SDL_RenderFillRectF(renderer, &bottom);

        // Left side
        SDL_FRect left = { rect.x, rect.y + thickness, thickness, rect.h - 2 * thickness };
        SDL_RenderFillRectF(renderer, &left);

        // Right side
        SDL_FRect right = { rect.x + rect.w - thickness, rect.y + thickness, thickness, rect.h - 2 * thickness };
        SDL_RenderFillRectF(renderer, &right);
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
            SDL_RenderDrawLineF(renderer, a.x, a.y, b.x, b.y);
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
        SDL_RenderDrawLineF(renderer, aUp.x, aUp.y, aDown.x, aDown.y);
        SDL_RenderDrawLineF(renderer, bUp.x, bUp.y, bDown.x, bDown.y);
    }

    static inline void DrawOBB(SDL_Renderer* renderer, const RotatedRectF& obb, const Color& color)
    {
        auto vertices = obb.vertices();
        for (int k = 0; k < 4; k++)
        {
            SDL_FPoint a = vertices[k].toSDLf();
            SDL_FPoint b = vertices[(k + 1) % 4].toSDLf();
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderDrawLineF(renderer, a.x, a.y, b.x, b.y);
        }
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
            SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, mask.r, mask.g, mask.b));

        // create texture from surf
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        if (!tex)
            throw strprintf("Failed to convert surf to texture for %s: %s", filepath.c_str(), SDL_GetError());

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
                SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, mask.r, mask.g, mask.b));

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
                SDL_FreeSurface(surf);
                continue;
            }

            surfaces.push_back(surf);
        }

        if (surfaces.empty())
            throw strprintf("loadTextureSequence(): No images loaded.\n");

        // retrieve renderer's maximum texture size
        SDL_RendererInfo info;
        SDL_GetRendererInfo(renderer, &info);
        int max_texture_width = info.max_texture_width;
        int max_texture_height = info.max_texture_height;
        if (!max_texture_width || !max_texture_height)
            throw strprintf("loadTextureSequence(): Cannot retrieve renderer max texture size.\n");

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
        SDL_Surface* big_surface = SDL_CreateRGBSurface(0, total_width, total_height, surfaces[0]->format->BitsPerPixel,
            surfaces[0]->format->Rmask, surfaces[0]->format->Gmask, surfaces[0]->format->Bmask, surfaces[0]->format->Amask);
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

            SDL_FreeSurface(surfaces[i]);
        }
        surfaces.clear();

        // set transparent color
        if (mask.a)
            SDL_SetColorKey(big_surface, SDL_TRUE, SDL_MapRGB(big_surface->format, mask.r, mask.g, mask.b));

        // create a texture from the big surf
        SDL_Texture* result = SDL_CreateTextureFromSurface(renderer, big_surface);
        if (!result)
            throw strprintf("Failed to convert big surf to big texture for %s: %s", folderPath.c_str(), SDL_GetError());

        SDL_FreeSurface(big_surface);

        return result;
    }

    // load image from file into texture and detect rects row-wise automatically
    static inline SDL_Texture* loadTextureAutoDetect(
        SDL_Renderer* renderer,
        const std::string& filepath,
        std::vector< std::vector < RectI > >& rects,
        const Color& backgroundMask,
        const Color& spriteMask,
        int yDistanceThreshold = 5,
        bool detectCornerWithBackgroundOnly = false,
        bool alignYCenters = true,
        bool verbose = false)
    {
        SDL_Surface* surf = IMG_Load(filepath.c_str());
        if (!surf)
            throw strprintf("Failed to load texture file %s: %s", filepath.c_str(), SDL_GetError());

        // corner-based rectangles detection
        std::vector < RectI > allRects;
        SDL_LockSurface(surf);
        Uint8* pixels = (Uint8*)surf->pixels;
        int width = surf->w;
        int height = surf->h;
        int pitch = surf->pitch; // Number of bytes in a row (may include padding)
        Uint8 bpp = surf->format->BytesPerPixel;
        for (int y = 0; y < height; y++)
        {
            Uint8* row = pixels + y * pitch;
            Uint8* rowPrev = pixels + (y - 1) * pitch;
            for (int x = 0; x < width; x++)
            {
                Color pixel(row + x * bpp);
                Color pixelN = y ? Color(rowPrev + x * bpp) : backgroundMask;
                Color pixelNW = y ? Color(rowPrev + (x - 1) * bpp) : backgroundMask;
                Color pixelW = x ? Color(row + (x - 1) * bpp) : backgroundMask;

                // up-left corner detection
                if ((detectCornerWithBackgroundOnly ? pixel != backgroundMask : pixel == spriteMask) &&
                    pixelN == backgroundMask &&
                    pixelNW == backgroundMask &&
                    pixelW == backgroundMask)
                {
                    // up-right corner detection
                    int right = x;
                    for (; right < width; right++)
                        if (Color(row + right * bpp) == backgroundMask)
                            break;

                    // bottom-right corner detection
                    int bottom = y;
                    for (; bottom < height; bottom++)
                        if (Color(pixels + bottom * pitch + (right - 1) * bpp) == backgroundMask)
                            break;

                    allRects.push_back(RectI(x, y, right - x, bottom - y));
                }
            }
        }

        if(allRects.empty())
            throw(strprintf("Unable to extract auto tiles from texture file %s", filepath.c_str()));

        // eliminate background to allow rect minor adjustments
        for (int y = 0; y < height; y++)
        {
            Uint8* row = pixels + y * pitch;
            for (int x = 0; x < width; x++)
                if (Color(row + x * bpp) == backgroundMask)
                    for (int c = 0; c < bpp; c++)
                        row[x * bpp + c] = spriteMask[c];
        }
        SDL_UnlockSurface(surf);

        // group rects row-wise
        std::sort(allRects.begin(), allRects.end(), [yDistanceThreshold, alignYCenters](const RectI& a, const RectI& b)
            {
                return std::abs(alignYCenters ? a.center().y - b.center().y : a.pos.y - b.pos.y) > yDistanceThreshold ? a.center().y < b.center().y : a.pos.x < b.pos.x;
            });
        rects.push_back(std::vector <RectI>());
        for (int k = 0; k < allRects.size(); k++)
        {
            if (k && allRects[k].left() < allRects[k - 1].right())
                rects.push_back(std::vector <RectI>());
            rects[rects.size() - 1].push_back(allRects[k]);
        }

        if (verbose)
        {
            printf("\nloadTextureAutoDetect:\n");
            printf("extracted %d rects in total\n", int(allRects.size()));
            for (int i = 0; i < rects.size(); i++)
            {
                printf("row %02d: %d rects\n", i, int(rects[i].size()));
                for (int j = 0; j < rects[i].size(); j++)
                    printf("[%d %d %d %d] ", rects[i][j].pos.x, rects[i][j].pos.y, rects[i][j].size.x, rects[i][j].size.y);
                printf("\n");
            }
        }

        // set transparent color
        SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, spriteMask.r, spriteMask.g, spriteMask.b));

        // create texture from surf
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        if (!tex)
            throw strprintf("Failed to convert surf to texture for %s: %s", filepath.c_str(), SDL_GetError());

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
        std::vector< RectI >& rects,
        const Color& backgroundMask,
        int yDistanceThreshold = 5,
        bool alignYCenters = true,
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
        int BytesPerPixel = surface->format->BytesPerPixel;

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
                SDL_GetRGB(pixelValue, surface->format, &r, &g, &b);

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
                        SDL_GetRGB(neighborPixelValue, surface->format, &nr, &ng, &nb);

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
                SDL_GetRGB(pixelValue, surface->format, &r, &g, &b);

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
            printf("Extracted %llu components\n", rects.size());
            for (int i = 0; i < rects.size(); i++)
                printf("%d: %s\n", i, rects[i].str().c_str());
        }

        SDL_UnlockSurface(surface);

        // set transparent color
        SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, backgroundMask.r, backgroundMask.g, backgroundMask.b));

        // create texture from surf
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!tex)
            throw strprintf("Failed to convert surf to texture for %s: %s", filepath.c_str(), SDL_GetError());

        return tex;
    }

#ifdef WITH_TTF
    static inline SDL_Texture* generateText(
        const std::string& text,
        SDL_Renderer* renderer,
        const std::string& fontPath,
        const Color& fontColor,
        int fontSize,
        int fontStyle)
    {
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font)
            throw strprintf("Cannot open font from %s: %s", fontPath.c_str(), TTF_GetError());
        TTF_SetFontStyle(font, fontStyle);
        TTF_SetFontHinting(font, TTF_HINTING_NORMAL);
        SDL_Color textColor = { fontColor.r, fontColor.g, fontColor.b, fontColor.a };
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_SetTextureScaleMode(textTexture, SDL_ScaleModeBest);
        SDL_FreeSurface(textSurface);

        return textTexture;
    }
#endif
}
