// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "treesittertextdocumentscopes.h"

#include <math.h>

#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"
#include "edbee/edbee.h"
#include "edbee/util/regexp.h"

#include "edbee/debug.h"

namespace edbee {

/// Simple Range iterator
// class TreeSitterScopedTextRangeIteratorStrategy
// {
// public:
//     TreeSitterScopedTextRangeIteratorStrategy(TreeSitterTextDocumentScopes* scopes, size_t index = 0, size_t endIndex = std::string::npos)
//         : scopesRef_(scopes)
//         , index_(index)
//         , endIndex_(endIndex == std::string::npos ? scopes->length() : endIndex)
//     {
//         Q_ASSERT(endIndex <= scopes->length());
//         Q_ASSERT(index <= endIndex);
//     }
//
//     ScopedTextRange* next()
//     {
//         if (index_ >= endIndex_) {
//             return nullptr;
//         }
//         return scopesRef_->at(index_++);
//     }
//
//
// private:
//     TreeSitterTextDocumentScopes* scopesRef_;
//     size_t index_;
//     size_t endIndex_;
// };

class TreeSitterScopedTextRangeOffsetRangeIteratorStrategy : public ScopedTextRangeIteratorStrategy
{
public:
    TreeSitterScopedTextRangeOffsetRangeIteratorStrategy(TreeSitterTextDocumentScopes* scopes, size_t offsetStart, size_t offsetEnd)
        : scopesRef_(scopes)
        , range_(offsetStart, qMax(offsetStart, offsetEnd - 1))
        // , offsetStart_(offsetStart)
        // , offsetEnd_(offsetEnd)
        , index_(0)
    {
// qDebug()<< "BEGIN(itr) -" << scopesRef_->length() << " | " << range_.toString();
//     for(size_t i = 0; i < scopesRef_->length(); ++i) {
//         ScopedTextRange* scopedRange = scopesRef_->at(i);
//         qDebug() << "    - " << scopedRange->toString();
//     }
// qDebug() << "     ---";

    }

    ScopedTextRange* next()
    {
        // TODO: Make this more efficient, if we know ranges are sorted
        // or a tree is used, iterating can be much faster
        ScopedTextRange* range = nullptr;
        int count = 0;

        while(index_ < scopesRef_->length()) {
            range = scopesRef_->at(index_++);
            if (range->overlaps(range_)) {
// qDebug() << "  (itr-idx:" <<  index << ") " << range->toString();
                return range;
            }
        }
        return nullptr;
    }


private:
    TreeSitterTextDocumentScopes* scopesRef_;
    TextRange range_;
    // size_t offsetStart_;
    // size_t offsetEnd_;
    size_t index_;
};

//===========================================

TreeSitterTextDocumentScopes::TreeSitterTextDocumentScopes(TextDocument *textDocument)
    : TextDocumentScopes(textDocument)
{
}


TreeSitterTextDocumentScopes::~TreeSitterTextDocumentScopes()
{
    qDeleteAll(scopedTextRangeList_);
    scopedTextRangeList_.clear();
}

/// returns the scoepd range interator at the given  line
ScopedTextRangeIterator TreeSitterTextDocumentScopes::scopedRangeAtLine(size_t line)
{
    size_t offset = textDocument()->offsetFromLine(line);
    size_t offsetNextLine = textDocument()->offsetFromLine(line + 1);

    return ScopedTextRangeIterator(new TreeSitterScopedTextRangeOffsetRangeIteratorStrategy(this, offset, offsetNextLine));
}

TextScopeList TreeSitterTextDocumentScopes::scopesAtOffset(size_t offset, bool includeEnd)
{
    TextScopeList result;

    // size_t line = textDocument()->lineFromOffset(offset);
    // size_t offsetInLine = offset-textDocument()->offsetFromLine(line);

    // scopes.reserve( ranges.size() );
    for (qsizetype i = 0, cnt = scopedTextRangeList_.length(); i < cnt; ++i) {
        ScopedTextRange* range = scopedTextRangeList_.at(i);
        if (range->min() <= offset) {
            if (offset  < range->max() || (includeEnd && offset <= range->max())) {
                result.append(range->scope());
            }
        }
    }
    return result;
}

QVector<ScopedTextRange *> TreeSitterTextDocumentScopes::createScopedRangesAtOffsetList(size_t offset)
{
    QVector<ScopedTextRange*> result;

    for (qsizetype i = 0, cnt = scopedTextRangeList_.length(); i < cnt; ++i) {
        ScopedTextRange* range = scopedTextRangeList_.at(i);
        if (range->min() <= offset) {
            if (offset  < range->max()) {
                result.append(range);
            }
        }
    }
    return result;
}

QString TreeSitterTextDocumentScopes::toString()
{
    return scopesAsStringList().join("\n");
}

QStringList TreeSitterTextDocumentScopes::scopesAsStringList()
{
    QStringList result;
    for (qsizetype i = 0, cnt = scopedTextRangeList_.length(); i < cnt; ++i) {
        ScopedTextRange* range = scopedTextRangeList_.at(i);
        result.append(range->toString());
    }
    return result;
}


void TreeSitterTextDocumentScopes::giveScopedTextRange(ScopedTextRange *range)
{
    scopedTextRangeList_.append(range);
}

void TreeSitterTextDocumentScopes::clear()
{
    for(qsizetype i = 0, cnt = scopedTextRangeList_.length(); i < cnt; ++i) {
        delete scopedTextRangeList_.at(i);
    }
    scopedTextRangeList_.clear();
}

/// Removes the scopes which ares inside the offset range
/// @param offsetBegin the start of the offset range
/// @param offsetEnd the end of the offset range
/// @param startIndex (default 0), can be used to skip a certain offset
void TreeSitterTextDocumentScopes::removeScopesWithinOffsets(size_t offsetBegin, size_t offsetEnd, size_t startIndex)
{
    qDebug() << "removeScopesWithOffsets" << offsetBegin << "-" << offsetEnd;
    if (scopedTextRangeList_.length() <= static_cast<qsizetype>(startIndex)) return;

    for (auto it = scopedTextRangeList_.begin() + static_cast<qsizetype>(startIndex); it != scopedTextRangeList_.end(); ) {
        ScopedTextRange* range = *it;
        size_t min = range->min();
        size_t max = range->max();

        // qDebug() <<  "  ~> range: " << min << "-" << max <<
        qDebug() << range->toString();

        if (
            // (offsetBegin <= range->caret() && range->caret() <= offsetEnd)
            // || (offsetBegin  <= range->anchor() &&  range->anchor() <= offsetEnd)
            (min <= offsetEnd && max >= offsetBegin)
        ) {
            qDebug() << "    [DELETE]";
            delete range;
            it = scopedTextRangeList_.erase(it);  // erase() returns next valid iterator
        } else {
            ++it;
        }
    }
}

void TreeSitterTextDocumentScopes::notifyScopesChanges(size_t offset, size_t endOffset, size_t line, size_t endLine)
{
    emit scopesChanged(offset, endOffset, line, endLine);
}


} // edbee
