/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "newlinecommand.h"

#include "edbee/models/texteditorconfig.h"
#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textrange.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/util/util.h"
#include "edbee/views/textselection.h"

#include "debug.h"


namespace edbee {


/// This method calculates the required 'smart' indent value at the given positon
/// @param controller the controller
/// @param range the range to calculate the smart tab for
QString NewlineCommand::calculateSmartIndent(TextEditorController* controller, TextRange& range)
{
    QString result;

    TextDocument* doc = controller->textDocument();
    TextBuffer* buf = doc->buffer();
    TextEditorConfig* config = doc->config();

    // find the line column position
    int caretOffset = range.min();
    int line = doc->lineFromOffset( range.min() );
    int startOffset = doc->offsetFromLine(line);
    int endOffset = qMin( caretOffset ,doc->offsetFromLine(line+1)-1 );

    // searches for the start of the current line
    int charPos = buf->findCharPosWithinRangeOrClamp( startOffset, 1, config->whitespaceWithoutNewline(), false, startOffset, endOffset );

    // when the start is found
    if( charPos > startOffset ) {

        // we have found the column index. This isn't yet the column,
        // because when using tab-characters the column position can be different.
        // calculate the position by converting the tabs to spaces
        int column = Util().convertTabsToSpaces( doc->textPart( startOffset, charPos-startOffset ), config->indentSize() ).length();

        // when a tab character is used, we need to add the given number of tabs
        if( config->useTabChar() ) {
            int tabs   = column / config->indentSize();
            int spaces = column % config->indentSize();
            result.append( QString("\t").repeated(tabs));
            if( spaces>0 ) {
                result.append( QString(" ").repeated(spaces));
            }

        // else we can add spaces to the given column position
        } else {
            result.append( QString(" ").repeated( column) );
        }
    }
    return result;
}


/// Executes the insert newline command
/// @param controller the controller to execute this command for
void NewlineCommand::execute(TextEditorController* controller)
{
    TextDocument* doc = controller->textDocument();
    TextEditorConfig* config = doc->config();

    // when no smarttab is enabled this function is simple
    // the current selection is simple replaced by a newline :)
    if( !config->smartTab() )
    {
        controller->replaceSelection( "\n", CoalesceId_InsertNewLine );
        return;
    }


    // when we're using smart tab. We're going to indent every next line
    // to the same level as the previous one
    QStringList texts;

    TextSelection* sel = controller->textSelection();
    for( int i=0,cnt=sel->rangeCount(); i<cnt; ++i ) {
        TextRange& range = sel->range(i);

        // add the string to insert
        texts.push_back( QString("\n%1").arg( calculateSmartIndent( controller, range ) ) );
    }

    // next execute the replace command
    controller->replaceSelection( texts, CoalesceId_InsertNewLine );


}


/// Returns the command name
QString NewlineCommand::toString()
{
    return "NewlineCommand";
}


} // edbee
