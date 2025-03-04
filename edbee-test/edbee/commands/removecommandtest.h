// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

class RemoveCommand;

/// The unit tests for deleting characters/words and lines
class RemoveCommandTest : public edbee::test::TestCase
{
    Q_OBJECT
public:
    RemoveCommandTest();

private slots:

    void init();
    void clean();
    void testSmartBackspace();

private:
    RemoveCommand* command_;
};


} // edbee

DECLARE_TEST(edbee::RemoveCommandTest );

