// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textlexer.h"

#include "edbee/models/grammars/regextextgrammar.h"
#include "edbee/models/textgrammar.h"
#include "edbee/models/textdocumentscopes.h"

#include "edbee/debug.h"

namespace edbee {

TextLexer::TextLexer(TextGrammar* grammar, TextDocument* document)
    : textDocumentRef_(document)
    , grammarRef_(grammar)
{
    grammarRef_->initialize();

}

/// This method returns the text document
TextDocument* TextLexer::textDocument()
{
    return textDocumentRef_;
}

} // edbee
