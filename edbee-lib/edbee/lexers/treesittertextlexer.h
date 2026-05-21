// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QMap>
#include <QList>
#include <QVector>

#include "edbee/models/textlexer.h"

struct TSParser;
struct TSTree;
struct TSQuery;

namespace edbee {

class ScopedTextRange;
class ScopedTextRangeList;
class TextDocumentScopes;
class TextGrammar;
class TreeSitterTextGrammar;
class TreeSitterTextDocumentScopes;

/// A simple lexer matches texts with simple regular expressions
class EDBEE_EXPORT TreeSitterTextLexer : public TextLexer
{
public:
    TreeSitterTextLexer(TreeSitterTextGrammar* grammar, TextDocument *document);
    virtual ~TreeSitterTextLexer();

    virtual void textChanged(const TextBufferChange& change);
    TreeSitterTextGrammar* treeSitterTextGrammar();


    virtual TextDocumentScopes* textScopes();

    virtual void lexRange(size_t beginOffset, size_t endOffset);
    virtual void fullRefresh();

    void updateTsTree();

    TSTree* createTsTree();


private:
    void updateHighlightScopes(uint32_t startByte = UINT32_MAX, uint32_t endByte = 0);

    TreeSitterTextDocumentScopes* textDocumentScopes_; ///< TODO: Replace with treesitter version

    TSParser* tsParser_;
    TSTree* tsTree_;

    // TSQuery* tsQuery_;
};

} // edbee
