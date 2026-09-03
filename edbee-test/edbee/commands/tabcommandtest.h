// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

class TextEditorWidget;
class TextEditorController;
class TextDocument;
class TextSelection;


/// Tests the tab command
class TabCommandTest : public edbee::test::TestCase
{
    Q_OBJECT
public:
    TabCommandTest();

private slots:

    void init();
    void clean();

    void testTabInEmptyDocument();
    void testTabAtStartOfDocument();
    void testTabAfterIndentation();
    void testTabBackInEmptyDocument();

private:

    TextEditorWidget* widget() const;
    TextEditorController* ctrl() const;
    TextDocument* doc() const;
    TextSelection* sel() const;

    TextEditorWidget* widget_;
};


} // edbee

DECLARE_TEST(edbee::TabCommandTest);
