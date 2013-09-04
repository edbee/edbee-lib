/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "pastecommand.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include "edbee/commands/copycommand.h"
#include "edbee/models/textbuffer.h"
#include "edbee/models/textchange.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textrange.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/views/textselection.h"

#include "debug.h"

namespace edbee {

PasteCommand::PasteCommand()
{
}

void PasteCommand::execute(TextEditorController* controller)
{

    QClipboard* clipboard     = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();
    QString text              = clipboard->text();

    TextDocument* doc    = controller->textDocument();
    TextRangeSet* sel    = controller->textSelection();

    // a line-based paste
    if( mimeData->hasFormat( CopyCommand::VARBIT_TEXT_TYPE ) ) {
        TextRangeSet newRanges(doc);
        for( int i=0,cnt=sel->rangeCount(); i<cnt; ++i ) {
            TextRange& range = sel->range(i);
            int line   = doc->lineFromOffset( range.min() );
            int offset = doc->offsetFromLine(line);
            newRanges.addRange(offset,offset);
        }

        controller->replaceRangeSet( newRanges, text, commandId() );
        return;

    // normal paste
    } else {
        if( !text.isEmpty() ) {
            controller->replaceSelection(text,commandId());
        }
        return;
    }

}

QString PasteCommand::toString()
{
    return "PasteCommand";
}

} // edbee
