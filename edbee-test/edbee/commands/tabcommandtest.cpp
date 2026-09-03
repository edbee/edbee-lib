// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "tabcommandtest.h"

#include "edbee/models/textdocument.h"
#include "edbee/models/texteditorconfig.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/texteditorwidget.h"
#include "edbee/views/textselection.h"

#include "edbee/debug.h"

namespace edbee {


TabCommandTest::TabCommandTest()
    : widget_(0)
{
}


/// Initialization for every test case
void TabCommandTest::init()
{
    widget_ = new TextEditorWidget();
    doc()->config()->setIndentSize(2);
    doc()->config()->setUseTabChar(false);
}


/// cleanup the testcase
void TabCommandTest::clean()
{
    delete widget_;
}


/// a tab in an empty document has no character in front of the caret to look at
void TabCommandTest::testTabInEmptyDocument()
{
    doc()->setText("");
    sel()->setRange(0, 0);

    ctrl()->executeCommand("tab");
    testEqual( doc()->text(), "  " );
    testEqual( sel()->rangesAsString(), "2>2" );
}


/// the caret at the very first offset of the document has no character in front of it either
void TabCommandTest::testTabAtStartOfDocument()
{
    doc()->setText("abc");
    sel()->setRange(0, 0);

    ctrl()->executeCommand("tab");
    testEqual( doc()->text(), "  abc" );
}


/// tabbing within the leading whitespace inserts up to the next tabstop
void TabCommandTest::testTabAfterIndentation()
{
    doc()->setText("  abc");
    sel()->setRange(2, 2);

    ctrl()->executeCommand("tab");
    testEqual( doc()->text(), "    abc" );
}


/// a backtab in an empty document has nothing to outdent
void TabCommandTest::testTabBackInEmptyDocument()
{
    doc()->setText("");
    sel()->setRange(0, 0);

    ctrl()->executeCommand("tab_back");
    testEqual( doc()->text(), "" );
}


/// Returns the widget
TextEditorWidget* TabCommandTest::widget() const
{
    return widget_;
}


/// returns the controller
TextEditorController* TabCommandTest::ctrl() const
{
    return widget_->controller();
}


/// returns the document
TextDocument* TabCommandTest::doc() const
{
    return widget_->textDocument();
}


/// returns the text selection
TextSelection* TabCommandTest::sel() const
{
    return widget_->textSelection();
}


} // edbee
