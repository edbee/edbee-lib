/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "linedatalisttextchangetest.h"

#include "edbee/models/changes/linedatalisttextchange.h"
#include "edbee/models/chardocument/chartextdocument.h"
#include "edbee/models/textlinedata.h"

#include "debug.h"

namespace edbee {

/// constructs the basic textdocument
void LineDataListTextChangeTest::init()
{
    doc_ = new CharTextDocument();
    doc_->setText("1\n2\n3");
}


/// cleans the data
void LineDataListTextChangeTest::clean()
{
    qDeleteAll(changeList_);
    changeList_.clear();
    delete doc_;
    doc_ = 0;
}


/// Test the execution of the data
/// At the moment only the grow/shrink is tested
void LineDataListTextChangeTest::testExecute()
{
    // test inserting
    testEqual( manager()->length(), 3 );
    createChange(1, 0, 1)->execute( doc_ );
    testEqual( manager()->length(), 4 );

    // test replace
    createChange(0, 2, 3)->execute( doc_ );
    testEqual( manager()->length(), 5 );

    // test delete
    createChange(1, 3, 0)->execute( doc_ );
    testEqual( manager()->length(), 2 );
}


/// Placing a growing textchange under the previous textchange and merge
void LineDataListTextChangeTest::testMerge_growBelow()
{
    LineDataListTextChange* change1 = createChange(1, 0, 1);
    change1->execute(doc_);
    LineDataListTextChange* change2 = createChange(2, 0, 1);
    change2->execute(doc_);
    testTrue( change1->merge( doc_, change2 ) );
    testEqual( change1->line(), 1);
    testEqual( change1->length(), 0);
    testEqual( change1->newLength(), 2);
}


/// Placing a growing textchange above the previous textchange and merge
void LineDataListTextChangeTest::testMerge_growAbove()
{
    LineDataListTextChange* change1 = createChange(2, 0, 1);
    change1->execute(doc_);
    LineDataListTextChange* change2 = createChange(1, 0, 1);
    change2->execute(doc_);
    testTrue( change1->merge( doc_, change2 ) );
    testEqual( change1->line(), 1);
    testEqual( change1->length(), 0);
    testEqual( change1->newLength(), 2);
}


/// Placing a shrinking textchange under the previous textchange and merge
void LineDataListTextChangeTest::testMerge_shrinkBelow()
{
    LineDataListTextChange* change1 = createChange(1, 1, 0);
    change1->execute(doc_);
    LineDataListTextChange* change2 = createChange(1, 1, 0);
    change2->execute(doc_);
    testTrue( change1->merge( doc_, change2 ) );
    testEqual( change1->line(), 1);
    testEqual( change1->length(), 2);
    testEqual( change1->newLength(), 0);
}


/// Placing a shrinking textchange above the previous textchange and merge
void LineDataListTextChangeTest::testMerge_shrinkAbove()
{
    LineDataListTextChange* change1 = createChange(1, 1, 0);
    change1->execute(doc_);
    LineDataListTextChange* change2 = createChange(0, 1, 0);
    change2->execute(doc_);
    testTrue( change1->merge( doc_, change2 ) );
    testEqual( change1->line(), 0);
    testEqual( change1->length(), 2);
    testEqual( change1->newLength(), 0);
}


/// returns the line data manager
TextLineDataManager* LineDataListTextChangeTest::manager()
{
    return doc_->lineDataManager();
}


/// creates a line change
/// @param line the start line that's changed
/// @param length the new number of lines
/// @param newLength the new number of line
LineDataListTextChange* LineDataListTextChangeTest::createChange(int line, int length, int newLength)
{
    LineDataListTextChange* result = new LineDataListTextChange( manager(), line, length, newLength );
    changeList_.append(result);
    return result;
}


} // edbee
