// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorUI.h"
#include "View.h"
#include "Game.h"
#include "Window.h"
#include "TextSprite.h"
#include "EditorStyle.h"
#include "Fonts.h"
#include <sstream>

using namespace agp;

namespace
{
    float helpTextWidth(const std::string& text, float height)
    {
#ifdef WITH_TTF
        TTF_Font* font = Fonts::instance()->font(editorStyle::FONT);
        int w = 0, h = 0;
        if (font)
        {
            TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
            if (TTF_GetStringSize(font, text.c_str(), text.size(), &w, &h) && h > 0)
                return float(w) * height / h;
        }
#endif
        return float(text.size()) * height * 0.5f;
    }

    std::vector<std::string> wrapHelpText(const std::string& text, float width, float height)
    {
        std::vector<std::string> lines;
        std::istringstream words(text);
        std::string word, line;
        while (words >> word)
        {
            std::string candidate = line.empty() ? word : line + " " + word;
            if (!line.empty() && helpTextWidth(candidate, height) > width)
            {
                lines.push_back(line);
                line.clear();
            }
            // Long category names/paths also wrap, without splitting a UTF-8 character.
            if (helpTextWidth(word, height) > width)
            {
                for (size_t pos = 0; pos < word.size();)
                {
                    size_t end = pos + 1;
                    while (end < word.size() && (static_cast<unsigned char>(word[end]) & 0xC0) == 0x80)
                        ++end;
                    std::string character = word.substr(pos, end - pos);
                    if (!line.empty() && helpTextWidth(line + character, height) > width)
                    {
                        lines.push_back(line);
                        line.clear();
                    }
                    line += character;
                    pos = end;
                }
            }
            else
                line += (line.empty() ? "" : " ") + word;
        }
        if (!line.empty())
            lines.push_back(line);
        return lines;
    }
}

EditorUI::EditorUI()
    : UIScene(RectF(0, 0, 1, 1 / Game::instance()->aspectRatio()), { 1,1 })
{
    _cursorCoords = new RenderableObject(this, RectF(0, 0, 1, CURSOR_TEXT_HEIGHT * _rect.size.y),
        new TextSprite("", editorStyle::FONT, editorStyle::TEXT), 3);
    dynamic_cast<TextSprite*>(_cursorCoords->sprite())->setBackgroundColor(editorStyle::PANEL);

    _helpboxPanel = new RenderableObject(this, RectF(), editorStyle::PANEL);
    _helpboxPanel->setBorderThickness(1);
    _helpboxPanel->setVisible(false);
    _helpboxAccent = new RenderableObject(this, RectF(), Color(100, 180, 255), 1);
    _helpboxAccent->setVisible(false);
    for (int i = 0; i < HELPBOX_MAX_ROWS; ++i)
        _helpboxColors[i] = editorStyle::TEXT;

    _view->setFixedAspectRatio(Game::instance()->aspectRatio());
    _view->setRect(_rect);
    _editing = false;
    _autohide = false;
    _helpboxDirty = true;
    _helpboxHovered = false;
    _helpboxOpacity = 1;
    _helpboxHideDelay = 2;
    _cursor = nullptr;
}

EditorUI::~EditorUI()
{
    SDL_DestroyCursor(_cursor);
    _cursorCoords->setSprite(nullptr, true);
    for (auto line : _helpboxLines)
        line->setSprite(nullptr, true);
}

void EditorUI::setCursor(SDL_SystemCursor cursor)
{
    if (_cursor)
        SDL_DestroyCursor(_cursor);
    _cursor = SDL_CreateSystemCursor(cursor);
    SDL_SetCursor(_cursor);
}

void EditorUI::setCursorText(const std::string& text, const Color& textColor)
{
    auto sprite = dynamic_cast<TextSprite*>(_cursorCoords->sprite());
    sprite->setText(text);
    sprite->setColor(textColor.a ? textColor : editorStyle::TEXT);
}

void EditorUI::setHelpboxText(int row, const std::string& text, const Color& textColor)
{
    if (row < 0 || row >= HELPBOX_MAX_ROWS)
        return;
    _helpboxText[row] = text;
    _helpboxColors[row] = textColor.a ? textColor : (row == 0 ? Color(210, 220, 235) : editorStyle::TEXT);
    _helpboxDirty = true;
    _helpboxHideDelay = 2;
}

void EditorUI::clearHelpboxText()
{
    for (auto& text : _helpboxText)
        text.clear();
    _helpboxDirty = true;
}

