// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textdocumentscopes.h"

#include <math.h>

#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"
#include "edbee/edbee.h"
#include "edbee/util/regexp.h"

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



/// Creates a scoped text range based on a scope textrange
MultiLineScopedTextRangeReference::MultiLineScopedTextRangeReference(MultiLineScopedTextRange& range)
    : ScopedTextRange(range.anchor(), range.caret(), range.scope())
    , multiScopeRef_(&range)
{
}


// the destructor
MultiLineScopedTextRangeReference::~MultiLineScopedTextRangeReference()
{
}


/// Returns the reference the multilinescoped textrange
MultiLineScopedTextRange* MultiLineScopedTextRangeReference::multiLineScopedTextRange()
{
    return multiScopeRef_;
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


/// The multiline scoped textrange
/// @param anchor
MultiLineScopedTextRange::MultiLineScopedTextRange(size_t anchor, size_t caret, TextScope* scope)
    : ScopedTextRange(anchor, caret, scope)
    , ruleRef_(nullptr)
    , endRegExp_(nullptr)
{
}


/// The multi-line destructor
MultiLineScopedTextRange::~MultiLineScopedTextRange()
{
    delete endRegExp_;
}


/// Sets the rule (we need the rule to perform end-of-line matching)
void MultiLineScopedTextRange::setGrammarRule(TextGrammarRule* rule)
{
    ruleRef_ = rule;
}


/// Returns the active grammar rule
TextGrammarRule* MultiLineScopedTextRange::grammarRule() const
{
    return ruleRef_;
}


/// Gives the end regular expression
void MultiLineScopedTextRange::giveEndRegExp(RegExp* regExp)
{
    endRegExp_ = regExp;
}


/// returns the end-regular expression
RegExp*MultiLineScopedTextRange::endRegExp()
{
    return endRegExp_;
}


/// Compares selection ranges
bool MultiLineScopedTextRange::lessThan(MultiLineScopedTextRange* r1, MultiLineScopedTextRange* r2)
{
    size_t min1 = r1->min();
    size_t min2 = r2->min();

    if (min1 < min2) return true;
    if (min1 == min2)  return r1->length() < r2->length();
    return false;
}


/// Returns a reference to the textdocument scopes
TextDocumentScopes* MultiLineScopedTextRangeSet::textDocumentScopes()
{
     return textDocumentScopesRef_;
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


/// A multilinescoped textrange set
MultiLineScopedTextRangeSet::MultiLineScopedTextRangeSet(TextDocument *textDocument , TextDocumentScopes *textDocumentScopes)
    : TextRangeSetBase(textDocument)
    , textDocumentScopesRef_(textDocumentScopes)
{
}

/// the destructor of the multiline scoped rangeset
MultiLineScopedTextRangeSet::~MultiLineScopedTextRangeSet()
{
    reset();
}


/// Completely empties the scope list
void MultiLineScopedTextRangeSet::reset()
{
    qDeleteAll(scopedRangeList_);
    scopedRangeList_.clear();
}


/// Returns the number of ranges
size_t MultiLineScopedTextRangeSet::rangeCount() const
{
    return static_cast<size_t>(scopedRangeList_.size());
}


/// returns the given textrange
TextRange& MultiLineScopedTextRangeSet::range(size_t idx)
{
    Q_ASSERT(idx < rangeCount());
    return *(scopedRangeList_[static_cast<qsizetype>(idx)]);
}


/// returns the cont range
const TextRange& MultiLineScopedTextRangeSet::constRange(size_t idx) const
{
    Q_ASSERT(idx < rangeCount());
    return *(scopedRangeList_[static_cast<qsizetype>(idx)]);
}


/// Adds a range with the default scope
void MultiLineScopedTextRangeSet::addRange(size_t anchor, size_t caret)
{
    scopedRangeList_.append(new MultiLineScopedTextRange(anchor, caret,Edbee::instance()->scopeManager()->refEmptyScope()));
}


///adds the given range
void MultiLineScopedTextRangeSet::addRange(const TextRange& range)
{
    addRange(range.anchor(), range.caret());
}


///' removes the given scope
void MultiLineScopedTextRangeSet::removeRange(size_t idx)
{
    delete scopedRangeList_[static_cast<qsizetype>(idx)];
    scopedRangeList_.removeAt(static_cast<qsizetype>(idx));
}


/// removes all scopes
void MultiLineScopedTextRangeSet::clear()
{
    qDeleteAll(scopedRangeList_);
    scopedRangeList_.clear();
}


void MultiLineScopedTextRangeSet::toSingleRange()
{
    Q_ASSERT(false); ///< NOT IMPLEMENTED
    //scopedRangeList_.remove(1, scopedRangeList_.size()-1);
}


/// Sorts all ranges
void MultiLineScopedTextRangeSet::sortRanges()
{
    std::sort(scopedRangeList_.begin(), scopedRangeList_.end(), MultiLineScopedTextRange::lessThan);
}


/// this method returns the scoped range
MultiLineScopedTextRange& MultiLineScopedTextRangeSet::scopedRange(size_t idx)
{
    Q_ASSERT(0 <= idx && idx < rangeCount());
    return *(scopedRangeList_[static_cast<qsizetype>(idx)]);
}


/// Adds a textrange with the given name
MultiLineScopedTextRange& MultiLineScopedTextRangeSet::addRange(size_t anchor, size_t caret, const QString& name, TextGrammarRule* rule)
{
    MultiLineScopedTextRange* tr = new MultiLineScopedTextRange(anchor, caret, Edbee::instance()->scopeManager()->refTextScope(name) );
    tr->setGrammarRule(rule);
    scopedRangeList_.append(tr);
    return *tr;
}


/// Removes all ranges after a given offset. This means it will remove all
/// complete ranges after the given offset. Ranges that start before the offset and
/// end after the offset are 'invalidated' which means the end offset is placed to the end of the document
void MultiLineScopedTextRangeSet::removeAndInvalidateRangesAfterOffset(size_t offset)
{
    size_t len = textDocument()->length();
    beginChanges();
    for (size_t idx = rangeCount() - 1; idx != std::string::npos; --idx) {
        TextRange& range = this->range(idx);
        if (range.caret() >= offset && range.anchor() >= offset) {
            removeRange(idx);
        } else if (range.min() < offset && range.max() >= offset) {
            range.maxVar() = len;   // move the marker to the end
        }
    }
    endChangesWithoutProcessing();  // we only deleted the last range. Do the result is still sorted
}


/// Gives the scoped text range to this object
void MultiLineScopedTextRangeSet::giveScopedTextRange(MultiLineScopedTextRange* textScope)
{
    scopedRangeList_.append(textScope);
}


/// Process the changes if required
void MultiLineScopedTextRangeSet::processChangesIfRequired(bool joinBorders)
{
    Q_UNUSED(joinBorders);
    if (!changing_) {
        ++changing_; // prevent changing by functions below:
        //mergeOverlappingRanges(joinBorders);
        sortRanges();
        --changing_;
    }
}


/// convert the found ranges to strings
QString MultiLineScopedTextRangeSet::toString()
{
    QString result;
    for (size_t i = 0, cnt = rangeCount(); i < cnt; ++i) {
        MultiLineScopedTextRange& range = scopedRange(i);
        result.append(range.toString());
        result.append("|");
    }
    return result;
}


//===========================================


/// The constructor for storing/managing the textdocument scopes
/// @param textDocument the textdocument this scoping is for
TextDocumentScopes::TextDocumentScopes(TextDocument* textDocument)
    : textDocumentRef_(textDocument)
    , defaultScopedRange_(0, 0, Edbee::instance()->scopeManager()->refTextScope("text.plain"))
    , scopedRanges_(textDocument, this)
    , lastScopedOffset_(0)
{
    connect(textDocument, SIGNAL(languageGrammarChanged()), this, SLOT(grammarChanged()));
}


/// The destructor
TextDocumentScopes::~TextDocumentScopes()
{
    for (size_t i = 0, cnt = lineRangeList_.length(); i < cnt; ++i) {
        delete lineRangeList_.at(i);
    }
    lineRangeList_.clear();
}


/// returns the last scoped offset
size_t TextDocumentScopes::lastScopedOffset()
{
    return lastScopedOffset_;
}


/// Sets the last scoped offset
/// @param offset the last scoped offset
void TextDocumentScopes::setLastScopedOffset(size_t offset)
{
    size_t previousOffset = lastScopedOffset_ ;
    if (previousOffset != offset) {
        lastScopedOffset_ = offset;
        emit lastScopedOffsetChanged(previousOffset, lastScopedOffset_);
    }
}


/// Sets the default scope
/// @param the name of the scope
/// @param rule the rule that matched
void TextDocumentScopes::setDefaultScope(const QString& name, TextGrammarRule* rule )
{
    defaultScopedRange_.setScope( Edbee::instance()->scopeManager()->refTextScope(name) );
    defaultScopedRange_.setGrammarRule(rule);
}


/// Sets the scoped line list
/// @param line the line
/// @param list the list with all scopes on the given line
void TextDocumentScopes::giveLineScopedRangeList(size_t line, ScopedTextRangeList* list)
{
    size_t len = lineRangeList_.length();
    if (line >= len) {
        lineRangeList_.fill(len, 0, 0, line - len + 1);
    }
    delete lineRangeList_.at(line); // delete a possible old value
    lineRangeList_.set(line, list);
}



/// Returns all scoped ranges on the given line
/// @param line the line to retrieve the scoped ranges for
/// @return the scoped textrange list
ScopedTextRangeList* TextDocumentScopes::scopedRangesAtLine(size_t line)
{
    if (line >= lineRangeList_.length() || line < 0) { return nullptr; }
    return lineRangeList_.at(line);
}


/// Returns the number of scopes lines in the lineRangeList_
size_t TextDocumentScopes::scopedLineCount()
{
    return lineRangeList_.length();
}


/// gives the multi-lined textrange to the scopedranges
void TextDocumentScopes::giveMultiLineScopedTextRange(MultiLineScopedTextRange *range)
{
    scopedRanges_.giveScopedTextRange(range);
}


/// Invalidates all scopes after the given offset
/// @param offset the offset from which to remove the offset
void TextDocumentScopes::removeScopesAfterOffset(size_t offset)
{
    if (offset == 0) {
        scopedRanges_.clear();
    } else {
        scopedRanges_.removeAndInvalidateRangesAfterOffset(offset);
    }
    if(offset < lastScopedOffset_) {
        setLastScopedOffset(offset);
    }

    // delete/remove all line ranges (after this line)
    size_t line = this->textDocument()->lineFromOffset(offset) + 1;
    if (line < lineRangeList_.length()) {
        for(size_t i = line, cnt = lineRangeList_.length(); i < cnt; ++i) {
            delete lineRangeList_.at(i);
        }
        lineRangeList_.replace(line, lineRangeList_.length() - line, 0, 0);
    }
}


/// Retursn the default scoped textrange
/// Currently this is done very dirty, by retrieving the defaultscoped range the begin and end is set tot he complete document
/// a better solution would be a subclass that always returns 0 for an anchor and the documentlength for the caret
MultiLineScopedTextRange& TextDocumentScopes::defaultScopedRange()
{
    defaultScopedRange_.set(0, textDocument()->length());   // make sure this is always set like this
    return defaultScopedRange_;
}


/// Returns all scope-ranges at the given offset-ranges
QVector<MultiLineScopedTextRange*> TextDocumentScopes::multiLineScopedRangesBetweenOffsets(size_t offsetBegin, size_t offsetEnd)
{
    QVector<MultiLineScopedTextRange*> result;
    result.append( &defaultScopedRange_);
    size_t minOffset = 0;
    for (size_t i = 0, cnt = scopedRanges_.rangeCount(); i < cnt && minOffset <= offsetEnd; ++i) {
        MultiLineScopedTextRange& range = scopedRanges_.scopedRange(i);
        minOffset = range.min();
        size_t maxOffset = range.max();
        if ((offsetBegin <= minOffset && minOffset < offsetEnd) || (minOffset <= offsetBegin && offsetBegin < maxOffset)) {
            result.append(&range);
        }
    }
    return result;
}


/// returns all scopes between the given offsets
TextScopeList TextDocumentScopes::scopesAtOffset(size_t offset, bool includeEnd)
{
    TextScopeList result;

    size_t line = textDocument()->lineFromOffset(offset);
    size_t offsetInLine = offset-textDocument()->offsetFromLine(line);
    ScopedTextRangeList* list = scopedRangesAtLine(line);
    if (list) {
        //scopes.reserve( ranges.size() );
        for (size_t i = 0, cnt = list->size(); i < cnt; ++i) {
            ScopedTextRange* range = list->at(i);
            if (range->min() <= offsetInLine) {
                if (offsetInLine < range->max() || (includeEnd && offsetInLine <= range->max())) {
                   result.append(range->scope());
                }
            }
        }
    }
    return result;
}


/// Returns all scoped ranges at the given offset
/// Hmmm this is almost exactly the same implementation as the scopesAtOffset method !? (perhaps we should refactor this)
///
/// Warning you MUST destroy (qDeleteAll) the list with scoped textranges returned by this list
///
/// @param offset he offset to retrieve the scoped ranges
/// @return the vector with text scopes. These scopes are document wide
QVector<ScopedTextRange*> TextDocumentScopes::createScopedRangesAtOffsetList(size_t offset)
{
    QVector<ScopedTextRange*> result;

    // retrieve the line
    size_t line = textDocument()->lineFromOffset(offset);
    size_t lineOffset = textDocument()->offsetFromLine(line);
    size_t offsetInLine = offset - lineOffset;

    ScopedTextRangeList* list = scopedRangesAtLine(line);
    if (list) {
        for (size_t i = 0, cnt = list->size(); i < cnt; ++i) {
            ScopedTextRange* range = list->at(i);
            if (range->min() <= offsetInLine && offsetInLine < range->max()) {

                // it's a multi-line scope reference
                MultiLineScopedTextRange* ms = range->multiLineScopedTextRange();
                if (ms) {
                    result.append(new ScopedTextRange(ms->min(), ms->max(), ms->scope()));

                // it's a line scope
                } else {
                    result.append(new ScopedTextRange(lineOffset + range->min(), lineOffset + range->max(), range->scope()));
                }
            }
        }
    }
    return result;
}


/// Converts the textdocument scoped to a string
QString TextDocumentScopes::toString()
{
    return scopedRanges_.toString();
}


/// Returns a string-list will all scopes. This list is mainly used for debugging and testing
QStringList TextDocumentScopes::scopesAsStringList()
{
    QStringList result;

    // first add all multi-line scopes
    for (size_t i = 0, cnt = scopedRanges_.rangeCount(); i < cnt; ++i) {
        MultiLineScopedTextRange& range = scopedRanges_.scopedRange(i);
        result.append(range.toString());
    }

    result.append("**");

    // next add all line based scoped
    for(size_t i = 0, lineCnt = lineRangeList_.length(); i < lineCnt; ++i) {
        ScopedTextRangeList* list = lineRangeList_.at(i);
        if (list != 0) {
            result.append(list->toString());
        } else {
            result.append(QStringLiteral(" << null value @ %1>>").arg(i));
        }
    }
    return result;
}


/// add all dumped line scopes
void TextDocumentScopes::dumpScopedLineAddresses(const QString& text)
{
    qlog_info()<< "dumpScopedLineAddresses("<< text << "): " << lineRangeList_.length();
    for (size_t i = 0, cnt = lineRangeList_.length(); i < cnt; ++i) {
        qlog_info() << "-" << i << ":" << QString::number((quintptr)lineRangeList_.at(i),16);
    }
    qlog_info() << ".";
}


/// returns the current textdocument scope
TextDocument*TextDocumentScopes::textDocument()
{
    return textDocumentRef_;
}


/// the grammar has been changed
void TextDocumentScopes::grammarChanged()
{
    removeScopesAfterOffset(0);
}

} // edbee
