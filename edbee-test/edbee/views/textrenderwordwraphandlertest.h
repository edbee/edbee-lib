#pragma once

#include "edbee/util/test.h"

namespace edbee {

class TextRenderWordwrapHandlerTest : public edbee::test::TestCase
{
    Q_OBJECT
private slots:

    void lineIndexForYpos();
};

}

DECLARE_TEST(edbee::TextRenderWordwrapHandlerTest);
