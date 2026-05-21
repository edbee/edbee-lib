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

namespace edbee {

class MultiLineScopedTextRange;
class RegExp;
class ScopedTextRange;
class TextDocumentScopes;
class RegexTextGrammarRule;
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
    TextScope(const QString& fullScope);
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


struct ScopedTextRangeIteratorStrategy {
    virtual ~ScopedTextRangeIteratorStrategy() = default;
    virtual ScopedTextRange* next() = 0;
};


//===========================================


struct ScopedTextRangeEmptyIteratorStrategy : public ScopedTextRangeIteratorStrategy {
    virtual ~ScopedTextRangeEmptyIteratorStrategy() = default;
    virtual ScopedTextRange* next() { return nullptr; };
};


//===========================================

/// A ScopedTextRange iterator. Can be used to iterator over a list of
/// ScopedTextRanges. This allows efficient iteration of multiple implementation
/// of text scopes
///
/// It's very simple:
///
/// ```c++
/// ScopedTextRangeIterator it;
/// ScopedTextRange *range = nullptr;
/// while(range = it.next()) {
/// }
/// ```
class EDBEE_EXPORT ScopedTextRangeIterator
{
public:
    explicit ScopedTextRangeIterator(ScopedTextRangeIteratorStrategy* iteratorStrategy);
    virtual ~ScopedTextRangeIterator();

    virtual ScopedTextRange* next();

private:
    std::unique_ptr<ScopedTextRangeIteratorStrategy> strategy;
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

    void removeToOffset(size_t offset);
    void removeFromOffset(size_t offset);
    void removeWithinOffset(size_t offsetBegin, size_t offsetEnd);
    void clear();

    QString toString();

private:

    QVector<ScopedTextRange*> ranges_;  ///< the textranges
    bool independent_;                  ///< this boolean tells if the line contains a multi-lined scope start or end
};


//===========================================

/// This class is used to 'contain' all document scope information
class EDBEE_EXPORT TextDocumentScopes : public QObject
{
Q_OBJECT

public:
    TextDocumentScopes(TextDocument* textDocument);
    virtual ~TextDocumentScopes();

    TextDocument* textDocument();

    virtual ScopedTextRangeIterator scopedRangeAtLine(size_t line) = 0;
    virtual bool lineBasedOffsets() = 0;
    virtual size_t scopedLineCount() = 0;

    // used by actions:
    virtual QVector<ScopedTextRange*> createScopedRangesAtOffsetList(size_t offset) = 0;
    virtual TextScopeList scopesAtOffset(size_t offset, bool includeEnd = false) = 0;

    virtual QString toString() = 0;
    virtual QStringList scopesAsStringList() = 0;

signals:
    void scopesChanged(size_t offset, size_t endOffset, size_t line, size_t endLine);


private:
    TextDocument* textDocumentRef_;             ///< The default document reference

};

} // edbee
