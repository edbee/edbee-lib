// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <QList>
#include "edbee/util/test.h"

namespace edbee {

class TextScopeList;
class TextScopeManager;

class DynamicVariablesTest : public edbee::test::TestCase
{
    Q_OBJECT

private slots:
    void clean();

    void testSize();
    void testValue();

private:
    TextScopeList* createTextScopeList( TextScopeManager& tm, const QString& name );

    QList<TextScopeList*> scopeList_;
};

} // edbee

DECLARE_TEST(edbee::DynamicVariablesTest);
