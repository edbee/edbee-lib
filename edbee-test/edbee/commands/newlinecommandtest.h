// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

class TextDocument;
class NewlineCommand;
class TextEditorController;
class TextEditorConfig;
class TextEditorWidget;


/// test the newline command
class NewlineCommandTest : public edbee::test::TestCase
{
    Q_OBJECT
public:
    NewlineCommandTest();

private slots:

    void init();
    void clean();
    void testCalculateSmartIndent_useSpaces();
    void testCalculateSmartIndent_useTabs();

private:
    TextDocument* doc();
    TextEditorConfig* config();
    TextEditorController* controller();

    TextEditorWidget* widget_;
    NewlineCommand* command_;
};


} // edbee

DECLARE_TEST(edbee::NewlineCommandTest);

