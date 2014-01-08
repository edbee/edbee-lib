/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "commentcommand.h"

#include "edbee/models/dynamicvariables.h"
#include "edbee/models/texteditorconfig.h"
#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textdocumentscopes.h"
#include "edbee/views/textselection.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/util/rangesetlineiterator.h"
#include "edbee/util/regexp.h"

#include "debug.h"

namespace edbee {

// TODO, we also need to support, block-comments


/// this method checks if the selection needs to be added
static bool areAllLinesCommented( TextEditorController* controller, RegExp& commentStart )
{
    TextDocument* doc = controller->textDocument();
    TextBuffer* buf = doc->buffer();

    // iterate over all lines
    RangeSetLineIterator itr( controller->textSelection() );
    while( itr.hasNext() ) {
        int line = itr.next();

        // directly search in the raw data pointer buffer to prevent QString creation
        int offset = doc->offsetFromLine(line);

        /// when there's no comment found at this line return false
        if( commentStart.indexIn( buf->rawDataPointer(), offset, offset+doc->lineLength(line)-1 ) < 0 ) {
            return false;
        }

    }
    return true;
}


/// removes all comments
/// @param controller the controller to operate on
/// @param commentStart the start of the comment
static void removeLineComment( TextEditorController* controller, const QString& commentStart  )
{
    RegExp regExp( QString("^\\s*(%1\\s?)").arg( RegExp::escape(commentStart.trimmed() ) ) );

    TextDocument* doc = controller->textDocument();
    TextBuffer* buf = doc->buffer();

    // iterate over all lines and build all ranges
    TextRangeSet ranges(doc);
    TextRangeSet* newSelection = new TextRangeSet(controller->textSelection());

    RangeSetLineIterator itr( controller->textSelection() );
    while( itr.hasNext() ) {

        // directly search in the raw data pointer buffer to prevent QString creation
        int line = itr.next();
        int offset = doc->offsetFromLine(line);

        // when there's no comment found at this line return false
        if( regExp.indexIn( buf->rawDataPointer(), offset, offset + doc->lineLength(line) - 1 ) >= 0 ) {
            ranges.addRange( regExp.pos(1), regExp.pos(1) + regExp.len(1) );
            newSelection->changeSpatial( regExp.pos(1), regExp.len(1), 0 );
        }
    }

    // shouldn't be empty, but just in case
    if( ranges.rangeCount() > 0 ) {
        doc->beginChanges( controller );
        doc->replaceRangeSet( ranges, "" );
        doc->giveSelection( controller, newSelection );
        doc->endChanges(0);
    } else {
        delete newSelection;
    }
}


/// Adds comments on all line starts
static void insertLineComments( TextEditorController* controller, const QString& str )
{
    TextDocument* doc = controller->textDocument();
    TextBuffer* buf = doc->buffer();

    // iterate over all lines and build all ranges
    TextRangeSet ranges(doc);
    TextRangeSet* newSelection = new TextRangeSet(controller->textSelection());

    // iterate over all lines
    RangeSetLineIterator itr( controller->textSelection() );
    while( itr.hasNext() ) {

        // directly search in the raw data pointer buffer to prevent QString creation
        int line = itr.next();
        int offset = doc->offsetFromLine(line);

        // when there's no comment found at this line return false
        int wordStart = buf->findCharPosWithinRangeOrClamp( offset, 1, doc->config()->whitespaces(), false, offset, offset + doc->lineLength(line)-1);
        ranges.addRange(wordStart,wordStart);
        newSelection->changeSpatial( wordStart, 0, str.length() );
    }

    // shouldn't be empty, but just in case
    if( ranges.rangeCount() > 0 ) {
        doc->beginChanges( controller );
        doc->replaceRangeSet( ranges, str );
        doc->giveSelection( controller, newSelection );
        doc->endChanges(0);
    } else {
        delete newSelection;
    }
}


/// Executes the given comment
/// @param controller the controller this command is executed for
void CommentCommand::execute(TextEditorController* controller)
{

// TODO: We need to determine the scope per RANGE !!!!!!!!!!!!!
// Currently we simply take the first one
    TextDocument* doc = controller->textDocument();
    TextDocumentScopes* scopes = doc->scopes();
    TextScopeList list = scopes->scopesAtOffset( controller->textSelection()->range(0).min() );

    // get the comment
    QString commentStart = controller->dynamicVariables()->value("TM_COMMENT_START", &list ).toString();
    if( commentStart.isEmpty() ) {
        return;
    }
    RegExp commentStartRegExp( QString("^\\s*%1").arg( RegExp::escape(commentStart.trimmed() ) ) );

    // Iterate over the lines (TODO, implement this)
    bool removeComments = areAllLinesCommented( controller, commentStartRegExp );

    // we to remove all comments
    if( removeComments ) {
        removeLineComment( controller, commentStart);

    // add comments
    } else {
        insertLineComments( controller, commentStart );

    }
}


/// converts this command to a string
QString CommentCommand::toString()
{
    return "CommentCommand";
}



} // edbee
