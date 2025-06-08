// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "rangesetlineiterator.h"

#include "edbee/models/textdocument.h"
#include "edbee/models/textrange.h"

#include "edbee/debug.h"

namespace edbee {

/// Constructs the rangeset line iterator
RangeSetLineIterator::RangeSetLineIterator(TextRangeSet* rangeSet)
    : rangeSetRef_(rangeSet)
    , rangeIndex_(0)
    , rangeEndLine_(std::string::npos)
    , curLine_(std::string::npos)
{
    findNextLine();
}


/// Checks if there's a next line number available
bool RangeSetLineIterator::hasNext() const
{
    return curLine_ != std::string::npos;
}


/// returns the next line number
size_t RangeSetLineIterator::next()
{
    size_t result = curLine_;
    findNextLine();
    return result;
}


/// finds the next line number
void RangeSetLineIterator::findNextLine()
{
    // current line isn't at the end of the current range
    if (curLine_ != std::string::npos && curLine_ < rangeEndLine_) {
        ++curLine_;

    // the curLine_ is finished with the current range
    } else {
        // when the rangeset is finishedd
        while (rangeIndex_ < rangeSetRef_->rangeCount()) {
            TextRange& range = rangeSetRef_->range(rangeIndex_);
            ++rangeIndex_;

            // get the start/end lines
            size_t startLine = rangeSetRef_->textDocument()->lineFromOffset(range.min());
            size_t endLine = rangeSetRef_->textDocument()->lineFromOffset(range.max());
            if( startLine == curLine_ ) { ++startLine; }

            // still in range?
            if (startLine <= endLine) {
                curLine_ = startLine;
                rangeEndLine_ = endLine;
                return;
            }
        }
        curLine_ = std::string::npos;  // done
    }
}


} // edbee
