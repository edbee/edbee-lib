/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QString>

namespace edbee {

class TextEditorController;

/// Predefined coalesce/command ids
enum CoalesceId {
    CoalesceId_ForceMerge    = -1,           /// FORCE coalescing
    CoalesceId_None          = 0,            /// NO coalescing
    CoalesceId_Selection     = 1 << 16,      /// 2 bytes for sub-typpes
    CoalesceId_AppendChar    = 2 << 16,
    CoalesceId_Backspace     = 3 << 16,
    CoalesceId_Delete        = 4 << 16,
    CoalesceId_InsertNewLine = 5 << 16,
    CoalesceId_InsertTab     = 6 << 16,
    CoalesceId_CutLine       = 7 << 16,
    CoalesceId_Paste         = 8 << 16,
    CoalesceId_UserDefined   = 9 << 16
};



class TextEditorCommand
{
public:
    TextEditorCommand();
    virtual ~TextEditorCommand() {}

    /// This method should return the command identifier
//    virtual int commandId() = 0;

    /// This method should execute the given command
    virtual void execute( TextEditorController* controller ) = 0;

    /// should return the description of the command
    virtual QString toString() = 0;
};

} // edbee
