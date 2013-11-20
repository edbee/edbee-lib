/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QList>

#include "util/test.h"

namespace edbee {

class LineDataListTextChange;
class TextDocument;
class TextLineDataManager;

// For testing the line data list text changes
class LineDataListTextChangeTest: public edbee::test::TestCase
{
    Q_OBJECT

private slots:
    void init();
    void clean();

    void testExecute();
    void testMerge_growBelow();
    void testMerge_growAbove();
    void testMerge_shrinkBelow();
    void testMerge_shrinkAbove();

private:
    TextLineDataManager* manager();
    LineDataListTextChange* createChange( int line, int length, int newLength );

    TextDocument* doc_;                             ///< The document used for testing
    QList<LineDataListTextChange*> changeList_;     ///< The change list (For auto deletion of changes)
};

} // edbee

DECLARE_TEST(edbee::LineDataListTextChangeTest);
