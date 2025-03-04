// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

/// Contains tests of the utility functions
class UtilTest : public edbee::test::TestCase
{
    Q_OBJECT

private slots:
    void testConvertTabsToSpaces();
    void testTabColumnOffsets();

};


} // edbee

DECLARE_TEST(edbee::UtilTest);
