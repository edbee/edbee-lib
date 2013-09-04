/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "texteditorconfig.h"

#include "debug.h"

namespace edbee {

TextEditorConfig::TextEditorConfig()
    : caretWidth_(2)
    , caretBlinkingRate_(700)   // 0 means no blinking (default = 700)
    , indentSize_(4)
    , useTabChar_(true)
    , charGroups_()
    , whitespaces_("\n\t ")
    , whitespaceWithoutNewline_("\t ")
    , extraLineSpacing_(0)
    , useLineSeparator_(false)
    , lineSeparatorPen_( QColor(230,230,230), 1.5, Qt::DashLine )
    , undoGroupPerSpace_(true)
    , showCaretOffset_(true)
    , themeName_("Monokai")
{
    charGroups_.append( QString("./\\()\"'-:,.;<>~!@#$%^&*|+=[]{}`~?"));
}


} // edbee
