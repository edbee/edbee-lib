// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include "edbee/models/change.h"

namespace edbee {

/// This is an abstract class for ranged changes
/// This are changes (text changes and line changes) that span a range in an array.
/// These ranges share a common alogrithm for performing merges, detecting overlaps etc.
class EDBEE_EXPORT AbstractRangedChange : public Change
{
public:
    virtual ~AbstractRangedChange();

    /// Returns the offset of the change
    virtual size_t offset() const = 0;

    /// Set the offset
    virtual void setOffset(size_t value) = 0;
    void addOffset(size_t amount);

    /// Set the old length
    virtual void setDocLength(size_t value) = 0;

    /// Return the length in the document
    virtual size_t docLength() const = 0;

    /// Returns the length of this item in memory
    virtual size_t storedLength() const = 0;

protected:

    /// implement this method to merge to old data. Sample implementation
    ///
    /// SingleTextChange* singleTextChange = dynamic_cast<SingleTextChange*>(change);
    /// QString newText;
    /// newText.resize( calculateMergeDataSize( change) );
    /// mergeData( newText.data(), text_.data(), singleTextChange->text_.data(), change, sizeof(QChar) );
    virtual void mergeStoredData(AbstractRangedChange* change) = 0;

    size_t getMergedDocLength(AbstractRangedChange* change);
    size_t getMergedStoredLength(AbstractRangedChange* change);
    void mergeStoredDataViaMemcopy(void* targetData, void* data, void* changeData, AbstractRangedChange* change, size_t itemSize);
    bool merge(AbstractRangedChange* change);

public:
    bool isOverlappedBy( AbstractRangedChange* secondChange );
    bool isTouchedBy( AbstractRangedChange* secondChange );
};


} // edbee
