#include "textrenderwordwraphandlertest.h"

#include "edbee/views/textrenderwordwraphandler.h"
#include "edbee/views/textrenderer.h"

#include "edbee/debug.h"

namespace edbee {

void edbee::TextRenderWordwrapHandlerTest::lineIndexForYpos()
{
    // we can test this with a null textrenderer (if we NOT exceed the last y-pos in the offset-cache)
    TextRendererWordwrapHandler handler(nullptr);
    handler.wordwrapLineYOffsetCache() << 0 << 10 << 25 << 40 << 200 << 400;

    testEqual( handler.lineIndexForYpos(-1), 0 );
    testEqual( handler.lineIndexForYpos(0), 0 );
    testEqual( handler.lineIndexForYpos(4), 0 );
    testEqual( handler.lineIndexForYpos(9), 0 );
    testEqual( handler.lineIndexForYpos(10), 1 );
    testEqual( handler.lineIndexForYpos(15), 1 );
    testEqual( handler.lineIndexForYpos(30), 2 );
    testEqual( handler.lineIndexForYpos(250), 4 );

}

} // edbee
