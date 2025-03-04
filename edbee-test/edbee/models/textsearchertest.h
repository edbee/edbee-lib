// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

class TextDocument;
class TextRangeSet;
class TextSearcher;


/// Basic seach testing
class TextSearcherTest : public edbee::test::TestCase
{
Q_OBJECT
public:
    TextSearcherTest();

private slots:
    void init();
    void clean();

    void testInitialState();
    void testFindNextRange();

    void testFindNext();
    void testFindPrev();
    void testSelectNext();
    void testSelectPrev();
    void testSelectAll();


private:
    TextDocument* createFixtureDocument();

    TextDocument* doc_;         ///< The document used for testign
    TextSearcher* searcher_;    ///< The searcher
    TextRangeSet* ranges_;      ///< The text ranges
};

} // edbee


DECLARE_TEST(edbee::TextSearcherTest);
