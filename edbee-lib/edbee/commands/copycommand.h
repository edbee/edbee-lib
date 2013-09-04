/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QString>

#include "edbee/texteditorcommand.h"

namespace edbee {

class TextEditorController;

class CopyCommand : public TextEditorCommand
{
public:
    static const QString VARBIT_TEXT_TYPE;

public:
    CopyCommand();

    /// This method should return the command identifier
    virtual int commandId() { return CoalesceId_None; }


    virtual void execute( TextEditorController* controller );
    virtual QString toString();
};

} // edbee
