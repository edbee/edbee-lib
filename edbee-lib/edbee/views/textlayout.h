#pragma once

#include "edbee/exports.h"

#include <QRectF>

class QTextLayout;

namespace edbee {

class TextLayout;

/// A Virtual textline
class TextLine
{
public:
    TextLayout(TextLayout* layout, TextLine *line);
    virtual ~TextLayout();


    QTextLine* qtextLine_;
};


/// A virtual textlayout
/// This class wraps the QTextLayout used for rendering.
/// Modifying all cursor/positioning related methods, supporting single-cursor items,
/// while rendering multiple QTextLayout characters
class TextLayout
{
public:
    TextLayout();
    virtual ~TextLayout();

    QTextLayout* qTextLayout() const;


    QRectF boundingRect() const;


protected:
    QTextLayout *qtextLayout_;
};

}
