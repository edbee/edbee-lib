// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {


class GapVectorTest : public edbee::test::TestCase
{
    Q_OBJECT

private slots:

    void testMoveGap();
    void testResize();
    void testReplace();

    void testCopyRange();

    void testIssue141();

    void testIssueLineDataVector();

};

} // edbee

DECLARE_TEST(edbee::GapVectorTest);
