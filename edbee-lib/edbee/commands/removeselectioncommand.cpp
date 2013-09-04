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

RemoveSelectionCommand::RemoveSelectionCommand(int direction)
    : direction_(direction)
{
}

/// This method should return the command identifier
int RemoveSelectionCommand::commandId()
{
    return direction_ < 0 ? CoalesceId_Backspace : CoalesceId_Delete;
}

void RemoveSelectionCommand::execute( TextEditorController* controller )
{
    TextSelection* sel = controller->textSelection();

    controller->beginUndoGroup( new ComplexTextChange( controller ) );

    // when there's no selection, make a 'virtual' selection
    TextRangeSet* ranges = 0;
    if( !sel->hasSelection() ) {
        ranges = new TextRangeSet( *sel );
        ranges->moveCarets(direction_);

        // no change required?
        if( !ranges->hasSelection() ) {
            delete ranges;
            return; // nothing to do
        }
        controller->changeAndGiveTextSelection(ranges);
    }
    // replace the selection
    controller->replaceSelection( "", commandId() );

    controller->endUndoGroup( commandId(), true );

    /*
    controller->replaceRangeSet( ranges ? *ranges : *sel, "", commandId(), true );
    delete ranges;
    */
}

QString RemoveSelectionCommand::toString()
{
    return QString("RemoveSelectionCommand(%1)").arg(direction_);
}


} // edbee
