// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

class LineOffsetVector;

class LineOffsetVectorTest : public edbee::test::TestCase
{
    Q_OBJECT

private slots:

    void testMoveDeltaToIndex();
    void testChangeOffsetDelta();
    void testTextReplaced();
    void testFindLineFromOffset();
};


} // edbee

DECLARE_TEST(edbee::LineOffsetVectorTest);

