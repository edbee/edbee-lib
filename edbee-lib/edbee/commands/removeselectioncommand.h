/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "edbee/texteditorcommand.h"

namespace edbee {

class TextEditorController;

/// Removes the current selection
class RemoveSelectionCommand : public TextEditorCommand
{
public:
    RemoveSelectionCommand(int direction);
    virtual int commandId();
    virtual void execute( TextEditorController* controller );
    virtual QString toString();

private:
    int direction_;             ///< The direction
};


} // edbee
