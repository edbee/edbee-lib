// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "lineoffsetvector.h"

#include "edbee/models/textbuffer.h"

#include "edbee/debug.h"

namespace edbee {


LineOffsetVector::LineOffsetVector()
    : offsetList_(16)
    , offsetDelta_(0)
    , offsetDeltaIndex_(0)
{
    // make sure there's always line 0 in the buffer
    size_t v = 0;
    offsetList_.replace(0, 0, &v, 1);
}


void LineOffsetVector::applyChange(TextBufferChange change)
{
    size_t line = change.line() + 1;    // line 0 may NEVER be replaced!

    // repace the lines
    moveDeltaToIndex( line );

    offsetList_.replace(line, change.lineCount(), change.newLineOffsets().constData(), change.newLineCount());

    // Add the delta to all the lines
    int endLine = line + change.newLineCount();

    if( offsetDeltaIndex_ < endLine ) {
        offsetDeltaIndex_ = endLine;
    }

    Q_ASSERT(offsetDeltaIndex_ <= this->length() );

    changeOffsetDelta(endLine, change.newTextLength() - change.length());
}



/*
/// This method is should be called when text is replaced. This method
/// updates all elements of the split vector and calculates all offsets
/// @param offset the offset to change
/// @param length the number of chars that are replaced
/// @param text the new text
void LineOffsetVector::textReplaced(int offset, int length, const QChar *newData , int newDataLength)
{
    LineChange change;
    prepareChange(offset,length, newData, newDataLength, &change);
    applyChange( &change );

}
*/

/// this method returns the line offset at the given line offset
size_t LineOffsetVector::at(size_t idx) const
{
    Q_ASSERT(idx < length());
    if (idx >= offsetDeltaIndex_) {
        return offsetList_.at(idx) + offsetDelta_;
    }
    return offsetList_.at(idx);
}

size_t LineOffsetVector::length() const
{
    return offsetList_.length();
}


/// this method searches the line from the given offset
size_t LineOffsetVector::findLineFromOffset(size_t offset)
{
    if( offset == 0 ) return 0;
    size_t offsetListLength = offsetList_.length();
    size_t offsetAtDelta = 0;
    if( offsetDeltaIndex_ < offsetListLength ) {
        offsetAtDelta = offsetList_.at(offsetDeltaIndex_) + offsetDelta_;
    } else {
        offsetAtDelta = offsetList_.at( offsetListLength - 1);
    }

    // only binary search the left part
    size_t line = 0;
    if (offset < offsetAtDelta) {
        line = searchOffsetIgnoringOffsetDelta(offset, 0, offsetDeltaIndex_);

    // binary search the right part
    } else {
        line = searchOffsetIgnoringOffsetDelta(offset - offsetDelta_, offsetDeltaIndex_, offsetListLength);
    }
    return line;

}

/// This method appends an offset to the end of the list
/// It simply applies the current offsetDelta because it will always be AFTER the current offsetDelta
void LineOffsetVector::appendOffset(size_t offset)
{
    this->offsetList_.append(offset - offsetDelta_);
}

/// This method returns the offset to string
QString LineOffsetVector::toUnitTestString()
{
    QString s;
    for( int i=0,cnt=offsetDeltaIndex_; i<cnt; ++i ) {
        if( i != 0 ) { s.append(","); }
        s.append( QStringLiteral("%1").arg( offsetList_[i] ) );
    }
    s.append( QStringLiteral("[%1>").arg( offsetDelta_ ) );
    for( int i=offsetDeltaIndex_,cnt=offsetList_.length(); i<cnt; ++i ) {
        if( i != offsetDeltaIndex_ ) { s.append(","); }
        s.append( QStringLiteral("%1").arg( offsetList_[i] ) );
    }
    return s;
}

/// initializes the construct for unit testing
/// @param offsetDelta the offsetDelta to use
/// @param offsetDeltaIndex the offsetDeltaIndex to use
/// @param a list of integer offsets. Close the list with -1 !!!
void LineOffsetVector::initForUnitTesting(size_t offsetDelta, size_t offsetDeltaIndex, ... )
{
    offsetDelta_ = offsetDelta;
    offsetDeltaIndex_ = offsetDeltaIndex;
    offsetList_.clear();

    va_list offsets;
    va_start(offsets, offsetDeltaIndex);
    size_t val = va_arg(offsets, size_t);
    while (val != std::string::npos) {
        offsetList_.append(val);
        val = va_arg(offsets, size_t);
    }
    va_end(offsets);
}



/// Returns the line from the start position. This method uses a binary search.
/// Warning this method uses RAW values from the offsetList it does NOT
/// take in account the offsetDelta
size_t LineOffsetVector::searchOffsetIgnoringOffsetDelta(size_t offset, size_t org_start, size_t org_end)
{
    size_t begin = org_start;
    size_t end   = org_end - 1;

    if (begin == end) return begin;

    size_t half = end;
    size_t offsetAtHalf = 0;
    while( begin <= end  ) {

        // BINAIRY SEARCH:
        half = begin + ((end-begin)>>1);

        // compare the value
        offsetAtHalf = offsetList_.at(half);

        if (offsetAtHalf == offset) {
           return half;
        }
        if ( offsetAtHalf < offset ) {
            begin = half + 1;
        } else {
            end = half -1;
        }
    }

    if (offsetAtHalf > offset) {
        return half - 1;
    } else {
        if( half == org_end ) return org_end - 1;
        return half;
    }
}

/// <oves the delta to the given index
void LineOffsetVector::moveDeltaToIndex(size_t index)
{
    if (offsetDelta_) {

        // move to the right
        if (offsetDeltaIndex_ < index) {
            for (int i=offsetDeltaIndex_; i < index; ++i) {
                offsetList_[i] += offsetDelta_; // apply the delta
            }
        // move to the left
        } else if (index < offsetDeltaIndex_) {

            // In this situation there are 2 solution.
            // (1) Make the delta 0 or (2) apply the 'negative index' to the items on the left
            size_t length         = offsetList_.length();
            size_t delta0cost     = length - offsetDeltaIndex_;
            size_t negativeDeltaCost = offsetDeltaIndex_ - index;

            // prefer delta 0
            if (delta0cost <= negativeDeltaCost) {
                for (size_t i=offsetDeltaIndex_; i < length; ++i) {
                    offsetList_[i] += offsetDelta_;
                }
                offsetDelta_ = 0;
            // else apply the negative delta
            } else {
                for (size_t i=index; i < offsetDeltaIndex_; ++i) {
                    offsetList_[i] -= offsetDelta_;
                }
            }
        }
    }
    offsetDeltaIndex_ = index;

}

/// This method moves the offset delta to the given location
void LineOffsetVector::changeOffsetDelta(size_t index, std::ptrdiff_t delta)
{
    // there are 3 situations.
    // (1) The delta is at the exact location of the previous delta
    if (index == offsetDeltaIndex_) {
        offsetDeltaIndex_ = index;
        offsetDelta_ += delta;

    // (2) The new index is BEFORE the old index.
    } else if (index < offsetDeltaIndex_) {

qDebug() << "changeOFfsetDelta(" << index << ", " << delta <<")";
qDebug() << "  => " << this->toUnitTestString();
        // apply the 'negative' offset to the given location
        for (size_t i=index; i < offsetDeltaIndex_; ++i) {

            // negative offset values are strange!
            if (static_cast<size_t>(offsetDelta_ + delta) > offsetList_[i] ) {
                Q_ASSERT(false && "Negative offset values aren't allowed");
                offsetList_[i] = 0;
            } else {
                offsetList_[i] -= static_cast<size_t>(offsetDelta_ + delta);
            }
qDebug() << "   -= (" << offsetDelta_ << " + " << delta << "), (" << (offsetDelta_ + delta) << ")";
qDebug() << "   => " << offsetList_[i];
        }
qDebug() << "-- (offsetDelta_ = "<< offsetDelta_ <<", offsetDeltaIndex_=" << offsetDeltaIndex_ <<", delta=" << delta<<")";
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
    if (offsetDeltaIndex_ == length()) {
        offsetDelta_ = 0;
    }

}


} // edbee
