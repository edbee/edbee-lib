/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "edbee/texteditorcommand.h"

namespace edbee {

class TextEditorController;

class PasteCommand : public TextEditorCommand
{
public:
    PasteCommand();

    /// This method should return the command identifier
    virtual int commandId() { return CoalesceId_Paste; }

    virtual void execute( TextEditorController* controller );
    virtual QString toString();
};

} // edbee
