// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include "edbee/texteditorcommand.h"

namespace edbee {


/// A debug command, for simply suppling/dumping some editor state information to the console
class EDBEE_EXPORT DebugCommand : public TextEditorCommand
{
public:

    enum DebugCommandType {
        DumpScopes,
        RebuildScopes,
        DumpUndoStack,
        DumpCharacterCodes
    };

    DebugCommand( DebugCommandType command );

    virtual void execute(TextEditorController* controller) override;
    virtual QString toString() override;
    virtual bool readonly() override;

protected:

    void dumpScopes( TextEditorController* controller );
    void rebuildScopes( TextEditorController* controller );

    void dumpUndoStack( TextEditorController* controller );
    void dumpCharacterCodes( TextEditorController* controller );
private:

    DebugCommandType command_;      ///< the command to execute

};

} // edbee
