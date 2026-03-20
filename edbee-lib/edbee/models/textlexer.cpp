// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
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

    // TODO: Check the best place do this (We need to figure out if the rules are required for the scopes)
    TextRegexGrammar* regExGrammar = dynamic_cast<TextRegexGrammar*>(grammar);
    if (regExGrammar) {
        textScopes()->setDefaultScope(grammarRef_->defaultScopeName(), regExGrammar->mainRule());
    } else {
        textScopes()->setDefaultScope(grammarRef_->defaultScopeName(), nullptr);
    }
    textScopes()->removeScopesAfterOffset(0); // invalidate the complete scopes
}

/// This method returns the text document
TextDocument* TextLexer::textDocument()
{
    return textDocumentScopesRef_->textDocument();
}

} // edbee
