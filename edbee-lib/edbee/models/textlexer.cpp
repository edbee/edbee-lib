// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textlexer.h"

#include "edbee/models/textgrammar.h"
#include "edbee/models/textdocumentscopes.h"

#include "edbee/debug.h"

namespace edbee {

TextLexer::TextLexer( TextDocumentScopes* scopes)
    : textDocumentScopesRef_(scopes)
    , grammarRef_(nullptr)
{
}

void TextLexer::setGrammar(TextGrammar* grammar)
{
    Q_ASSERT(grammar);
    grammarRef_ = grammar;
    textScopes()->setDefaultScope(grammarRef_->mainRule()->scopeName(), grammarRef_->mainRule());
    textScopes()->removeScopesAfterOffset(0); // invalidate the complete scopes
}

/// This method returns the text document
TextDocument* TextLexer::textDocument()
{
    return textDocumentScopesRef_->textDocument();
}

} // edbee
