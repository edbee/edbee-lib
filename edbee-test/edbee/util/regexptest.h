/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "util/test.h"

namespace edbee {

class RegExpTest  : public edbee::test::TestCase
{
    Q_OBJECT
private slots:

    void testRegExp();

};

} // edbee

DECLARE_TEST(edbee::RegExpTest);

