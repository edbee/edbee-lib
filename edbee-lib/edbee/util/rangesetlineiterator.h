// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

namespace edbee {

class TextRangeSet;

/// a rangeset line iterator. For iterating over ALL distinct lines
/// used in a rangeset.
///
/// Usage sample:
/// @code{.cpp}
///
/// RangeSetLineIterator itr(controller->textSelection())
/// while (itr.hasNext()) {
///     qDebug() << "Line: " << itr.next();
/// }
///
/// @endcode
class EDBEE_EXPORT RangeSetLineIterator {
public:
    RangeSetLineIterator(TextRangeSet* rangeSet);

    bool hasNext() const;
    size_t next();

private:
    void findNextLine();

private:
    TextRangeSet* rangeSetRef_;     ///< a reference to the range sets
    size_t rangeIndex_;             ///< the current range index
    size_t rangeEndLine_;           ///< the last line of the current range (std::string::npos when no end is known)
    size_t curLine_;                ///< The current line number (this value is std::string::npos if there's no current line)


};

} // edbee
