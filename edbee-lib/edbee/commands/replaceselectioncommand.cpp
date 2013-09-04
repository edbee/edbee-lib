/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "replaceselectioncommand.h"

#include "edbee/texteditorcontroller.h"

#include "debug.h"

namespace edbee {

ReplaceSelectionCommand::ReplaceSelectionCommand( const QString& text, int coalesceId )
    : text_(text)
    , coalesceId_( coalesceId )
{
}

void ReplaceSelectionCommand::execute(TextEditorController* controller)
{
    controller->replaceSelection( text_ , coalesceId_ );
}

QString ReplaceSelectionCommand::toString()
{
    return QString("ReplaceSelectionCommand");
}


} // edbee
