// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QHash>
#include <QObject>
#include <QStringList>
#include <QVector>

#include "edbee/models/textrange.h"
#include "edbee/util/gapvector.h"
#include "edbee/models/textdocumentscopes.h"

namespace edbee {


/// This class 'defines' a single document scope
class EDBEE_EXPORT MultiLineScopedTextRange : public ScopedTextRange
{
public:
    MultiLineScopedTextRange(size_t anchor, size_t caret, TextScope* scope);
    virtual ~MultiLineScopedTextRange();

    void setGrammarRule(RegexTextGrammarRule* rule);
    RegexTextGrammarRule* grammarRule() const;

    void giveEndRegExp(RegExp* regExp);
    RegExp* endRegExp();

    static bool lessThan(MultiLineScopedTextRange* r1, MultiLineScopedTextRange* r2);

private:
    RegexTextGrammarRule* ruleRef_; ///< The grammar rule that found this range
    RegExp* endRegExp_;        ///< The end regexp
};

//===========================================


/// A line based ScopedText range, that referenes a multi-line text-reference
class EDBEE_EXPORT MultiLineScopedTextRangeReference : public ScopedTextRange
{
public:
    MultiLineScopedTextRangeReference(MultiLineScopedTextRange& range);
    virtual ~MultiLineScopedTextRangeReference();

    /// returns the multi-line scoped text range
    virtual MultiLineScopedTextRange* multiLineScopedTextRange();

private:
    MultiLineScopedTextRange* multiScopeRef_;       ///< the reference to the multi-scoped textrange that defined this scope
};


//===========================================


/// This is a set of scoped textranges. This set is used
/// to remember parsed language ranges
class EDBEE_EXPORT MultiLineScopedTextRangeSet : public TextRangeSetBase
{
public:
    MultiLineScopedTextRangeSet(TextDocument* textDocument, TextDocumentScopes* textDocumentScopes);
    virtual ~MultiLineScopedTextRangeSet();
    void reset();

    // text range functionality
    virtual size_t rangeCount() const;
    virtual TextRange& range(size_t idx);
    virtual const TextRange& constRange(size_t idx) const;
    virtual void addRange(size_t anchor, size_t caret);
    virtual void addRange(const TextRange& range);

    virtual void removeRange(size_t idx);
    virtual void clear();
    virtual void toSingleRange();
    virtual void sortRanges();
    virtual MultiLineScopedTextRange& scopedRange(size_t idx);
    virtual MultiLineScopedTextRange& addRange(size_t anchor, size_t caret, const QString& name , RegexTextGrammarRule *rule);

    void removeAndInvalidateRangesAfterOffset(size_t offset);
    void removeAndInvalidateRangesBetweenOffsets(size_t offsetBegin, size_t offsetEnd);

    // adds a text scope
    void giveScopedTextRange(MultiLineScopedTextRange* textScope);
    void processChangesIfRequired(bool joinBorders);

    QString toString();

    TextDocumentScopes* textDocumentScopes();

private:

    TextDocumentScopes* textDocumentScopesRef_;         ///< A reference to the text document scopes
    QList<MultiLineScopedTextRange*> scopedRangeList_;  ///< A list of all scoped ranges
};

//===========================================

/// This class is used to 'contain' all document scope information
class EDBEE_EXPORT RegexTextDocumentScopes : public TextDocumentScopes
{
Q_OBJECT
public:
    RegexTextDocumentScopes(TextDocument* textDocument);
    virtual ~RegexTextDocumentScopes();

    void setDefaultScope(const QString& name, RegexTextGrammarRule *rule);
    MultiLineScopedTextRange& defaultScopedRange();

    void giveLineScopedRangeList(size_t line, ScopedTextRangeList* list);

    void giveMultiLineScopedTextRange(MultiLineScopedTextRange* range);
    void removeScopesAfterOffset(size_t offset);
    void removeScopesWithinOffsets(size_t offsetBegin, size_t offsetEnd);

    ScopedTextRangeIterator scopedRangeAtLine(size_t line);
    bool lineBasedOffsets() { return true; }
    ScopedTextRangeList* scopedRangesAtLineDeprecated(size_t line);
    size_t scopedLineCount();

    QVector<MultiLineScopedTextRange*> multiLineScopedRangesBetweenOffsets(size_t offsetBegin, size_t offsetEnd);
    TextScopeList scopesAtOffset(size_t offset, bool includeEnd = false);
    QVector<ScopedTextRange*> createScopedRangesAtOffsetList(size_t offset);
    MultiLineScopedTextRangeSet& scopedRanges();

    QString toString();
    QStringList scopesAsStringList();

    void dumpScopedLineAddresses(const QString& text = QString());

    size_t lastScopedOffset();
    void setLastScopedOffset(size_t offset);

// signals:
//     // TODO: We need to signal the range of the changed scopes
//     void lastScopedOffsetChanged(size_t previousOffset, size_t lastScopedOffset);


private:
    MultiLineScopedTextRange defaultScopedRange_;     ///< The default scoped text range
    MultiLineScopedTextRangeSet scopedRanges_;        ///< A list with all (multi-line) ranges
    GapVector<ScopedTextRangeList*> lineRangeList_;   ///< A list of all line scopes. Note: scopes are LINE based (offset is relativee to line start)

    /// This special variable is used to 'remember' to which offset the document has been scoped.
    /// This should speed up the syntax highlighting drasticly because the parsing only needs to happen
    /// to the end of the 'visible' document
    ///
    /// If you have an offset AFTER this offset the document has not been scoped yet. This needs to happen.
    /// For all 'open' multi-line scopes with an end-offset of (documentLength).
    ///
    /// The scopedToOffset_ should only mark the multi-line scopes. Single lines scopes do NOT affect other regions of the document
    size_t lastScopedOffset_;            ///< How far has the text been fully scoped?
};


} // edbee
