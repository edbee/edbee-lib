// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

class CascadingQVariantMap;

/// For testing the QVariantMap
class CascadingQVariantMapTest : public edbee::test::TestCase
{
Q_OBJECT

private slots:

    void testValue();
    void testRoot();


private:
    CascadingQVariantMap* createFixture();
    void destroyFixture( CascadingQVariantMap* item );

};

} // edbee

DECLARE_TEST(edbee::CascadingQVariantMapTest);
