// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "regexptest.h"

#include "edbee/util/regexp.h"

#include "edbee/debug.h"

namespace edbee {

void RegExpTest::testRegExp()
{
    RegExp* regExp = new RegExp( "a+(b*)c*");
    size_t idx = regExp->indexIn("xxxaaabccccddddd");
    testEqual(idx, 3 );
    testEqual(regExp->pos(0), 3);
    testEqual(regExp->len(0), 8);

    testEqual(regExp->pos(1), 6); // the b is at the 6th position
    testEqual(regExp->len(1), 1);

    testEqual(regExp->pos(2), std::string::npos); // the b is at the 6th position
    testEqual(regExp->len(2), std::string::npos);

    delete regExp;
}


} // edbee