void EditorUI::layoutHelpbox()
{
    _helpboxDirty = false;
    for (auto line : _helpboxLines)
        line->setVisible(false);
    _helpboxLineColors.clear();

    // Size in window points, independent of world zoom and window aspect ratio.
    SDL_Renderer* renderer = Game::instance()->window()->renderer();
    int windowW = 0, windowH = 0, outputW = 0, outputH = 0;
    SDL_GetWindowSize(SDL_GetRenderWindow(renderer), &windowW, &windowH);
    SDL_GetCurrentRenderOutputSize(renderer, &outputW, &outputH);
    float pixelScale = windowW > 0 ? float(outputW) / windowW : 1;
    const float textHeight = 22 * pixelScale;
    const float gap = 2 * pixelScale;
    const float margin = 8 * pixelScale;
    const float paddingX = 12 * pixelScale, paddingY = 6 * pixelScale;
    RectF area = _view->mapFromScene(_rect);
    float maxWidth = std::max(1.0f, area.size.x - 2 * margin - 2 * paddingX);
    int maxLines = std::max(1, int((area.size.y - 2 * margin - 2 * paddingY) / (textHeight + gap)));
    std::vector<std::string> lines;
    float contentWidth = 0;
    for (int row = HELPBOX_MAX_ROWS - 1; row >= 0; --row)
        for (const auto& text : wrapHelpText(_helpboxText[row], maxWidth, textHeight))
        {
            if (int(lines.size()) >= maxLines)
                break;
            lines.push_back(text);
            _helpboxLineColors.push_back(_helpboxColors[row]);
            contentWidth = std::max(contentWidth, helpTextWidth(text, textHeight));
        }

    _helpboxPanel->setVisible(!lines.empty());
    _helpboxAccent->setVisible(!lines.empty());
    if (lines.empty())
        return;

    float width = std::min(contentWidth + 1, maxWidth) + 2 * paddingX;
    float height = float(lines.size()) * (textHeight + gap) - gap + 2 * paddingY;
    RectF panel(area.pos.x + margin, area.bottom() - margin - height, width, height);
    _helpboxPanel->setRect(_view->mapToScene(panel));
    _helpboxAccent->setRect(_view->mapToScene(RectF(panel.pos.x + 1, panel.pos.y + paddingY,
        2 * pixelScale, height - 2 * paddingY)));
    _helpboxAccent->setColor(_helpboxText[2].empty() ? Color(100, 180, 255) : _helpboxColors[2]);

    for (size_t i = 0; i < lines.size(); ++i)
    {
        if (i == _helpboxLines.size())
            _helpboxLines.push_back(new RenderableObject(this, RectF(),
                new TextSprite("", editorStyle::FONT), 2));
        auto line = _helpboxLines[i];
        line->setRect(_view->mapToScene(RectF(panel.pos.x + paddingX,
            panel.pos.y + paddingY + float(i) * (textHeight + gap),
            std::min(helpTextWidth(lines[i], textHeight) + 1, maxWidth), textHeight)));
        dynamic_cast<TextSprite*>(line->sprite())->setText(lines[i]);
        line->setVisible(true);
    }
    applyHelpboxOpacity();
}

void EditorUI::applyHelpboxOpacity()
{
    int alpha = int(255 * _helpboxOpacity);
    _helpboxPanel->setColor(editorStyle::PANEL.adjustAlpha(alpha));
    _helpboxPanel->setBorderColor(Color(65, 77, 94, alpha));
    _helpboxAccent->setColor(_helpboxAccent->color().adjustAlpha(alpha));
    for (size_t i = 0; i < _helpboxLineColors.size(); ++i)
        dynamic_cast<TextSprite*>(_helpboxLines[i]->sprite())->setColor(_helpboxLineColors[i].adjustAlpha(alpha));
}

void EditorUI::render()
{
    if (_helpboxDirty)
    {
        layoutHelpbox();
        refreshObjects();
    }
    UIScene::render();
}

void EditorUI::update(float timeToSimulate)
{
    UIScene::update(timeToSimulate);
    if (!_active)
        return;
    if (_helpboxHovered || _editing)
        _helpboxHideDelay = 2;
    else
        _helpboxHideDelay = std::max(0.0f, _helpboxHideDelay - timeToSimulate);
    float target = !_autohide || _helpboxHideDelay > 0 ? 1.0f : 0.0f;
    float previous = _helpboxOpacity;
    float step = 6 * timeToSimulate;
    _helpboxOpacity = target > previous ? std::min(target, previous + step) : std::max(target, previous - step);
    if (_helpboxOpacity != previous)
        applyHelpboxOpacity();
}

void EditorUI::toggleAutohide()
{
    _autohide = !_autohide;
    _helpboxHideDelay = 2;
    _helpboxOpacity = 1;
    applyHelpboxOpacity();
}

void EditorUI::event(SDL_Event& evt)
{
    UIScene::event(evt);
    if (evt.type >= SDL_EVENT_WINDOW_FIRST && evt.type <= SDL_EVENT_WINDOW_LAST)
        _helpboxDirty = true;
    if (evt.type == SDL_EVENT_MOUSE_MOTION)
    {
        PointF mouse = _view->mapToScene(evt.motion.x, evt.motion.y);
        _helpboxHovered = _helpboxPanel->rect().contains(mouse);
        if (_helpboxHovered)
            _helpboxHideDelay = 2;
        _cursorCoords->setPos(mouse + PointF{ CURSOR_TEXT_MARGIN_X * _rect.size.x, CURSOR_TEXT_MARGIN_Y * _rect.size.y });
    }
}
