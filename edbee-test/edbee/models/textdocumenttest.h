/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "util/test.h"

namespace edbee {

class TextDocumentTest : public edbee::test::TestCase
{
    Q_OBJECT

private slots:

    void testLineData();

};


} // edbee


DECLARE_TEST(edbee::TextDocumentTest);
