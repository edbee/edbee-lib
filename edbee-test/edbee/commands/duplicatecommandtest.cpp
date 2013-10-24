/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "duplicatecommandtest.h"

#include "edbee/models/textdocument.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/texteditorwidget.h"
#include "edbee/views/textselection.h"

#include "debug.h"

namespace edbee {


/// tests the line duplication
void DuplicateCommandTest::testLastLineDuplication()
{
    // get all the required objects
    TextEditorWidget* widget = new TextEditorWidget();
    TextEditorController* ctrl = widget->controller();
    TextDocument* doc = widget->textDocument();
    TextSelection* sel = widget->textSelection();

    // construct the initial situation
    doc->setText("duplication test");
    sel->setRange(1,1);

    // test the situation
    testEqual( doc->text(),"duplication test" );
    testEqual( sel->rangesAsString(), "1>1" );

    // execute the duplication command
    ctrl->executeCommand("duplicate");
    testEqual( doc->text(), "duplication test\nduplication test" );
    testEqual( sel->rangesAsString(), "18>18" );


    delete widget;
}


} // edbee
