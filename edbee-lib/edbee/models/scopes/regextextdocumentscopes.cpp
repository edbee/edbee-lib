// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "regextextdocumentscopes.h"

#include <math.h>

#include "edbee/models/textdocument.h"
#include "edbee/edbee.h"
#include "edbee/util/regexp.h"

#include "edbee/debug.h"

namespace edbee {


class RegexScopedTextRangeAtLineIteratorStrategy : public ScopedTextRangeIteratorStrategy
{
public:
    RegexScopedTextRangeAtLineIteratorStrategy(ScopedTextRangeList* scopedTextRangeList)
        : scopedTextRangeListRef_(scopedTextRangeList)
        , index_(0)   // we start at 1 to skip the text document scope
    {
    }

    ScopedTextRange* next()
    {
        ScopedTextRange* range = nullptr;
        if(index_ < scopedTextRangeListRef_->size()) {
            return scopedTextRangeListRef_->at(index_++);
        }
        return nullptr;
    }

private:
    ScopedTextRangeList* scopedTextRangeListRef_;
    size_t index_;
};

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
void MultiLineScopedTextRange::setGrammarRule(RegexTextGrammarRule* rule)
{
    ruleRef_ = rule;
}


/// Returns the active grammar rule
RegexTextGrammarRule* MultiLineScopedTextRange::grammarRule() const
{
    return ruleRef_;
}


/// Gives the end regular expression
void MultiLineScopedTextRange::giveEndRegExp(RegExp* regExp)
{
    endRegExp_ = regExp;
}


/// returns the end-regular expression
RegExp* MultiLineScopedTextRange::endRegExp()
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
    Q_ASSERT(idx < rangeCount());
    return *(scopedRangeList_[static_cast<qsizetype>(idx)]);
}


/// Adds a textrange with the given name
MultiLineScopedTextRange& MultiLineScopedTextRangeSet::addRange(size_t anchor, size_t caret, const QString& name, RegexTextGrammarRule* rule)
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


