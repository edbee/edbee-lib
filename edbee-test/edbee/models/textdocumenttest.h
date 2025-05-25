// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

class TextDocumentTest : public edbee::test::TestCase
{
    Q_OBJECT

private slots:

    void testLineData();
    void testReplaceRangeSet_simple();
    void testReplaceRangeSet_sizeDiff();
    void testReplaceRangeSet_simpleInsert();
    void testReplaceRangeSet_delete();
    void testReplaceRangeSet_delete2();
};

} // edbee

DECLARE_TEST(edbee::TextDocumentTest);
