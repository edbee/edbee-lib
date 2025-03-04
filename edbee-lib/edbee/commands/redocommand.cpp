// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "redocommand.h"

#include "edbee/texteditorcontroller.h"

#include "edbee/debug.h"

namespace edbee {

RedoCommand::RedoCommand( bool soft )
    : soft_(soft)
{
}

void RedoCommand::execute( TextEditorController* controller)
{
    controller->redo(soft_);
}

QString RedoCommand::toString()
{
    return QStringLiteral("RedoCommand(%1)").arg( soft_ ? "soft" : "hard" );
}

bool RedoCommand::readonly()
{
    return soft_;
}


} // edbee
