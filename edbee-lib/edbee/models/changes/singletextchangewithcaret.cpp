/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "singletextchangewithcaret.h"

#include "debug.h"

namespace edbee {

SingleTextChangeWithCaret::SingleTextChangeWithCaret(int offset, int length, const QString& text, int caret )
    : SingleTextChange( offset, length, text )
    , caret_( caret )
{
}


} // edbee
