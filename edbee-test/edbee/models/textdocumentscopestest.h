// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

/// A special scope-selector test. Based on the official scopeselector tests of textmate
class TextDocumentScopesTest : public edbee::test::TestCase
{
Q_OBJECT

private slots:
    void testStartsWith();
    void testRindexOf();

    void testScopeSelectorRanking();

};

} // edbee

DECLARE_TEST(edbee::TextDocumentScopesTest);
