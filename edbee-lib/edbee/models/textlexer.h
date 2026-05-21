// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include "edbee/models/textbuffer.h"

namespace edbee {

class RegexTextDocumentScopes;
class TextGrammar;
class TextDocument;
class TextDocumentScopes;

/// This is a single lexer
class EDBEE_EXPORT TextLexer {
public:
    TextLexer(TextGrammar* grammar, TextDocument* textDocument);
    virtual ~TextLexer() {}

    /// Inform the lexer some data has been changed
    // virtual void textReplaced( int offset, int length, int newLength ) = 0;
    virtual void textChanged(const edbee::TextBufferChange& change) = 0;

    inline TextGrammar* grammar() { return grammarRef_; }

    virtual void fullRefresh() = 0;

    /// Lex the given range
    /// WARNING, this method must be VERY optimized and should 'remember' the lexing
    /// states between calls. To invalidate the scopes/lexing state the textReplaced method can be used
    ///
    /// @param beginOffset the first offset
    /// @param endOffset the last offset to
    virtual void lexRange(size_t beginOffset, size_t endOffset) = 0;

    virtual TextDocumentScopes* textScopes() = 0;
    TextDocument* textDocument();

private:
    TextDocument* textDocumentRef_;	///< Reference to the document
    TextGrammar* grammarRef_;       ///< Reference to the grammar
};

} // edbee
