/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "duplicatecommand.h"

#include "edbee/models/changes/complextextchange.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textrange.h"
#include "edbee/views/textselection.h"
#include "edbee/texteditorcontroller.h"

#include "debug.h"

namespace edbee {


/// constructs the duplication command
DuplicateCommand::DuplicateCommand()
{
}


/// Executes the duplication command
/// @param controller the active controller
void DuplicateCommand::execute(TextEditorController* controller)
{
    // create a new range set and the new texts
    TextRangeSet newRanges( controller->textSelection() );
    TextRangeSet* newSelection = new TextRangeSet( controller->textSelection() );
    QStringList newTexts;
    TextDocument* doc = controller->textDocument();

    int coalesceId = CoalesceId_Duplicate;
//    int lastFullLine = -1;

    // iterate over all range and build the new texts to insert
    int delta = 0;
    for( int i=0, cnt=newRanges.rangeCount(); i<cnt; ++i ) {
        TextRange& range = newRanges.range(i);

        // when the range is empty we need to use the complete line line (inclusive the newline)
        if( range.isEmpty() ) {

            // get the complete line
            int line = doc->lineFromOffset( range.caret() );
            range.setCaret( doc->offsetFromLine(line) );
            range.setAnchor( range.caret() );
            newTexts.append( QString("%1\n").arg(doc->lineWithoutNewline(line)) );


// FIXME: (#73) This is a hack!! Currenlty duplicating multiple lines with multiple carets on the same line
//        exposes a bug int the coalescing system. The current coalescing system requires an equal number of textrange
//        of course this shouldn't be required
//if( lastFullLine  == line ) {
//    coalesceId = 0;
//}
//lastFullLine  = line;



        } else {

            // append the new text, and change the insert position to the place before the caret :)
            newTexts.append( doc->textPart( range.min(), range.length() ) );
            range.maxVar() = range.min();
        }

        // change the spatial of the new selection
        // insert the spatial at the caret position.
        // we only insert text so the length = 0 and the new length is the length of the inserted text
        int length = newTexts.last().length();
//qlog_info() << i << ">> change spatial: " << newSelection->rangesAsString() << ": " << ( range.caret() + delta ) << ", " << length;
        newSelection->changeSpatial( range.caret() + delta, 0, length );
        delta += length;

//qlog_info() <<  "  =>> change spatial: " << newSelection->rangesAsString();

        // we also msut move the next ranges
//        newRanges.changeSpatial( range.caret(), 0, newTexts.last().size() );
    }
//qlog_info() << "REPLACE RANGESET: " << newRanges.rangesAsString() << " | texts= " << newTexts.join(",");
//qlog_info() << "REPLACE SELECTION: " << newSelection->rangesAsString();


    // changes the ranges
    controller->beginUndoGroup( new ComplexTextChange( controller ) );
    controller->replaceRangeSet( newRanges, newTexts, coalesceId );
    controller->changeAndGiveTextSelection( newSelection );
    controller->endUndoGroup( coalesceId, true ); // commandId(), true );
}


/// Returns the textual representation of this command
QString DuplicateCommand::toString()
{
    return "DuplicateCommand";
}


} // edbee
