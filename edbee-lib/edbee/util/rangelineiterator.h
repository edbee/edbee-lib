// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"


namespace edbee {


class TextDocument;
class TextRange;

/// Implements a line iterator for a single range
/// It iterates over all affected lines that are inside the given textrange
///
/// Warning you should not change the document in such way that the linecount changes
/// Doing this will result in incorrect behavior of this iterator
///
/// Usage sample:
/// @code{.cpp}
///
/// RangeLineIterator itr( &controller->textSelection().range(0) )
/// while( itr.hasNext() ) {
///     qDebug() << "Line: " << itr.next();
/// }
///
/// @endcode
class EDBEE_EXPORT RangeLineIterator {
public:
    RangeLineIterator(TextDocument* doc, const TextRange& range);
    RangeLineIterator(TextDocument* doc, size_t start, size_t end);

    bool hasNext() const;
    size_t next();

private:
    size_t curLine_;              ///< The current line number
    size_t endLine_;              ///< The last line
};


} // edbee
