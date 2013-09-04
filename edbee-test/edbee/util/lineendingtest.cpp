/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "lineendingtest.h"

#include "edbee/util/lineending.h"

#include "debug.h"

namespace edbee {

void LineEndingTest::testDetect()
{
    testEqual( LineEnding::detect("aaa\nbb\nccc")->type(), LineEnding::Unix );
    testEqual( LineEnding::detect("aaa\rbb\rccc")->type(), LineEnding::MacClassic );
    testEqual( LineEnding::detect("aaa\r\nbb\r\nccc")->type(), LineEnding::Windows );

    testEqual( LineEnding::detect("aaa\nbb\r\nccc\nddd")->type(), LineEnding::Unix );
    testEqual( LineEnding::detect("aaa\rbb\nccc\rddd")->type(), LineEnding::MacClassic );
    testEqual( LineEnding::detect("aaa\r\nbb\nccc\r\nddd")->type(), LineEnding::Windows );

    // multiple types (prefered type is unix)
    testEqual( LineEnding::detect("aaa\rbb\nccc\r\nddd")->type(), LineEnding::Unix);


    testTrue( LineEnding::detect("aaaaa") == 0 );
    testEqual( LineEnding::detect("aaaaa", LineEnding::get( LineEnding::Unix ) )->type(), LineEnding::Unix );
}



} // edbee
