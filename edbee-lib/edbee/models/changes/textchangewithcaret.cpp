// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textchangewithcaret.h"

#include "edbee/debug.h"

namespace edbee {

TextChangeWithCaret::TextChangeWithCaret(int offset, int length, const QString& text, int caret )
    : TextChange( offset, length, text )
    , caret_( caret )
{
}


/// returns the caret position
int TextChangeWithCaret::caret() const
{
    return caret_;
}


/// Sets the caret position
/// @param caret the caret to set
void TextChangeWithCaret::setCaret(int caret)
{
    caret_ = caret;
}


} // edbee
