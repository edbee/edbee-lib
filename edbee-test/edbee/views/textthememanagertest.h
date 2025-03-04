// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

class TextThemeManagerTest : public edbee::test::TestCase
{
Q_OBJECT

private slots:

    void reloadingThemesShouldNotifyAllPointerOwners();

};

} // edbee

DECLARE_TEST(edbee::TextThemeManagerTest);

