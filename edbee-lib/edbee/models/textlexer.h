// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include "edbee/models/textbuffer.h"

namespace edbee {

class TextGrammar;
class TextDocument;
class TextDocumentScopes;

/// This is a single lexer
class EDBEE_EXPORT TextLexer {
public:
    TextLexer( TextDocumentScopes* scopes );
    virtual ~TextLexer() {}

    /// Inform the lexer some data has been changed
    // virtual void textReplaced( int offset, int length, int newLength ) = 0;
    virtual void textChanged(const edbee::TextBufferChange& change) = 0;

    /// Inform the lexer the grammar has been changed
    void setGrammar(TextGrammar* grammar);
    inline TextGrammar* grammar() { return grammarRef_; }

    /// Lex the given range
    /// WARNING, this method must be VERY optimized and should 'remember' the lexing
    /// states between calls. To invalidate the scopes/lexing state the textReplaced method can be used
    ///
    /// @param beginOffset the first offset
    /// @param endOffset the last offset to
    virtual void lexRange(size_t beginOffset, size_t endOffset ) = 0;

    TextDocumentScopes* textScopes() { return textDocumentScopesRef_; }
    TextDocument* textDocument();

private:
    TextDocumentScopes* textDocumentScopesRef_; ///< A Text document refs
    TextGrammar* grammarRef_;                   ///< The reference to the active grammar
};

} // edbee
