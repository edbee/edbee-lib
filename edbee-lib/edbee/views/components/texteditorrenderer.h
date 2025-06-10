// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

class QLinearGradient;
class QPainter;
class QRect;

namespace edbee {

class TextRenderer;
class TextTheme;

class EDBEE_EXPORT TextEditorRenderer {
public:
    TextEditorRenderer(TextRenderer *renderer);
    virtual ~TextEditorRenderer();

    virtual int preferedWidth();
    virtual void render(QPainter* painter);
    virtual void renderLineBackground(QPainter *painter, size_t line);
    virtual void renderLineSelection(QPainter *painter, size_t line);
    virtual void renderLineBorderedRanges(QPainter *painter, size_t line);
    virtual void renderLineSeparator(QPainter *painter, size_t line);
    virtual void renderLineText(QPainter *painter, size_t line);
    virtual void renderCarets(QPainter *painter);
    virtual void renderShade(QPainter *painter, const QRect& rect);

    virtual int extraPixelsToUpdateAroundLines();

    TextRenderer* renderer() { return rendererRef_; }

private:
    TextRenderer* rendererRef_;       ///< the renderere reference
    TextTheme* themeRef_;             ///< A theem reference used while rendering
    QLinearGradient* shadowGradient_; ///< The shadow gradient to draw
};

} // edbee
