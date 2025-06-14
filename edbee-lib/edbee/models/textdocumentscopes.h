// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QHash>
#include <QObject>
#include <QStringList>
#include <QVector>

#include "edbee/models/textrange.h"
#include "edbee/util/gapvector.h"

namespace edbee {

class MultiLineScopedTextRange;
class RegExp;
class ScopedTextRange;
class TextDocumentScopes;
class TextGrammarRule;
class TextScope;

/// This type defines a single scope atom
typedef short TextScopeAtomId;

/*
    ScopeElement
    FullScope =   ScopeElement.ScopeElement.ScopeElement
*/

/// This class defines a full text-scope. A full textscope is textscope
/// with one ore more scoped-elements
class EDBEE_EXPORT TextScope {
public:
    const QString name();
    size_t atomCount();
    TextScopeAtomId atomAt(size_t idx) const;

    bool startsWith(TextScope* scope);
    size_t rindexOf(TextScope* scope);

private:
    TextScope( const QString& fullScope );
    TextScope();
    ~TextScope();

    size_t scopeAtomCount_;              ///< the number of scope-atoms
    TextScopeAtomId* scopeAtoms_;        ///< the scope atoms

    friend class TextScopeManager;
};


//===========================================


/// A list of text-scopes.
/// on a certian location, usually more then one scope is available on a given location
class EDBEE_EXPORT TextScopeList : public QVector<TextScope*>
{
public:
    TextScopeList();
    TextScopeList(int initialSize);
    TextScopeList(QVector<ScopedTextRange*>& ranges);

    size_t atomCount() const;

    QString toString();
};


//===========================================

///
/// Our first version of scope-selector only support a list of scopes
/// Thus the Descendant (space) selector only
///
/// TODO: Implement full scope selectors see text below
///
/// Full Scope selectors: (FUTURE)
///
///  ,       = grouping (multiple seperate selectors)
///  (space) = descendent (supported)
///
///  -       = exclude (unsupported)
///  |       = or (unsupported)
///  &       = and (unsupported)
///  ()      = unsupported
///
/// Extra From the textmate author:
/// http://blog.macromates.com/2005/introduction-to-scopes/
/// discuession on:  http://textmate.1073791.n5.nabble.com/formal-definition-of-scope-selector-syntax-td12109.html
///
/// Update: Starting with 1.1b17 it's also possible
/// to AND, OR, and subtract scope selectors,
///
/// e.g.: (a | b) & c - d
///
/// would select the scope which is not matched by d, and matched by both c, and a or b.
///
/// OFFICIAL BNF:
///   atom:         «string» | '*'
///   scope:        «atom» ('.' «atom»)*
///   path:         '^'? «scope» ('>'? «scope»)* '$'?
///   group:        '(' «selector» ')'
///   filter:       ("L:"|"R:"|"B:") («group» | «path»)
///   expression:   '-'? («filter» | «group» | «path»)
///   composite:    «expression» ([|&-] «expression»)*
///   selector:     «composite» (',' «composite»)*
///
class EDBEE_EXPORT TextScopeSelector {
public:
    TextScopeSelector(const QString& selector);
    virtual ~TextScopeSelector();

    double calculateMatchScore(const TextScopeList* scopeList);
    QString toString();

private:
    double calculateMatchScoreForSelector(TextScopeList* selector, const TextScopeList* scopeList);

private:
    QVector<TextScopeList*> selectorList_;
};


//===========================================


/// The scope manager is used to manage the scopes...
/// A scope consist out of several scope-parts:
///
///  scope-part1.scope-part2.scope-part3
///
/// These text are converted to a list of numbers
///   12.3.24
///
class EDBEE_EXPORT TextScopeManager {
public:
    TextScopeManager();
    virtual ~TextScopeManager();

public:

    void reset();

    TextScopeAtomId wildcardId();

    TextScopeAtomId findOrRegisterScopeAtom(const QString& atom);
    TextScope* refTextScope(const QString& scopeString);
    TextScope* refEmptyScope();

    TextScopeList* createTextScopeList(const QString &scopeListString);

    const QString& atomName(TextScopeAtomId id);

private:
    TextScopeAtomId wildCardId_;                            ///< The atom id reserved for the wildcard '*'

