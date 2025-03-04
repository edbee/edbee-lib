// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QString>
#include "edbee/texteditorcommand.h"

namespace edbee {

class EDBEE_EXPORT UndoCommand : public TextEditorCommand
{
public:
    UndoCommand( bool soft= false );

    virtual int commandId() { return CoalesceId_None; }

    virtual void execute( TextEditorController* controller ) override;
    virtual QString toString() override;
    virtual bool readonly() override;

private:
    bool soft_;         ///< should this command perform a soft undo?
};

} // edbee
