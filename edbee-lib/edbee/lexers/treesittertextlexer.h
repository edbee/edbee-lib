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

class MultiLineScopedTextRange;
class RegExp;
class ScopedTextRange;
class ScopedTextRangeList;
class TextDocumentScopes;
class TextGrammar;
class TreeSitterTextGrammar;

/// A simple lexer matches texts with simple regular expressions
class EDBEE_EXPORT TreeSitterTextLexer : public TextLexer
{
public:
    TreeSitterTextLexer(TreeSitterTextGrammar* grammar, TextDocumentScopes* scopes);
    virtual ~TreeSitterTextLexer();

    virtual void textChanged(const TextBufferChange& change);
    TreeSitterTextGrammar* treeSitterTextGrammar();

    virtual void lexRange(size_t beginOffset, size_t endOffset);

    void updateTsTree();

    TSTree* createTsTree();


private:
    void updateHighlightScopes(uint32_t startByte = UINT32_MAX, uint32_t endByte = 0);

    TSParser* tsParser_;
    TSTree* tsTree_;

    // TSQuery* tsQuery_;
};

} // edbee
