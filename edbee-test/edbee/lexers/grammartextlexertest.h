// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/util/test.h"

namespace edbee {

class GrammarTextLexer;
class TextDocument;
class TextDocumentScopes;
class TextGrammar;


/// The grammar text lexer test
class GrammarTextLexerTest : public edbee::test::TestCase
{
    Q_OBJECT
public:
    GrammarTextLexerTest();

private slots:
    void init();
    void clean();

    void testHamlLexer();

private:

private:
    void createFixtureDocument( const QString& data );

    TextDocumentScopes* scopes();
    GrammarTextLexer* lexer();

    TextDocument* doc_;         ///< The document used for testign

};

} // edbee

DECLARE_TEST(edbee::GrammarTextLexerTest);
