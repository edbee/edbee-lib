// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textlayout.h"

#include <QTextLayout>

#include "edbee/debug.h"

namespace edbee {

TextLayout::TextLayout(TextDocument* document)
    : qtextLayout_(new QTextLayout())
    , textDocumentRef_(document)
    , singleCharRanges_(nullptr)
{
}


TextLayout::~TextLayout()
{
    delete singleCharRanges_;
    delete qtextLayout_;
}


void TextLayout::setCacheEnabled(bool enable)
{
    qtextLayout_->setCacheEnabled(enable);
}


QTextLayout *TextLayout::qTextLayout() const
{
    return qtextLayout_;
}


QRectF TextLayout::boundingRect() const
{
    return qtextLayout_->boundingRect();
}


void TextLayout::buildLayout()
{
    qtextLayout_->beginLayout();
    qtextLine_ = qtextLayout_->createLine();
    qtextLayout_->endLayout();
}


/// Converts the document cursorPosition to a virtual cursorposition
size_t TextLayout::toVirtualCursorPosition(size_t cursorPos) const
{
    size_t delta = 0;
    // convert cursor to a valid location
    TextRangeSet* ranges = singleCharRanges();
    if (ranges) {
        for (size_t i = 0, cnt = ranges->rangeCount(); i < cnt; i++) {
            TextRange& range = ranges->range(i);
            if (cursorPos + delta > range.min()) {
                delta += range.length() - 1;
            }
        }
    }
    return cursorPos + delta;
}


/// Converts the virtual cursorPosition to a docuemnt cursorposition
size_t TextLayout::fromVirtualCursorPosition(size_t cursor) const
{
    // when the cursor falls in a single-character range.
    // Set the cursor to the start of this range
    size_t delta = 0;

    TextRangeSet* ranges = singleCharRanges();
    if (ranges) {
        for (size_t i=0, cnt = ranges->rangeCount(); i < cnt; i++) {
            TextRange& range = ranges->range(i);
            if (range.min() <= cursor && cursor < range.max()) {
               delta += cursor - range.min();
            } else if (range.max() <= cursor) {
               delta += range.length() - 1;
            }
        }
    }
    return cursor - delta;
}


void TextLayout::draw(QPainter *p, const QPointF &pos, const QVector<QTextLayout::FormatRange> &selections, const QRectF &clip) const
{
    qtextLayout_->draw(p, pos, selections, clip);
}


void TextLayout::drawCursor(QPainter *painter, const QPointF &position, size_t cursorPosition, int width) const
{
    size_t virtualCursorPosition = toVirtualCursorPosition(cursorPosition);
    qtextLayout_->drawCursor(painter, position, static_cast<int>(virtualCursorPosition), width);
}


void TextLayout::setFormats(const QVector<QTextLayout::FormatRange> &formats)
{
    qtextLayout_->setFormats(formats);
}


void TextLayout::setText(const QString &string)
{
    qtextLayout_->setText(string);
}


void TextLayout::useSingleCharRanges()
{
    if(!singleCharRanges_) {
        singleCharRanges_ = new TextRangeSet(textDocumentRef_);
    }
}


TextRangeSet *TextLayout::singleCharRanges() const
{
    return singleCharRanges_;
}


void TextLayout::addSingleCharRange(size_t index, size_t length)
{
    useSingleCharRanges();
    singleCharRanges()->addRange(index, index+length);
}



qreal TextLayout::cursorToX(size_t cursorPos, QTextLine::Edge edge) const
{
    size_t virtualCursorPos = toVirtualCursorPosition(cursorPos);
    qreal x =  qtextLine_.cursorToX(static_cast<int>(virtualCursorPos), edge);
    return x;
}


size_t TextLayout::xToCursor(qreal x, QTextLine::CursorPosition cpos) const
{
    ptrdiff_t virtualCursor = qtextLine_.xToCursor(x, cpos);
    Q_ASSERT(virtualCursor >= 0);
    return fromVirtualCursorPosition(static_cast<size_t>(virtualCursor));
}


//=================================================


TextLayoutBuilder::TextLayoutBuilder(TextLayout *textLayout, QString & baseString, QVector<QTextLayout::FormatRange> & baseFormatRanges)
    : textLayoutRef_(textLayout)
    , baseString_(baseString)
    , baseFormatRanges_(baseFormatRanges)
{
}


void TextLayoutBuilder::replace(size_t index, size_t length, const QString replacement, QTextCharFormat format)
{
    baseString_.replace(static_cast<qsizetype>(index), static_cast<qsizetype>(length), replacement);
    textLayoutRef_->addSingleCharRange(index, static_cast<size_t>(replacement.length()));  /// TODO: Should we store the original length!?!?!?

    // change existing format ranges:
    ptrdiff_t delta = replacement.length() - static_cast<ptrdiff_t>(length);
    if (delta != 0) {
        for(qsizetype i=0, cnt = baseFormatRanges_.length(); i < cnt; i++ ) {
            QTextLayout::FormatRange& formatRange = baseFormatRanges_[i];
            if (formatRange.start >= static_cast<int>(index)) {
                formatRange.start += delta;
            }
        }
    }

    // append the text format
    QTextLayout::FormatRange formatRange;
    formatRange.format = format;
    formatRange.start = static_cast<int>(index);
    formatRange.length = static_cast<int>(replacement.length());
    baseFormatRanges_.append(formatRange);
}

} // edbee