    // scope atoms
    QList<QString> atomNameList_;                           ///< All scope-atom names
    QHash<QString, TextScopeAtomId> atomNameMap_;           ///< the scope atom map

    // full scopes
    QList<TextScope*> textScopeList_;                       ///< The list of full-scope
    QHash<QString, TextScope*> textScopeRefMap_;            ///< The full-scope map
};


//===========================================


/// A base scoped text range
class EDBEE_EXPORT ScopedTextRange : public TextRange
{
public:
    ScopedTextRange(size_t anchor, size_t caret, TextScope* scope);
    virtual ~ScopedTextRange();

    void setScope(TextScope* scope);
    TextScope* scope() const;
    QString toString() const;

    /// returns the multi-line scoped text range
    virtual MultiLineScopedTextRange* multiLineScopedTextRange() { return nullptr; }


private:
    TextScope* scopeRef_;      ///< The scope for this range

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


/// a list of textscopes
/// This class is used for single-line scopes
/// (Todo, this needs to be optimized)
class EDBEE_EXPORT ScopedTextRangeList {
    Q_DISABLE_COPY(ScopedTextRangeList)
public:

    explicit ScopedTextRangeList();
    virtual ~ScopedTextRangeList();

    size_t size() const;
    ScopedTextRange* at(size_t idx);
    void giveRange(ScopedTextRange* at);
    void giveAndPrependRange(ScopedTextRange* range);

    void squeeze();
    void setIndependent(bool enable = true);
    bool isIndependent() const;

    QString toString();

private:

    QVector<ScopedTextRange*> ranges_;  ///< the textranges
    bool independent_;                  ///< this boolean tells if the line contains a multi-lined scope start or end
};


//===========================================


/// This class 'defines' a single document scope
class EDBEE_EXPORT MultiLineScopedTextRange : public ScopedTextRange
{
public:
    MultiLineScopedTextRange(size_t anchor, size_t caret, TextScope* scope);
    virtual ~MultiLineScopedTextRange();

    void setGrammarRule(TextGrammarRule* rule);
    TextGrammarRule* grammarRule() const;

    void giveEndRegExp(RegExp* regExp);
    RegExp* endRegExp();

    static bool lessThan(MultiLineScopedTextRange* r1, MultiLineScopedTextRange* r2);

private:
    TextGrammarRule* ruleRef_; ///< The grammar rule that found this range
    RegExp* endRegExp_;        ///< The end regexp
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
    virtual MultiLineScopedTextRange& addRange(size_t anchor, size_t caret, const QString& name , TextGrammarRule *rule);

    void removeAndInvalidateRangesAfterOffset(size_t offset);

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
class EDBEE_EXPORT TextDocumentScopes : public QObject
{
Q_OBJECT

public:
    TextDocumentScopes(TextDocument* textDocument);
    virtual ~TextDocumentScopes();

    size_t lastScopedOffset();
    void setLastScopedOffset(size_t offset);

    // scope management
    void setDefaultScope(const QString& name, TextGrammarRule *rule);

    void giveLineScopedRangeList(size_t line, ScopedTextRangeList* list);
    ScopedTextRangeList* scopedRangesAtLine(size_t line);
    size_t scopedLineCount();

    void giveMultiLineScopedTextRange(MultiLineScopedTextRange* range);
    void removeScopesAfterOffset(size_t offset);
    MultiLineScopedTextRange& defaultScopedRange();

    QVector<MultiLineScopedTextRange*> multiLineScopedRangesBetweenOffsets(size_t offsetBegin, size_t offsetEnd);
    TextScopeList scopesAtOffset(size_t offset, bool includeEnd = false);
    QVector<ScopedTextRange*> createScopedRangesAtOffsetList(size_t offset);

    QString toString();
    QStringList scopesAsStringList();

    void dumpScopedLineAddresses(const QString& text = QString());

    // getters
    TextDocument* textDocument();

protected slots:

    void grammarChanged();

signals:
    void lastScopedOffsetChanged(size_t previousOffset, size_t lastScopedOffset);

private:
    TextDocument* textDocumentRef_;             ///< The default document reference

    MultiLineScopedTextRange defaultScopedRange_;     ///< The default scoped text range
    MultiLineScopedTextRangeSet scopedRanges_;        ///< A list with all (multi-line) ranges
    GapVector<ScopedTextRangeList*> lineRangeList_;   ///< A list of all line scopes

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
