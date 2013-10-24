/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "duplicatecommand.h"

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
    QStringList newTexts;
    TextDocument* doc = controller->textDocument();

    // iterate over all range and build the new texts to insert
    for( int i=0, cnt=newRanges.rangeCount(); i<cnt; ++i ) {
        TextRange& range = newRanges.range(i);

        // when the range is empty we need to use the complete line line (inclusive the newline)
        if( range.isEmpty() ) {
            // get the complete line
            int line = doc->lineFromOffset( range.caret() );
            range.setCaret( doc->offsetFromLine(line) );
            range.setAnchor( range.caret() );
            newTexts.append( doc->textPart( range.caret(), doc->lineLength(line) ) );

        } else {

            // append the new text, and change the insert position to the place before the caret :)
            newTexts.append( doc->textPart( range.min(), range.length() ) );
            range.maxVar() = range.min();

        }
    }

    // insert the selected text before the carets
    controller->replaceRangeSet( newRanges, newTexts );
}


/// Returns the textual representation of this command
QString DuplicateCommand::toString()
{
    return "DuplicateCommand";
}


} // edbee
