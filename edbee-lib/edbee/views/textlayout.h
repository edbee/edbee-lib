// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QRectF>
#include <QTextLine>
#include <QTextLayout>

#include "edbee/models/textrange.h"

class QTextLayout;

namespace edbee {

class TextLayout;

/// A virtual textlayout
///
/// This class wraps the QTextLayout used for rendering.
/// Modifying all cursor/positioning related methods, supporting single-cursor items,
/// while rendering multiple QTextLayout characters
///
/// Note: this is very Edbee specific. Every TextLayout has got a single line!
class TextLayout
{
public:
    TextLayout(TextDocument* document);
    virtual ~TextLayout();

    void setCacheEnabled(bool enable);
    QTextLayout* qTextLayout() const;
    QRectF boundingRect() const;


    void buildLayout();

    size_t toVirtualCursorPosition(size_t cursor) const;
    size_t fromVirtualCursorPosition(size_t cursor) const;


    void draw(QPainter *p, const QPointF &pos, const QVector<QTextLayout::FormatRange> &selections = QVector<QTextLayout::FormatRange>(), const QRectF &clip = QRectF()) const;
    void drawCursor(QPainter *painter, const QPointF &position, size_t cursorPosition, int width) const;

    void setFormats(const QVector<QTextLayout::FormatRange> &formats);
    void setText(const QString &string);

    void useSingleCharRanges();
    TextRangeSet* singleCharRanges() const;
    void addSingleCharRange(size_t index, size_t length);


    /// These calculations manipulate the cursor position. ..
    /// We assume the SOURCE character is 1... (TODO: store the orginal number of replace characters)
    qreal cursorToX(size_t cursorPos, QTextLine::Edge edge = QTextLine::Leading) const;
    size_t xToCursor(qreal x, QTextLine::CursorPosition cpos = QTextLine::CursorBetweenCharacters) const;

protected:
    QTextLayout *qtextLayout_;
    TextDocument *textDocumentRef_;
    TextRangeSet *singleCharRanges_; ///< A list textRanges_ used by TextLayout. Every range in this list is treatet as a single character for cusor-movement etc
    QTextLine qtextLine_;
};


//=================================================


class TextLayoutBuilder
{
public:
    TextLayoutBuilder(TextLayout* textLayout, QString & baseString, QVector<QTextLayout::FormatRange> & baseFormatRanges);
    void replace(size_t index, size_t length, const QString replacement, QTextCharFormat format);

protected:
    TextLayout* textLayoutRef_;                                ///< A reference to the textlayout
    QString & baseString_;                                     ///< A reference to the base-string. (This string is modified!)
    QVector<QTextLayout::FormatRange> & baseFormatRanges_;     ///< A reference to the baseFormat range (This list is modified!)
};

} // edbee
