#include "textlayout.h"

#include <QTextLayout>

namespace edbee {

TextLayout::TextLayout()
    : qtextLayout_( new QTextLayout())
{
}

TextLayout::~TextLayout()
{
    delete qtextLayout_;
}

QTextLayout *TextLayout::qTextLayout() const
{
    return qtextLayout_;
}

QRectF TextLayout::boundingRect() const
{
    return qTextLayout()->boundingRect();
}

} // edbee
