/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once


#include "edbee/texteditorcommand.h"


namespace edbee {

class TextRange;

/// Inserts a newline.
/// When  smarttab is enabled it also inser leading tabs/spaces
class NewlineCommand : public TextEditorCommand
{
public:
    QString calculateSmartIndent( TextEditorController* controller, TextRange& range );
    virtual void execute( TextEditorController* controller );
    virtual QString toString();
};


} // edbee
