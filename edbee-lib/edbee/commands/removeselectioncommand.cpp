/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "removeselectioncommand.h"

#include "edbee/models/changes/complextextchange.h"

#include "edbee/models/textchange.h"
#include "edbee/models/textrange.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/views/textselection.h"

#include "debug.h"

namespace edbee {

/// Constructs the selection command
/// @param direction the direction to remove (-1,1)
RemoveSelectionCommand::RemoveSelectionCommand(int direction)
    : direction_(direction)
{
}


/// This method should return the command identifier
int RemoveSelectionCommand::commandId()
{
    return direction_ < 0 ? CoalesceId_Backspace : CoalesceId_Delete;
}


/// Executes the remove task
/// @param controller the controller
void RemoveSelectionCommand::execute( TextEditorController* controller )
{
    TextSelection* sel = controller->textSelection();
    TextRangeSet* ranges = new TextRangeSet( *sel );
    if( !ranges->hasSelection() ) {
        ranges->moveCarets(direction_);
        if( !ranges->hasSelection() ) {
            delete ranges;
            return;
        }
    }

    controller->beginUndoGroup( new ComplexTextChange( controller ) );

    // use the simple replacerangeset function
    controller->replaceRangeSet( *ranges, "", commandId() );
    controller->changeAndGiveTextSelection( ranges );

    controller->endUndoGroup( commandId(), true );
}


/// Returns the name of the command
QString RemoveSelectionCommand::toString()
{
    return QString("RemoveSelectionCommand(%1)").arg(direction_);
}


} // edbee
