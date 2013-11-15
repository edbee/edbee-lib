/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "backspacecommand.h"

#include "edbee/models/changes/complextextchange.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/texteditorconfig.h"
#include "edbee/models/textrange.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/views/textselection.h"
#include "edbee/util/util.h"

#include "debug.h"


namespace edbee {


/// The backspace command constructor
BackspaceCommand::BackspaceCommand( int deleteMode )
    : deleteMode_( deleteMode )
{
}


/// Performs a smart backspace by adjusting the textrange so the backspace leads to the start of a column
/// @param controller to perform the smartbackspace for
/// @param caret the current caret position
/// @return the new caret position
int BackspaceCommand::smartBackspace(TextDocument* doc, int caret )
{
    TextBuffer* buf = doc->buffer();
    TextEditorConfig* config = doc->config();

    // find the line column position
    int line = doc->lineFromOffset( caret );
    int lineStartOffset = doc->offsetFromLine(line);
    int lineEndOffset = qMin( caret,doc->offsetFromLine(line+1)-1 );

    // searches for the start of the current line
    int firstNoneWhitespaceCharPos = buf->findCharPosWithinRangeOrClamp( lineStartOffset, 1, config->whitespaceWithoutNewline(), false, lineStartOffset, lineEndOffset );

    // only when the caret if before a characer
    if( caret <= firstNoneWhitespaceCharPos && lineStartOffset < firstNoneWhitespaceCharPos ) {

        // retrieve the whitespace-start-part of the line
        QString linePrefix = doc->textPart( lineStartOffset, firstNoneWhitespaceCharPos-lineStartOffset );
        QList<int> lineColumnOffsets = Util().tabColumnOffsets( linePrefix, config->indentSize() );


        // when we're exactly at a columnOffset, we need to get the previous
        int lastColumnOffset = lineColumnOffsets.last() + lineStartOffset;
        if( lastColumnOffset == caret) {
            lastColumnOffset = lineStartOffset + ( lineColumnOffsets.size() > 1 ?  lineColumnOffsets.at( lineColumnOffsets.size()-2 ): 0 );
        }

        // we need to got the given number of characters
        return qMax( 0, caret - ( caret - lastColumnOffset ) );

    }
    return qMax( 0, caret-1 );
}


/// Delete characters to the left
/// @param controller the controller
/// @param ranges the ranges to delete
void BackspaceCommand::rangesForDeleteCharLeft(TextEditorController* controller, TextRangeSet* ranges)
{
    // there's already a selection, just delete that one
    if( ranges->hasSelection() ) { return; }

    // when there isn't a selection we need to 'smart-move' the caret

    // when a tab char is used the operation is pretty simple, just delete the character on the left
    if( controller->textDocument()->config()->useTabChar() ) {
        ranges->moveCarets(-1);

    // in the case of spaces, we need to some smart stuff :)
    } else {
        for( int i=0,cnt=ranges->rangeCount(); i<cnt; ++i ) {
            TextRange& range = ranges->range(i);
            range.minVar() = smartBackspace( controller->textDocument(), range.min() );
        }
    }
}


/// Delets the word to the left
/// @param controller the controller
/// @param ranges the textranges to delete
void BackspaceCommand::rangesForDeleteWordLeft(TextEditorController* controller, TextRangeSet* ranges)
{
    // there's already a selection, just delete that one
    if( ranges->hasSelection() ) { return; }

    TextEditorConfig* config = controller->textDocument()->config();
    ranges->moveCaretsByCharGroup( -1, config->whitespaceWithoutNewline(), config->charGroups() );
}


/// Deletes the line to the left
/// @param controller the controller to remove the data for
/// @param ranges the textranges that are used for deletion
void BackspaceCommand::rangesForDeleteLineLeft(TextEditorController* controller, TextRangeSet* ranges)
{
    ranges->moveCaretsToLineBoundary( -1, controller->textDocument()->config()->whitespaceWithoutNewline() );
}


/// preforms the backspace command
/// @param controller the controller to use for the backspace
void BackspaceCommand::execute(TextEditorController* controller)
{
    TextSelection* sel = controller->textSelection();
    TextRangeSet* ranges = new TextRangeSet( *sel );

    // depending on the delete mode we need to expand the selection
    switch( deleteMode_ ) {
        case DeleteCharLeft:
            rangesForDeleteCharLeft( controller, ranges );
            break;
        case DeleteWordLeft:
            rangesForDeleteWordLeft( controller, ranges );
            break;
        case DeleteLineLeft:
            rangesForDeleteLineLeft( controller, ranges );
            break;
    }


    // when there still isn't a selection, simply delete/ignore this command
    if( !ranges->hasSelection() ) {
        delete ranges;
        return;
    }

    // use the simple replacerangeset function
    controller->beginUndoGroup( new ComplexTextChange( controller ) );
    controller->replaceRangeSet( *ranges, "", CoalesceId_Backspace );
    controller->changeAndGiveTextSelection( ranges );
    controller->endUndoGroup( CoalesceId_Backspace, true );
}


/// Converts the command to a string
QString BackspaceCommand::toString()
{
    QString mode;
    switch( deleteMode_ ) {
        case DeleteCharLeft: mode = "DeleteCharLeft"; break;
        case DeleteWordLeft: mode = "DeleteWordLeft"; break;
        case DeleteLineLeft: mode = "DeleteLineLeft"; break;
        default: mode = "Unkown!";
    }
    return QString("BackspaceCommand(%1)").arg(mode);
}


} // edbee