/// Removes all multi lined offsets which are starting or ending between these two ranges
void MultiLineScopedTextRangeSet::removeAndInvalidateRangesBetweenOffsets(size_t offsetBegin, size_t offsetEnd)
{
    size_t len = textDocument()->length();
    beginChanges();
    for (size_t idx = rangeCount() - 1; idx != std::string::npos; --idx) {
        TextRange& range = this->range(idx);
        if ((range.caret() >= offsetBegin && range.caret() <= offsetEnd)
            || (range.anchor() >= offsetEnd && range.anchor() <= offsetEnd)) {
            removeRange(idx);
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



RegexTextDocumentScopes::RegexTextDocumentScopes(TextDocument *textDocument)
    : TextDocumentScopes(textDocument)
    , defaultScopedRange_(0, 0, Edbee::instance()->scopeManager()->refTextScope("text.plain"))
    , scopedRanges_(textDocument, this)
    , lastScopedOffset_(0)
{

}

RegexTextDocumentScopes::~RegexTextDocumentScopes()
{
    for (size_t i = 0, cnt = lineRangeList_.length(); i < cnt; ++i) {
        delete lineRangeList_.at(i);
    }
    lineRangeList_.clear();
}

/// returns the last scoped offset
size_t RegexTextDocumentScopes::lastScopedOffset()
{
    return lastScopedOffset_;
}


/// Sets the last scoped offset
/// @param offset the last scoped offset
void RegexTextDocumentScopes::setLastScopedOffset(size_t offset)
{
    size_t previousOffset = lastScopedOffset_ ;
    if (previousOffset != offset) {
        lastScopedOffset_ = offset;
qDebug() << "EMIT LAST SCOPED OFFSET:" << previousOffset << "," <<  lastScopedOffset_;
        // emit lastScopedOffsetChanged(previousOffset, lastScopedOffset_);

        size_t endOffset = textDocument()->length();
        size_t line = textDocument()->lineFromOffset(previousOffset);
        size_t endLine = textDocument()->lineCount();
qDebug() << " new:" << offset << "," <<  endOffset << " | " << line << ",", endLine;
        emit scopesChanged(offset, endOffset, line, endLine);
    }
}

/// Sets the default scope
/// @param the name of the scope
/// @param rule the rule that matched
void RegexTextDocumentScopes::setDefaultScope(const QString& name, RegexTextGrammarRule* rule )
{
    defaultScopedRange_.setScope(Edbee::instance()->scopeManager()->refTextScope(name));
    defaultScopedRange_.setGrammarRule(rule);
}


/// Returns the default scoped textrange
/// Currently this is done very dirty, by retrieving the defaultscoped range the begin and end is set tot he complete document
/// a better solution would be a subclass that always returns 0 for an anchor and the documentlength for the caret
MultiLineScopedTextRange& RegexTextDocumentScopes::defaultScopedRange()
{
    defaultScopedRange_.set(0, textDocument()->length());   // make sure this is always set like this
    return defaultScopedRange_;
}



/// Sets the scoped line list
/// @param line the line
/// @param list the list with all scopes on the given line
void RegexTextDocumentScopes::giveLineScopedRangeList(size_t line, ScopedTextRangeList* list)
{
    size_t len = lineRangeList_.length();
    if (line >= len) {
        lineRangeList_.fill(len, 0, nullptr, line - len + 1);
    }
    delete lineRangeList_.at(line); // delete a possible old value
    lineRangeList_.set(line, list);
}

/// Returns all scoped ranges on the given line
/// Less efficient then the first implementation
///
/// @param line the line to retrieve the scoped ranges for
/// @return the scoped textrange list
ScopedTextRangeIterator RegexTextDocumentScopes::scopedRangeAtLine(size_t line)
{
    if (line >= lineRangeList_.length()) { return ScopedTextRangeIterator(new ScopedTextRangeEmptyIteratorStrategy()); }


    ScopedTextRangeList* textRangeList = lineRangeList_.at(line);
    return ScopedTextRangeIterator(new RegexScopedTextRangeAtLineIteratorStrategy(textRangeList));
}

/// Returns all scoped ranges on the given line
/// @param line the line to retrieve the scoped ranges for
/// @return the scoped textrange list
ScopedTextRangeList* RegexTextDocumentScopes::scopedRangesAtLineDeprecated(size_t line)
{
    if (line >= lineRangeList_.length()) { return nullptr; }
    return lineRangeList_.at(line);
}


/// Returns the number of scopes lines in the lineRangeList_
size_t RegexTextDocumentScopes::scopedLineCount()
{
    return lineRangeList_.length();
}


/// gives the multi-lined textrange to the scopedranges
void RegexTextDocumentScopes::giveMultiLineScopedTextRange(MultiLineScopedTextRange *range)
{
    scopedRanges_.giveScopedTextRange(range);
}


/// Invalidates all scopes after the given offset
/// @param offset the offset from which to remove the offset
void RegexTextDocumentScopes::removeScopesAfterOffset(size_t offset)
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

/// Removes all affected scopes within these offsets
void RegexTextDocumentScopes::removeScopesWithinOffsets(size_t offsetBegin, size_t offsetEnd)
{
    scopedRanges_.removeAndInvalidateRangesBetweenOffsets(offsetBegin, offsetEnd);

    size_t startLine = textDocument()->lineFromOffset(offsetBegin);
    size_t endLine = textDocument()->lineFromOffset(offsetEnd);
    // endLine = qMin(endLine + 1, lineRangeList_.length());

 qDebug() << "removeScopesWithinOffset(" << offsetBegin << ", "  << offsetEnd << ")"
    << " | lines: [" << startLine << ", " << endLine << "], scopes.length" << lineRangeList_.length();
    // for(int i=0, cnt = lineRangeList_.length();  i < cnt; i++){
    //     ScopedTextRangeList* item = lineRangeList_.at(i);
    //     if (item)  {
    //         qDebug() << "-  " << i << ":" << item->toString();
    //     } else {
    //         qDebug() << "-  " << i << ": nullptr";
    //     }
    // }

    // size_t line = this->textDocument()->lineFromOffset(offset) + 1;
// qDebug() << "THIS isn't correct, we only may delete scopes if they fall withing the offset";
// qDebug() << "- for the first and last line, we should NOT delete the offsets that aren't effect by offsetBegin and offsetEnd";
// qDebug() << "";

    size_t startLineOffset = textDocument()->offsetFromLine(startLine);
    size_t endLineOffset = textDocument()->offsetFromLine(endLine);
    size_t localStartOffset = startLineOffset - offsetBegin;
    size_t localEndOffset = endLineOffset - offsetEnd;

qDebug() << "  - globalOffset: " << startLineOffset << " - " << localEndOffset << " => " << " localOffset: " << localStartOffset << " - " << localEndOffset;

    if (startLine == endLine) {
qDebug() << "  a: " << localStartOffset << " - " << localEndOffset;
        ScopedTextRangeList* rangeList = startLine < lineRangeList_.length() ? lineRangeList_.at(startLine) :  nullptr;
        if (rangeList) {
            rangeList->removeWithinOffset(localStartOffset, localEndOffset);
            if (localStartOffset == 0) {
                ScopedTextRange* range = new ScopedTextRange(0, textDocument()->lineLength(endLine), this->defaultScopedRange().scope());
                rangeList->giveAndPrependRange(range);
            }
        }
    } else {
        ScopedTextRangeList* firstRangeList = startLine < lineRangeList_.length() ? lineRangeList_.at(startLine) : nullptr;
        if (firstRangeList) {
qDebug() << "  bb: " << localStartOffset;
            firstRangeList->removeFromOffset(localStartOffset);
            // HACK to add default scope at start (This sucks and shouldn't be required)
            if (localStartOffset == 0) {
                ScopedTextRange* range = new ScopedTextRange(0, textDocument()->lineLength(endLine), this->defaultScopedRange().scope());
                firstRangeList->giveAndPrependRange(range);
            }
        }

        ScopedTextRangeList* lastRangeList = endLine < lineRangeList_.length() ? lineRangeList_.at(endLine) : nullptr;
        if (lastRangeList){
qDebug() << "  be: " << localEndOffset;
            lastRangeList->removeToOffset(localEndOffset);

            // HACK to add default scope at start (This sucks and shouldn't be required)
            ScopedTextRange* range = new ScopedTextRange(0, textDocument()->lineLength(endLine), this->defaultScopedRange().scope());
            lastRangeList->giveAndPrependRange(range);
        }

        // delete the lines in betwee
        if (endLine - startLine > 2) {
            size_t end = qMin(endLine - 1, lineRangeList_.length());
            for(size_t i = startLine + 1; i < end; ++i) {
                delete lineRangeList_.at(i);
            }
            lineRangeList_.replace(startLine + 1, end - startLine - 1, nullptr, 0);
        }
    }
}

/// Returns all scope-ranges at the given offset-ranges
QVector<MultiLineScopedTextRange*> RegexTextDocumentScopes::multiLineScopedRangesBetweenOffsets(size_t offsetBegin, size_t offsetEnd)
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
TextScopeList RegexTextDocumentScopes::scopesAtOffset(size_t offset, bool includeEnd)
{
    TextScopeList result;

    size_t line = textDocument()->lineFromOffset(offset);
    size_t offsetInLine = offset-textDocument()->offsetFromLine(line);
    ScopedTextRangeList* list = scopedRangesAtLineDeprecated(line);
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
QVector<ScopedTextRange*> RegexTextDocumentScopes::createScopedRangesAtOffsetList(size_t offset)
{
    QVector<ScopedTextRange*> result;

    // retrieve the line
    size_t line = textDocument()->lineFromOffset(offset);
    size_t lineOffset = textDocument()->offsetFromLine(line);
    size_t offsetInLine = offset - lineOffset;

    ScopedTextRangeList* list = scopedRangesAtLineDeprecated(line);
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


/// Returns the multilined scoped ranges
MultiLineScopedTextRangeSet& RegexTextDocumentScopes::scopedRanges()
{
    return scopedRanges_;
}


/// Converts the textdocument scoped to a string
QString RegexTextDocumentScopes::toString()
{
    return scopedRanges_.toString();
}


/// Returns a string-list will all scopes. This list is mainly used for debugging and testing
QStringList RegexTextDocumentScopes::scopesAsStringList()
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
void RegexTextDocumentScopes::dumpScopedLineAddresses(const QString& text)
{
    qlog_info()<< "dumpScopedLineAddresses("<< text << "): " << lineRangeList_.length();
    for (size_t i = 0, cnt = lineRangeList_.length(); i < cnt; ++i) {
        qlog_info() << "-" << i << ":" << QString::number((quintptr)lineRangeList_.at(i),16);
    }
    qlog_info() << ".";
}


/// the grammar has been changed
// void RegexTextDocumentScopes::grammarChanged()
// {
//     removeScopesAfterOffset(0);
// }

} // edbee

