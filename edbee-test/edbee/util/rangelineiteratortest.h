// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

/// performs testing of the RangeLine Iteratopr
class RangeLineIteratorTest : public edbee::test::TestCase
{
    Q_OBJECT

private slots:
    void testBasicIteration();
    void testSingleLineIteration();
};

} // edbee

DECLARE_TEST(edbee::RangeLineIteratorTest);
