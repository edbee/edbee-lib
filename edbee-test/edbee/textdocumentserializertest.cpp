// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textdocumentserializertest.h"

#include <QBuffer>

#include "edbee/models/chardocument/chartextdocument.h"
#include "edbee/models/textdocument.h"
#include "edbee/io/textdocumentserializer.h"

#include "edbee/debug.h"

namespace edbee {

void TextDocumentSerializerTest::testLoad()
{
    CharTextDocument doc;
    TextDocumentSerializer serializer( &doc );
    testEqual( doc.text(), QStringLiteral("") );

    // load the data
    QByteArray data("Test,\r\nWerkt het?\r\nRick!!");
    QBuffer buffer(&data);
    testTrue( serializer.load(&buffer) );
    testEqual( doc.text(), QStringLiteral("Test,\nWerkt het?\nRick!!"));    // windows line endings should be removed


    // clear the buffer
    doc.buffer()->setText("");
    testEqual( doc.text(), QStringLiteral("") );

    data = "Test,\nWerkt het?\nRick!!";
    buffer.setData(data);
    testTrue( serializer.load(&buffer) );
    testEqual( doc.text(), QStringLiteral("Test,\nWerkt het?\nRick!!"));    // windows line endings should be ke



}


} // edbee
