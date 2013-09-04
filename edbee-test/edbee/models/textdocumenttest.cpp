/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "textdocumenttest.h"

#include <QStringList>
#include <QDebug>

#include "edbee/models/textbuffer.h"
#include "edbee/models/chardocument/chartextdocument.h"
#include "edbee/models/textlinedata.h"

#include "debug.h"

namespace edbee {

/// test the documentbuffer
/// The marker list is a list of markers, with the end marker closed with a -1
#define testBuffer( buf, txt, markers ) \
do { \
    testEqual( buf->text(), txt );  \
    QString lineOffsets = buf->lineOffsetsAsString(); \
    testEqual( lineOffsets, markers ); \
} while(false)


void TextDocumentTest::testLineData()
{

    CharTextDocument doc;
    TextBuffer* buf = doc.buffer();
    buf->appendText("aaa\nbbb\nccc");
    testTrue( doc.getLineData( 0, 0 ) == 0 );
    testTrue( doc.getLineData( 1, 0 ) == 0 );
    testTrue( doc.getLineData( 2, 0 ) == 0 );

    // set an item at line 1
    doc.giveLineData( 1, 0, new QStringTextLineData("test") );
    testTrue( doc.getLineData( 0, 0 ) == 0 );
    testTrue( doc.getLineData( 1, 0 ) != 0 );
    testTrue( doc.getLineData( 2, 0 ) == 0 );

    QStringTextLineData* data = dynamic_cast<QStringTextLineData*>( doc.getLineData(1,0) );
    testEqual( data->value(), "test" );
    data->setValue("new-test");

    // inserting a line should 'shift' the data to the next line
    buf->replaceText(1,0, "\n");
    testBuffer( buf, "a\naa\nbbb\nccc","0,2,5,9");
    testTrue( doc.getLineData( 0, 0 ) == 0 );
    testTrue( doc.getLineData( 1, 0 ) == 0 );    
    testTrue( doc.getLineData( 2, 0 ) != 0 );

    Q_ASSERT(doc.getLineData( 1, 0 )==0);
    Q_ASSERT(doc.getLineData( 2, 0 ));

    data = dynamic_cast<QStringTextLineData*>( doc.getLineData(2,0) );
    Q_ASSERT(data);
    testEqual( data->value(), "new-test" );

    // removing a line should 'shift' the data to the previous line
    buf->replaceText(0,4,"");
    testBuffer( buf, "\nbbb\nccc","0,1,5");
    testTrue( doc.getLineData( 0, 0 ) == 0 );
    testTrue( doc.getLineData( 1, 0 ) != 0 );
    testTrue( doc.getLineData( 2, 0 ) == 0 );

    // replacing a line with a new line should remove the field
    buf->replaceText(0,3,"\n");
    testBuffer( buf, "\nb\nccc","0,1,3");
    testTrue( doc.getLineData( 0, 0 ) == 0 );
    testTrue( doc.getLineData( 1, 0 ) == 0 );

    // remove all items
    buf->replaceText(0,100,"");
    testBuffer( buf, "","0");
    testTrue( doc.getLineData( 0, 0 ) == 0 );

}



} // edbee
