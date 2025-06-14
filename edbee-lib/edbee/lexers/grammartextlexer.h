// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QMap>
#include <QList>
#include <QVector>

#include "edbee/models/textlexer.h"

namespace edbee {

class MultiLineScopedTextRange;
class RegExp;
class ScopedTextRange;
class ScopedTextRangeList;
class TextDocumentScopes;
class TextGrammar;
class TextGrammarRule;

/// A simple lexer matches texts with simple regular expressions
class EDBEE_EXPORT GrammarTextLexer : public TextLexer
{
public:
    GrammarTextLexer(TextDocumentScopes* scopes);
    virtual ~GrammarTextLexer();

    virtual void textChanged(const TextBufferChange& change);

private:
    virtual bool lexLine(size_t line, size_t& currentDocOffset );

public:
    virtual void lexLines(size_t line, size_t lineCount);
    virtual void lexRange(size_t beginOffset, size_t endOffset);

private:

    RegExp* createEndRegExp( RegExp* startRegExp, const QString &endRegExpStringIn);

    void findNextGrammarRule(const QString &line, size_t offsetInLine, TextGrammarRule *activeRule, TextGrammarRule *&foundRule, RegExp*& foundRegExp, size_t& foundPosition);
    void processCaptures(RegExp *foundRegExp, const QMap<size_t, QString>* foundCaptures);

    TextGrammarRule* findAndApplyNextGrammarRule(size_t currentDocOffset, const QString& line, size_t& offsetInLine);

    MultiLineScopedTextRange* activeMultiLineRange();
    ScopedTextRange* activeScopedTextRange();

    void popActiveRange();
    void pushActiveRange( ScopedTextRange* range, MultiLineScopedTextRange* multiRange );

    TextGrammarRule* findIncludeGrammarRule( TextGrammarRule* base );

private:

    QVector<MultiLineScopedTextRange*> activeMultiLineRangesRefList_;        ///< The current active scoped text ranges, DOC  (this is only valid during parsing)
    QVector<MultiLineScopedTextRange*> currentMultiLineRangeList_;           ///< The doc ranges currently created            (only valid during parsing
    QVector<MultiLineScopedTextRange*> closedMultiRangesRangesRefList_;      ///< A list of all ranges (from other lines) that have been closed. (only valid during parsing)

    QVector<ScopedTextRange*> activeScopedRangesRefList_;                    ///< The current active scoped text ranges, LINE (this is only valid during parsing)

//    QVector<MultiLineScopedTextRange*> currentLineRangesList_;      ///< The current scope ranges (only valid during parsing)

    ScopedTextRangeList* lineRangeList_;                            ///< The scopes at current line (only valid during parsing)

};

} // edbee
