// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

class TextUndoStackTest : public edbee::test::TestCase
{
Q_OBJECT

private slots:
    void testMultiCaretUndoIssue196();
    void testClearUndoStackCrashIssue24();
    void testClearUndoStackShouldnotUnregisterTheControllerIssue24();

};

} // edbee

DECLARE_TEST(edbee::TextUndoStackTest);

