// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {


/// The clas for testing the textbuffer
class TextBufferTest : public edbee::test::TestCase
{
    Q_OBJECT

private slots:

    void testlineFromOffset();
    void testColumnFromOffsetAndLine();
    void testReplaceText();
    void testFindCharPosWithinRange();
    void testLine();
    void testReplaceIssue141();
};

} // edbee

DECLARE_TEST(edbee::TextBufferTest);
