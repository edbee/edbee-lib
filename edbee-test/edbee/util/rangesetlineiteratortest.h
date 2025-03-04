// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

/// A test for testing the rangeset line iterator
class RangeSetLineIteratorTest  : public edbee::test::TestCase
{
    Q_OBJECT

private slots:
    void testBasicIteration();
};

} // edbee

DECLARE_TEST(edbee::RangeSetLineIteratorTest);
