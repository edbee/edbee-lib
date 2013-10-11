/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "cutcommand.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include "edbee/commands/copycommand.h"

#include "edbee/models/textchange.h"
#include "edbee/models/textrange.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/views/textselection.h"

#include "debug.h"

namespace edbee {

CutCommand::CutCommand()
{
}

void CutCommand::execute(TextEditorController* controller)
{
    QClipboard *clipboard = QApplication::clipboard();
    TextRangeSet* sel = controller->textSelection();

    // get the selected text
    QString str = sel->getSelectedText();
    if( !str.isEmpty() ) {
        clipboard->setText( str );
        controller->replaceSelection( "", 0);
        return;

    // perform a full-lines copy
    } else {
        TextRangeSet newSel( *sel );
        newSel.expandToFullLines(1);
        str = newSel.getSelectedText();

        QMimeData* mimeData = new QMimeData();
        mimeData->setText( str );
        mimeData->setData( CopyCommand::EDBEE_TEXT_TYPE, "line" );
        clipboard->setMimeData( mimeData );
        delete mimeData;

        // remove the selection
        controller->replaceRangeSet( newSel, "", commandId() );
        return;
    }
}

QString CutCommand::toString()
{
    return "CutCommand";
}

} // edbee
