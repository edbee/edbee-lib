// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

/// performs some tests on the single textchanges
class TextChangeTest : public edbee::test::TestCase
{
    Q_OBJECT

private slots:
    void testBoundaryMethods();
    void testMerge1();
    void testMerge2();
    void testMerge3();
    void testMerge4();
    void testMerge5();

    void testMerge6_splitMerge();
    void testMerge7_splitMergeInvert();
};

} // edbee

DECLARE_TEST(edbee::TextChangeTest);
