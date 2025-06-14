// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "lineoffsetvector.h"

#include "edbee/models/textbuffer.h"

#include "edbee/debug.h"

namespace edbee {


LineOffsetVector::LineOffsetVector()
    : offsetList_(16)
    , offsetDeltaIndex_(0)
    , offsetDelta_(0)
{
    // make sure there's always line 0 in the buffer
    ptrdiff_t v=0;
    offsetList_.replace( 0, 0, &v, 1);

    assertValid();
}

/// convenient method to apply the (signed) delta to an offset
size_t applyDelta(ptrdiff_t offset, ptrdiff_t delta)
{
    ptrdiff_t result = static_cast<ptrdiff_t>(offset) + delta;
    Q_ASSERT(result >= 0);
    return static_cast<size_t>(result);
}


void LineOffsetVector::applyChange(TextBufferChange change)
{
    ptrdiff_t offsetDelta = static_cast<ptrdiff_t>(change.newTextLength()) - static_cast<ptrdiff_t>(change.length());

    // I assume it is save to cast a size_t to ptrdiff_t
    if (sizeof(size_t) == sizeof(ptrdiff_t)) {
        const ptrdiff_t* offsets = nullptr;
        offsets = reinterpret_cast<const ptrdiff_t*>(change.newLineOffsets().constData());
        applyChange(change.line(), change.lineCount(), change.newLineCount(), offsets, offsetDelta);
    } else {
        ptrdiff_t* offsets = new ptrdiff_t[change.newLineCount()]; // change.newLineOffsets().constData()
        for (qsizetype i = 0, cnt = static_cast<qsizetype>(change.newLineCount()); i < cnt; ++i) {
          offsets[i] = static_cast<const ptrdiff_t>(change.newLineOffsets()[i]);
        }
        applyChange(change.line(), change.lineCount(), change.newLineCount(), offsets, offsetDelta);
        delete[] offsets;
    }
    return;
}

/// applies the change from a TextChangeData
/// @param line the line-index this change is for
/// @param removeineCount the number of lines to remove at line
/// @param addLineCount the new number of lines to add
/// @param newOffsets the new offets that are inserted at the line location (realy offsets! no delta applied!)
/// @param offsetDelta the offset delta applied in this change. (difference in offset between the replaced texts)
void LineOffsetVector::applyChange(size_t line, size_t removeLineCount, size_t addLineCount, const ptrdiff_t* newOffsets, ptrdiff_t offsetDelta)
{
    line += 1; // line 0 may NEVER be replaced!

    // repace the lines
    moveDeltaToIndex(line);

    // I assume it is save to cast a size_t to ptrdiff_t
    // Officially this isn't allowed, so for now we allocate a new buffer
    // and type cast every
    offsetList_.replace(line, removeLineCount, newOffsets, addLineCount);

    // Add the delta to all the lines
    size_t endLine = line + addLineCount;

    if (offsetDeltaIndex_ < endLine) {
        offsetDeltaIndex_ = endLine;
    }

    Q_ASSERT(offsetDeltaIndex_ <= this->length() );

    changeOffsetDelta(endLine, offsetDelta);
    assertValid();

}


/// this method returns the line offset at the given line offset
size_t LineOffsetVector::at(size_t idx) const
{
    Q_ASSERT(idx < length() );
    if (idx >= offsetDeltaIndex_) {
        return applyDelta(offsetList_.at(idx), offsetDelta_);
    }
    return applyDelta(offsetList_.at(idx), 0);
}


size_t LineOffsetVector::length() const
{
    return offsetList_.length();
}


/// this method searches the line from the given offset
size_t LineOffsetVector::findLineFromOffset(size_t offset)
{
    if (offset == 0) return 0;

    size_t offsetListLength = offsetList_.length();
    size_t offsetAtDelta = 0;
    if (offsetDeltaIndex_ < offsetListLength) {
        offsetAtDelta = applyDelta(offsetList_.at(offsetDeltaIndex_), offsetDelta_);
    } else {
        Q_ASSERT(offsetListLength > 0);
        offsetAtDelta = applyDelta(offsetList_.at(offsetListLength - 1), 0);
    }

    // only binary search the left part
    size_t line = 0;
    if (offset < offsetAtDelta) {
        line = searchOffsetIgnoringOffsetDelta(static_cast<ptrdiff_t>(offset), 0, offsetDeltaIndex_);

    // binary search the right part
    } else {
        line = searchOffsetIgnoringOffsetDelta(static_cast<ptrdiff_t>(offset) - offsetDelta_, offsetDeltaIndex_, offsetListLength);
    }

    return line;
}

/// This method appends an offset to the end of the list
/// It simply applies the current offsetDelta because it will always be AFTER the current offsetDelta
void LineOffsetVector::appendOffset(size_t offset)
{
    ptrdiff_t newOffset = static_cast<ptrdiff_t>(offset) -offsetDelta_;
    offsetList_.append(newOffset);
    assertValid();
}


/// This method returns the unitTestString representation => 1,2,[3>3,4
QString LineOffsetVector::toUnitTestString()
{
    QString s;
    for (size_t i = 0, cnt = offsetDeltaIndex_; i < cnt; ++i) {
        if (i != 0) { s.append(","); }
        s.append(QStringLiteral("%1").arg( offsetList_[i]));
    }
    if (offsetDeltaIndex_ > 0) s.append(",");
    s.append( QStringLiteral("[%1>").arg(offsetDelta_) );
    for (size_t i=offsetDeltaIndex_, cnt = offsetList_.length(); i < cnt; ++i) {
        if (i != offsetDeltaIndex_) { s.append(","); }
        s.append(QStringLiteral("%1").arg(offsetList_[i]));
    }
    return s;
}


/// This method returns the unitTestString representaion with the offest removed (1,2,[3>3,4) => 1,2,6,7
QString LineOffsetVector::toUnitTestFilledString()
{
    QString s;
    for (size_t i = 0, cnt = length(); i < cnt; ++i) {
        if (i != 0) { s.append(","); }
        s.append(QStringLiteral("%1").arg(at(i)));
    }
    return s;
}


/// initializes the construct for unit testing
/// @param offsetDelta the offsetDelta to use
/// @param offsetDeltaIndex the offsetDeltaIndex to use
/// @param a list of integer offsets. Close the list with -1 !!!
void LineOffsetVector::initForUnitTesting(ptrdiff_t offsetDelta, size_t offsetDeltaIndex, ... )
{
    offsetDelta_ = offsetDelta;
    offsetDeltaIndex_ = offsetDeltaIndex;
    offsetList_.clear();

    va_list offsets;
    va_start(offsets, offsetDeltaIndex);
    size_t val = va_arg(offsets, size_t);
    while (val != std::string::npos) {
        offsetList_.append(static_cast<ptrdiff_t>(val));
        val = va_arg(offsets, size_t);
    }
    va_end(offsets);

    assertValid();
}


/// This method returns the line from the start position. This method uses
/// a binary search. Warning this method uses RAW values from the offsetList it does NOT
/// take in account the offsetDelta
size_t LineOffsetVector::searchOffsetIgnoringOffsetDelta(ptrdiff_t offset, size_t org_start, size_t org_end )
{
    size_t begin = org_start;
    size_t end   = org_end - 1;

    if (begin == end) return begin;

    size_t half = end;
    ptrdiff_t r = 0;
    while (begin <= end) {
        // BINAIRY SEARCH:
        half = begin + ((end - begin) >> 1);

        // compre the value
        r = offsetList_.at(half) - offset;
        if (r == 0) { return half; }
        if (r < 0) {
            begin = half + 1;
        } else {
            end = half -1;
        }
    }

    if (r > 0) {
        return half - 1;
    } else {
        if (half == org_end) return org_end -1 ;
        return half;
    }
}


/// This method moves the delta to the given index
void LineOffsetVector::moveDeltaToIndex(size_t index)
{
    if (offsetDelta_) {

        // move to the right
        if (offsetDeltaIndex_ < index) {
            for(size_t i = offsetDeltaIndex_; i < index; ++i) {
                offsetList_[i] += offsetDelta_;
            }

        // move to the left
        } else if (index < offsetDeltaIndex_) {

            // In this situation there are 2 solution.
            // (1) Make the delta 0 or (2) apply the 'negative index' to the items on the left
            size_t length            = offsetList_.length();
            size_t delta0cost        = length - offsetDeltaIndex_;
            size_t negativeDeltaCost = offsetDeltaIndex_ - index;

            // prefer delta 0
            if (delta0cost <= negativeDeltaCost) {
                for (size_t i = offsetDeltaIndex_; i < length; ++i) {
                    offsetList_[i] += offsetDelta_;
                }
                offsetDelta_ = 0;
            // else apply the negative delta
            } else {
                for (size_t i = index; i < offsetDeltaIndex_; ++i) {
                    offsetList_[i] -= offsetDelta_;
                }
            }
        }
    }
    offsetDeltaIndex_ = index;

    assertValid();
}


/// This method moves the offset delta to the given location
void LineOffsetVector::changeOffsetDelta(size_t index, ptrdiff_t delta)
{
    Q_ASSERT(index <= length());

    // there are 3 situations.
    // (1) The delta is at the exact location of the previous delta
    if (index == offsetDeltaIndex_) {
        offsetDeltaIndex_ = index;
        offsetDelta_ += delta;

    // (2) The new index is BEFORE the old index.
    } else if (index < offsetDeltaIndex_) {
        // apply the 'negative' offset to the given location
        for (size_t i = index; i < offsetDeltaIndex_; ++i) {
            offsetList_[i] -= (offsetDelta_ + delta);
            // offsetList_[i] -= (offsetDelta_ + delta);
        }

        offsetDeltaIndex_ = index;
        offsetDelta_ += delta;

    // (3) the index is after the old index.
    } else if (offsetDeltaIndex_ < index) {
        // apply the old offsetsDelta to the indices before
        for (size_t i = offsetDeltaIndex_; i < index; ++i) {
            offsetList_[i] += offsetDelta_;
        }
        offsetDeltaIndex_ = index;
        offsetDelta_ += delta;
    }

    // set the delta to 0 when at the end of the line
    if( offsetDeltaIndex_ == length() ) {
        offsetDelta_ = 0;
    }

    assertValid();
}

/// Sets the offset delta and asserts the delta can be changed this way
/// -
void LineOffsetVector::setOffsetDelta(ptrdiff_t delta)
{
    size_t offsetBeforeDelta = offsetDeltaIndex_ > 0 ? applyDelta(offsetList_[offsetDeltaIndex_ -1], 0) : 0;
    size_t offsetAtDelta = applyDelta(offsetList_[offsetDeltaIndex_], delta);
    Q_ASSERT(offsetBeforeDelta < offsetAtDelta);
    this->offsetDelta_ = delta;
}


/// Asserts the data is valid
void LineOffsetVector::assertValid()
{
    Q_ASSERT(offsetDeltaIndex_ >= 0);

    // assert all offsets are always valid
    ptrdiff_t lastOffset = -1;
    for (size_t i = 0; i < offsetDeltaIndex_; ++i) {
        Q_ASSERT(lastOffset < offsetList_[i]);
        lastOffset = offsetList_[i];
    }

    for (size_t i = offsetDeltaIndex_, cnt = offsetList_.length(); i < cnt; ++i) {
        ptrdiff_t offset = offsetList_[i] + offsetDelta_;
        Q_ASSERT(lastOffset < offset);
        lastOffset = offset;
    }
}

} // edbee
