/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "edbee/texteditorcommand.h"

namespace edbee {

class TextDocument;

/// A backspace command. The behaviour of the backspace key depends
/// on several factors:
///
/// When the editor has enabled useTabChar_is enabled, backspace is very simple, it simply
/// deletes the previous character or the selection
///
/// When spaces are used for tabs the behaviour is different. If the caret is left of
/// the first non-space character, it will move 1 column to the left, depending on the tabsize
class BackspaceCommand : public TextEditorCommand
{
public:
    BackspaceCommand();
    int smartBackspace( TextDocument* doc, int caret );
    virtual void execute( TextEditorController* controller );
    virtual QString toString();
};


} // edbee
