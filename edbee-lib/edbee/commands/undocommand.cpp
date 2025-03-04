// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "undocommand.h"

#include "edbee/texteditorcontroller.h"

#include "edbee/debug.h"

namespace edbee {

UndoCommand::UndoCommand(bool soft)
    : soft_(soft)
{
}

void UndoCommand::execute( TextEditorController* controller)
{
    controller->undo(soft_);
}

QString UndoCommand::toString()
{
    return QStringLiteral("UndoCommand(%1)").arg( soft_ ? "soft" : "hard" );
}

bool UndoCommand::readonly()
{
    return soft_;
}


} // edbee
