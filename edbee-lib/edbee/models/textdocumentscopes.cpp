// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textdocumentscopes.h"

#include <math.h>

#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"
#include "edbee/edbee.h"

#include "edbee/debug.h"

namespace edbee {

/// A scoped text range
/// @param anchor the start of the range
/// @param caret the caret position of the range
/// @param scope the text scope
ScopedTextRange::ScopedTextRange(size_t anchor, size_t caret, TextScope* scope)
    : TextRange(anchor, caret)
    , scopeRef_(scope)
{
    Q_ASSERT(scopeRef_);
}


/// The default destructor
ScopedTextRange::~ScopedTextRange()
{
}


/// sets the scope of this textrange
void ScopedTextRange::setScope(TextScope* scope)
{
    Q_ASSERT(scope);
    scopeRef_ = scope;
}


/// returns the scope
TextScope* ScopedTextRange::scope() const
{
    return scopeRef_;
}


/// Converts the scoped textrange to a string
QString ScopedTextRange::toString() const
{
    return QStringLiteral("%1>%2:%3").arg(anchor()).arg(caret()).arg(scopeRef_->name() );
}



//===========================================


/// A scoped textrange lsit
ScopedTextRangeList::ScopedTextRangeList()
    : ranges_()
    , independent_(false)
{
}


/// The default destructor
ScopedTextRangeList::~ScopedTextRangeList()
{
    qDeleteAll(ranges_);
    ranges_.clear();
}


/// Retursn the number of scoped textranges in the list
size_t ScopedTextRangeList::size() const
{
    return static_cast<size_t>(ranges_.size());
}


/// Returns the scoped textrange at the given list
ScopedTextRange* ScopedTextRangeList::at(size_t idx)
{
    Q_ASSERT(idx < size());
    return ranges_.at(static_cast<qsizetype>(idx));
}


/// give a range to the range set
/// @param range the scoped text range
void ScopedTextRangeList::giveRange(ScopedTextRange* range)
{
    ranges_.append(range);
}


/// Prepends a range
/// @param range the range to prepend
void ScopedTextRangeList::giveAndPrependRange(ScopedTextRange* range)
{
    ranges_.prepend(range);
}


/// Squeezes the ranges (reduces the memory usage)
void ScopedTextRangeList::squeeze()
{
    ranges_.squeeze();
}


/// set the independent flag. The independent flag means it's not dependent on other lines
void ScopedTextRangeList::setIndependent(bool enable)
{
    independent_ = enable;;
}


/// returns the independent flag
bool ScopedTextRangeList::isIndependent() const
{
    return independent_;
}


/// Removes all scopes that are before the given offset
void ScopedTextRangeList::removeToOffset(size_t offset)
{
    for (auto it = ranges_.begin(); it != ranges_.end(); ) {
        ScopedTextRange* range = *it;
        if (range->max() <= offset) {
            delete range;
            it = ranges_.erase(it);  // erase() returns next valid iterator
        } else {
            ++it;
        }
    }
}

/// Removes all scopes that are after the given offset
void ScopedTextRangeList::removeFromOffset(size_t offset)
{
    for (auto it = ranges_.begin(); it != ranges_.end(); ) {
        ScopedTextRange* range = *it;
        if (range->min() <= offset) {
            delete range;
            it = ranges_.erase(it);  // erase() returns next valid iterator
        } else {
            ++it;
        }
    }
}

/// Removes all scopes within the given offsets
void ScopedTextRangeList::removeWithinOffset(size_t offsetBegin, size_t offsetEnd)
{
    TextRange overlappingRange(offsetBegin, offsetEnd);
    for (auto it = ranges_.begin(); it != ranges_.end(); ) {
        ScopedTextRange* range = *it;
        if (range->touches(overlappingRange)) {
            delete range;
            it = ranges_.erase(it);  // erase() returns next valid iterator
        } else {
            ++it;
        }
    }
}


/// Clears all ranges
void ScopedTextRangeList::clear()
{
    ranges_.clear();
}


/// Converts the scoped textrange list to a strubg
QString ScopedTextRangeList::toString()
{
    QString result;
    result.append( independent_ ? "[-]" : "[M]");
    foreach(ScopedTextRange* scope, ranges_) {
        if (!result.isEmpty()) { result.append("| "); }
        result.append( scope->toString());
    }
    return result;
}




//===========================================


/// A textcope is an object that represents a scope name
/// You usually don't need to supply the scopeManager parameter. It's only required on application startup
///
/// @param fullScope the name of the scope
/// @param scopeManager the scopemanager to use (when 0 this defaults to the global edbee scopemanager)
TextScope::TextScope(const QString& fullScope)
    : scopeAtomCount_(0)
    , scopeAtoms_(nullptr)
{
    QStringList scopeElementNames = fullScope.split(".");
    scopeAtomCount_ = static_cast<size_t>(scopeElementNames.length());

    TextScopeManager* sm = Edbee::instance()->scopeManager();
    scopeAtoms_ = new TextScopeAtomId[scopeAtomCount_];
    for (size_t i = 0; i < scopeAtomCount_; ++i) {
        TextScopeAtomId id = sm->findOrRegisterScopeAtom(scopeElementNames.at(static_cast<qsizetype>(i)));
        scopeAtoms_[i] = id;
    }
}

/// this method constructs a blank text scope.
TextScope::TextScope()
    : scopeAtomCount_(0)
    , scopeAtoms_(nullptr)
{
}


// Destructs the textscope
TextScope::~TextScope()
{
    delete[] scopeAtoms_;
}


/// Returns the name of this scope
const QString TextScope::name()
{
    QString str;
    TextScopeManager* sm = Edbee::instance()->scopeManager();
    for (size_t i = 0; i < scopeAtomCount_; ++i) {
        if (i) { str.append("."); }
        str.append(sm->atomName(scopeAtoms_[i])) ;
    }
    return str;
}


/// returns the number of atom items
size_t TextScope::atomCount()
{
    return scopeAtomCount_;
}


/// Returns the atom at the given index
TextScopeAtomId TextScope::atomAt(size_t idx) const
{
    Q_ASSERT(idx < scopeAtomCount_ );
    return scopeAtoms_[idx];
}


/// Checks if the current scope starts with the given scope
/// wild-card atoms will always match
/// @param scope the scope to check
bool TextScope::startsWith(TextScope* scope)
{
    if (scope->atomCount() > atomCount()) { return false; }    // match is never possible
    TextScopeAtomId wildCard = Edbee::instance()->scopeManager()->wildcardId();
    for (size_t i = 0; i <scope->scopeAtomCount_; ++i) {
        TextScopeAtomId atomId = scopeAtoms_[i];
        TextScopeAtomId scopeAtomId = scope->scopeAtoms_[i];
        bool atomEqual =  atomId == scopeAtomId || atomId == wildCard || scopeAtomId == wildCard;
        if (!atomEqual) { return false; }
    }
    return true;
}


/// Returns the 'index' of the given full-scope. This is a kind of substring search
///
/// - wildcard atoms will always match
///
/// @param scope the scope to search
/// @return std::string::npos if not found else the index where the scope is found
size_t TextScope::rindexOf(TextScope* scope)
{
    size_t searchSize = scope->atomCount();
    size_t end = atomCount() - searchSize;

    TextScopeAtomId wildCard = Edbee::instance()->scopeManager()->wildcardId();

    for (size_t idx = end; idx != std::string::npos; --idx) {

        // try to match the scopes
        size_t i=0;
        for (; i < searchSize; ++i) {
            TextScopeAtomId atomId = scopeAtoms_[idx + i];
            TextScopeAtomId scopeAtomId = scope->scopeAtoms_[i];
            bool atomEqual =  atomId == scopeAtomId || atomId == wildCard || scopeAtomId == wildCard;
            if (!atomEqual) { break; }
        }
        // when at the end we've found it
        if (i == searchSize) { return idx; }
    }
    return std::string::npos;
}


//=============================================


/// A list of text scopes
TextScopeList::TextScopeList()
{
}


/// The text scopelist with an initial size
TextScopeList::TextScopeList(int initialSize)
{
    reserve(initialSize);
}


/// Fills the scopelist with a list of ranges
TextScopeList::TextScopeList(QVector<ScopedTextRange *>& ranges)
{
    reserve(ranges.size());
    foreach( ScopedTextRange* scopedRange, ranges ) {
        append( scopedRange->scope() );
    }
}


/// Returns the total number of atoms
size_t TextScopeList::atomCount() const
{
    size_t result = 0;
    for (qsizetype i = 0, cnt = static_cast<qsizetype>(size()); i < cnt; ++i ) {
        result += at(i)->atomCount();
    }
    return result;
}


/// Converts the scopelist to a string
QString TextScopeList::toString()
{
    QString result;
    for (qsizetype i = 0; i < static_cast<qsizetype>(size()); ++i) {
        if (i) { result.append(" "); }
        result.append(at(i)->name());
    }
    return result;
}

//=============================================


/// The textscope selector
/// @param selector the selector to select a scope
TextScopeSelector::TextScopeSelector(const QString& selector)
{
    QStringList selectorList = selector.split(",");
    selectorList_.reserve(selectorList.size());
    foreach(QString sel, selectorList) {
        sel = sel.trimmed();
        selectorList_.append(Edbee::instance()->scopeManager()->createTextScopeList(sel));
    }
}


/// The destructor
TextScopeSelector::~TextScopeSelector()
{
    qDeleteAll(selectorList_);
    selectorList_.clear();
}


/// Calculates the matching score of the scope with this selector
/// Currently the scope-calculation is very 'basic'. Just enough to perform the most basic form of matching
///
/// The algorithm returns 1.0 on a full match...
///
/// @param scopeList the list of scopes
/// @return the matching score. -1 means no match found. >= 0 a match has been found
double TextScopeSelector::calculateMatchScore(const TextScopeList* scopeList)
{
    double result = -1.0;
    foreach(TextScopeList* selector, selectorList_) {
        result = qMax(result, calculateMatchScoreForSelector(selector, scopeList));
    }
    return result;
}


/// Converts the scope to a string
QString TextScopeSelector::toString()
{
    QString result;
    for (qsizetype i = 0, cnt = selectorList_.size(); i < cnt; ++i) {
        if (i) { result.append(", "); }
        result.append(selectorList_.at(i)->toString());
    }
    return result;
}


/// Calculates the matching score of the scope with this selector
/// Currently the scope-calculation is very 'basic'. Just enough to perform the most basic form of matching
///
/// The algorithm returns 1.0 on a full match...
///
/// @param scopeList the list of scopes
/// @return the matching score. -1 means no match found. >= 0 a match has been found
double TextScopeSelector::calculateMatchScoreForSelector(TextScopeList* selector, const TextScopeList* scopeList)
{
    double result = 0.0;
    double power = 0.0;

    // this method checks if the scope matches
    bool foundMatch=false;
    qsizetype nextScopeIdx = scopeList->size()-1;
    for (qsizetype selectorIdx = selector->size() - 1; selectorIdx >= 0; --selectorIdx) {
        TextScope* selectorPath = selector->at(selectorIdx);
        foundMatch=false;

        // find the given selector scope in the list
        for (qsizetype scopeIdx = nextScopeIdx; scopeIdx >= 0; --scopeIdx) {
            TextScope* scope = scopeList->at(scopeIdx);
            power += static_cast<double>(scope->atomCount());

            // dit we found the scope
            if (scope->startsWith(selectorPath)) {
                nextScopeIdx = scopeIdx - 1;
                foundMatch = true;
                for (size_t i = 0, iCnt = selectorPath->atomCount(); i < iCnt; ++i) {
                    result += 1.0 / pow(2, power - static_cast<double>(i));
                }
                break;
            }
        }

        // no match found?
        if (!foundMatch) { return -1; }
    }
    return result;
}



//=============================================


/// The scopemanager constructor
TextScopeManager::TextScopeManager()
{
    reset();
}


/// The destructor of the scope manager
TextScopeManager::~TextScopeManager()
{
    foreach(TextScope* textScope, textScopeList_) { delete textScope; }
    textScopeList_.clear();
    textScopeRefMap_.clear();
}


/// Clears and delets all scopes. WARNING this destroys all registered text-scopes
/// and registers the wildcard scope atom id
void TextScopeManager::reset()
{
    // delete and clear the scopemaps
    if (!textScopeList_.isEmpty()) {
        foreach(TextScope* textScope, textScopeList_ ) { delete textScope; }
        textScopeList_.clear();
        textScopeRefMap_.clear();
    }

    // clear the atomlists
    if (!atomNameList_.isEmpty()) {
        atomNameList_.clear();
        atomNameMap_.clear();
    }

    // insert some defaults
    wildCardId_ = findOrRegisterScopeAtom("*");     // register the 'start' wildcard

    // create a blank textscope
    TextScope* scope = new TextScope();
    textScopeList_.append(scope);
    textScopeRefMap_.insert("", scope);
}


/// returns the wildcard reference id
TextScopeAtomId TextScopeManager::wildcardId()
{
    return wildCardId_;
}


/// Registers the scope element
TextScopeAtomId TextScopeManager::findOrRegisterScopeAtom(const QString& atom)
{
//    element = element.toLower().trimmed();
    TextScopeAtomId id = atomNameMap_.value(atom, -1);
    if (id >= 0) { return id; }
    atomNameList_.append(atom);
    id = static_cast<short>(atomNameList_.size() - 1);
    atomNameMap_.insert(atom, id);
    return id;
}


/// Finds or creates a full-scope
TextScope* TextScopeManager::refTextScope(const QString& scopeString)
{
    TextScope* scope = textScopeRefMap_.value(scopeString, 0);
    if (scope) { return scope; }
    scope = new TextScope(scopeString);
    textScopeList_.append(scope);
    textScopeRefMap_.insert(scopeString, scope);
    return scope;
}


/// Returns the empty scope
/// @return the blank textscope
TextScope* TextScopeManager::refEmptyScope()
{
    return refTextScope("");
}


/// Creates a text-scope list from the given scope string
TextScopeList* TextScopeManager::createTextScopeList(const QString& scopeListString)
{
    QStringList scopeParts = scopeListString.split(" ");
    TextScopeList* list = new TextScopeList(static_cast<int>(scopeParts.size()));
    for (qsizetype i = 0, cnt=scopeParts.size(); i < cnt; ++i) {
        list->append(refTextScope(scopeParts.at(i)));
    }
    return list;
}


/// Returns the name of the given atom id
const QString& TextScopeManager::atomName(TextScopeAtomId id)
{
    Q_ASSERT(0 <= id && id < atomNameList_.length());
    return atomNameList_.at(id);
}

//===========================================

ScopedTextRangeIterator::ScopedTextRangeIterator(ScopedTextRangeIteratorStrategy* iteratorStrategy)
    : strategy(iteratorStrategy)
{
}

ScopedTextRangeIterator::~ScopedTextRangeIterator()
{
}

ScopedTextRange *ScopedTextRangeIterator::next()
{
    return strategy->next();
}


//===========================================


/// The constructor for storing/managing the textdocument scopes
/// @param textDocument the textdocument this scoping is for
TextDocumentScopes::TextDocumentScopes(TextDocument* textDocument)
    : textDocumentRef_(textDocument)
{
}


TextDocumentScopes::~TextDocumentScopes()
{
}

/// returns the current textdocument scope
TextDocument*TextDocumentScopes::textDocument()
{
    return textDocumentRef_;
}


} // edbee
