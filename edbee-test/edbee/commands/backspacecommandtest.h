/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "util/test.h"

namespace edbee {

class BackspaceCommand;

/// The smart backspace command test
class BackspaceCommandTest : public edbee::test::TestCase
{
    Q_OBJECT

private slots:

    void init();
    void clean();
    void testSmartBackspace();

private:
    BackspaceCommand* command_;
};


} // edbee

DECLARE_TEST(edbee::BackspaceCommandTest );

