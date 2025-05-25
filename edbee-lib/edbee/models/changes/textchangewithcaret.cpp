// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textchangewithcaret.h"

#include "edbee/debug.h"

namespace edbee {

TextChangeWithCaret::TextChangeWithCaret(size_t offset, size_t length, const QString& text, size_t caret)
    : TextChange(offset, length, text)
    , caret_(caret)
{
}


/// returns the caret position
size_t TextChangeWithCaret::caret() const
{
    return caret_;
}


/// Sets the caret position
/// @param caret the caret to set
void TextChangeWithCaret::setCaret(size_t caret)
{
    caret_ = caret;
}


} // edbee
