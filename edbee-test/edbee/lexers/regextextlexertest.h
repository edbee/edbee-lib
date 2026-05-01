// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

class RegexTextLexer;
class TextDocument;
class TextDocumentScopes;
class TextGrammar;


/// The grammar text lexer test
class RegexTextLexerTest : public edbee::test::TestCase
{
    Q_OBJECT
public:
    RegexTextLexerTest();

private slots:
    void init();
    void clean();

    void testHamlLexer();

private:

private:
    void createFixtureDocument( const QString& data );

    TextDocumentScopes* scopes();
    RegexTextLexer* lexer();

    TextDocument* doc_;         ///< The document used for testign

};

} // edbee

DECLARE_TEST(edbee::RegexTextLexerTest);
