/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "commentcommand.h"

#include "edbee/edbee.h"
#include "edbee/models/dynamicvariables.h"
#include "edbee/models/texteditorconfig.h"
#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textdocumentscopes.h"
#include "edbee/views/textselection.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/util/rangelineiterator.h"
#include "edbee/util/regexp.h"

#include "debug.h"

namespace edbee {

// TODO, we also need to support, block-comments
// TODO, we need to rewrite this to make it better testable


/// This method checks if all lines are comment
/// @param doc the document to check
/// @param range the range for checking all commented lines
/// @param commentStart the text used as the line comment prefix
/// @return true if all lines are commented
static bool areAllLinesCommented( TextDocument* doc, TextRange& range, const QString& commentStart )
{
    RegExp commentStartRegExp( QString("^\\s*%1").arg( RegExp::escape(commentStart.trimmed() ) ) );

    // we directly check in the buffer for fast regexp without QString creation
    TextBuffer* buf = doc->buffer();

    // iterate over all lines
    RangeLineIterator itr( doc, range );
    while( itr.hasNext() ) {

        int line = itr.next();
        int lineLength = doc->lineLengthWithoutNewline(line);

        // when it's the last line and its blank, we must skip it
        if( !itr.hasNext() && lineLength == 0 ) {
            continue;
        }

        // directly search in the raw data pointer buffer to prevent QString creation
        int offset = doc->offsetFromLine(line);

        /// when there's no comment found at this line return false
        if( commentStartRegExp.indexIn( buf->rawDataPointer(), offset, offset+doc->lineLength(line)-1 ) < 0 ) {
            return false;
        }
    }
    return true;
}


/// removes all line comments from the iven line
/// @param doc the document to change
/// @param range the range to remove the comments
/// @param commentStart the text used as the line comment prefix
static void removeLineComment( TextDocument* doc, TextRange& range, const QString& commentStart  )
{
    RegExp regExp( QString("^[^\\S\n]*(%1[^\\S\n]?)").arg( RegExp::escape(commentStart.trimmed() ) ) );
    TextBuffer* buf = doc->buffer();

    // iterate over all lines and build all ranges
    RangeLineIterator itr( doc, range );
    while( itr.hasNext() ) {

        // directly search in the raw data pointer buffer to prevent QString creation
        int line = itr.next();
        int offset = doc->offsetFromLine(line);

        // perform a regexp to extract the comment that needs to be removed
        if( regExp.indexIn( buf->rawDataPointer(), offset, offset + doc->lineLength(line) ) >= 0 ) {
            doc->replace( regExp.pos(1), regExp.len(1), "" );
        }
    }

}


/// Adds comments on all line starts
/// @param doc the document to insert the comment
/// @param range the textrange to insert comments
/// @param str the comment prefix
static void insertLineComments( TextDocument* doc, TextRange& range, const QString& str )
{
    TextBuffer* buf = doc->buffer();

    // iterate over all lines and build all ranges
    RangeLineIterator itr( doc, range );
    while( itr.hasNext() ) {

        // directly search in the raw data pointer buffer to prevent QString creation
        int line = itr.next();
        int offset = doc->offsetFromLine(line);
        int lineLength = doc->lineLengthWithoutNewline(line);

        // when it's the last line and its blank, we must skip it
        if( !itr.hasNext() && lineLength == 0 ) {
            continue;
        }

        // when there's no comment found at this line return false
        int wordStart = buf->findCharPosWithinRangeOrClamp( offset, 1, doc->config()->whitespaces(), false, offset, offset + lineLength );
        doc->replace(wordStart,0,str);

    }
}


/// Removes a block comment if possible
/// @param controller the controller to perform the operation on
/// @param range the current textrange
/// @param commentStart the start of the commment
/// @param commentEnd the end of the comment
/// @return this method returns true if the block comment was remoed
static bool removeBlockComment( TextEditorController* controller, TextRange& range, QString& commentStart, QString& commentEnd )
{
    TextDocument* doc = controller->textDocument();
    TextDocumentScopes* scopes = doc->scopes();

    // it seems that every language in sublime/textmate use the comment.block convention
    TextScope* blockCommentScope = Edbee::instance()->scopeManager()->refTextScope("comment.block");

    // we only fetch multi-line scoped textranges
    int middleRange= range.min()+(range.max()-range.min())/2;

    QVector<ScopedTextRange*> scopedRanges = scopes->createScopedRangesAtOffsetList(middleRange);
    foreach( ScopedTextRange* scopedRange, scopedRanges ) {
        TextScope* scope = scopedRange->scope();

        // did we found a block comment
        if( scope->startsWith( blockCommentScope ) ){
            int min = scopedRange->min();
            int max = scopedRange->max();

            // when the scope starts and ends with the comment start and end remove it
            if( doc->textPart(min, commentStart.length() ) == commentStart  &&
                doc->textPart(max - commentEnd.length(), commentEnd.length() ) == commentEnd ) {

                // * warning! * we cannot use the scoped rangeset directly
                // when we replace a text, the scoped rangeset could be invalidated and destroyed!

                // next remove the range
                doc->replace( min, commentStart.length(), "" );
                doc->replace( max-commentEnd.length() - commentStart.length(), commentEnd.length(), "" );

                qDeleteAll(scopedRanges);
                return true;
            }
        }
    }
    qDeleteAll(scopedRanges);
    return false;
}


/// Insert a block comment
/// Adds a comment start and end around the current text range.
/// If the textrange is empty, it first is expanded to a full line
/// @param controller the controller to insert a comment for
/// @param range the range to insert a block comment for
/// @param commentStart the start of the comment
/// @param commentEnd the end of the comment
/// @return the last affected range
static int insertBlockComment( TextEditorController* controller, TextRange& range, QString& commentStart, QString& commentEnd )
{
    TextDocument* doc = controller->textDocument();

    // for safety I don't use the range anymore. It's adjusted automaticly, but could in theory vanish
    int min = range.min();
    int max = range.max();

    // when the range is 'blank' expand it to a full line
    if( range.isEmpty() ) {
        int line = doc->lineFromOffset(min);
        min = doc->offsetFromLine(line);
        max = min + doc->lineLengthWithoutNewline(line);
//        range.expandToFullLine(doc,0);
//        range.deselectTrailingNewLine(doc);
    }


    doc->replace(min,0, commentStart);
    doc->replace(max+commentStart.length(),0, commentEnd);

    return max + commentStart.length() + commentEnd.length();
}



/// retrieve the comment start and comment for the given range
/// @return false if no 'block' comment-structure was found
static bool getCommandStartAndEnd( TextEditorController* controller, TextRange& range, QString& commentStart, QString& commentEnd  )
{
    TextDocument* doc = controller->textDocument();
    TextDocumentScopes* scopes = doc->scopes();

    // retrieve the starting scope
    TextScopeList startScopeList = scopes->scopesAtOffset( range.min() );

    // retrieve the comment start en end
    commentStart = controller->dynamicVariables()->value("TM_COMMENT_START", &startScopeList ).toString();
    commentEnd = controller->dynamicVariables()->value("TM_COMMENT_END", &startScopeList ).toString();

    // return true if the comment isn't empty
    return !commentStart.isEmpty();
}


/// Comments the given range
/// @param controller the controller that conatins the controller
/// @param range the range to comment
/// @return the last affected offset
static int commentRange( TextEditorController* controller, TextRange& range )
{
    QString commentStart, commentEnd;
    if( !getCommandStartAndEnd( controller, range, commentStart, commentEnd ) ) {
        return range.max();
    }
//qlog_info() << "COMMAND START:" << commentStart;
//qlog_info()<< "COMMAND END:"<< commentEnd;

    // here we need to remove an active block comment, if there is one
    if( removeBlockComment( controller, range, commentStart, commentEnd ) ) {
        return range.max();
    }

    // no end-comment means a line comment
    TextDocument* doc = controller->textDocument();
    if( commentEnd.isEmpty() ) {

        // we need to remove all comments
        if( areAllLinesCommented( doc, range, commentStart ) ) {
            removeLineComment( doc, range, commentStart);

        // add comments
        } else {
            insertLineComments( doc, range, commentStart );
        }

    // else it's a block comment
    } else {
        return insertBlockComment( controller, range, commentStart, commentEnd );
    }
    return range.max();
}


/// Executes the given comment
/// @param controller the controller this command is executed for
void CommentCommand::execute(TextEditorController* controller)
{
    // iterate over all ranges
    TextRangeSet* selection = new DynamicTextRangeSet( controller->textSelection() );
    TextDocument* doc = controller->textDocument();

    // start changing the document
    doc->beginChanges(controller);
    for( int i=0,cnt=selection->rangeCount(); i<cnt; ++i ) {
        TextRange& range = selection->range(i);
        commentRange( controller, range );
    }
    doc->giveSelection( controller, selection );
    doc->endChanges(0);
}


/// converts this command to a string
QString CommentCommand::toString()
{
    return "CommentCommand";
}



} // edbee
