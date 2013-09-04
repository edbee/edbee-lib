/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "copycommand.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include "edbee/texteditorcontroller.h"
#include "edbee/views/textselection.h"

#include "debug.h"


namespace edbee {

const QString CopyCommand::VARBIT_TEXT_TYPE("application/vnd.blommersit.text-type");


CopyCommand::CopyCommand()
{
}

/// Copies the current selection to the clipboard
void CopyCommand::execute(TextEditorController* controller)
{
    QClipboard* clipboard = QApplication::clipboard();
    TextSelection* sel = controller->textSelection();

    // get the selected text
    QString str = sel->getSelectedText();
    if( !str.isEmpty() ) {
        clipboard->setText( str );

    // perform a full-lines copy.. The full line of the caret's position is copied
    } else {

        str = sel->getSelectedTextExpandedToFullLines();
        QMimeData* mimeData = new QMimeData();
        mimeData->setText( str );
        mimeData->setData( VARBIT_TEXT_TYPE, "line" );
        clipboard->setMimeData( mimeData );
        delete mimeData;
    }
}

QString CopyCommand::toString()
{
    return "CopyCommand";
}

} // edbee
