// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "abstractrangedchange.h"

#include "edbee/debug.h"

namespace edbee {

/// default destructor is empty
AbstractRangedChange::~AbstractRangedChange()
{
}


/// Adds the given amount to the offset
/// @param amount the offset to add
void AbstractRangedChange::addOffset(ptrdiff_t amount)
{
    ptrdiff_t newOffset = static_cast<ptrdiff_t>(offset()) + amount;
    Q_ASSERT(newOffset >= 0);
    setOffset(static_cast<size_t>(newOffset));
}


/// Calculates the merged length
/// @param change the change that't being merged
size_t AbstractRangedChange::getMergedDocLength(AbstractRangedChange* change)
{
    size_t result = change->docLength();

    // add the prefix of the a length
    if (offset() < change->offset()) {
        result += (change->offset()-offset());
    }

    // add the postfix of the length
    size_t aEnd = offset() + docLength();
    size_t bEnd = change->offset() + change->storedLength();
    if (bEnd < aEnd) {
        result += aEnd - bEnd;
    }
    return result;
}


/// Calculates the merge data size, that's required for merging the given change
/// @param change the change to merge with this change
/// @return the size of this change
size_t AbstractRangedChange::getMergedStoredLength(AbstractRangedChange* change)
{
    size_t result = 0;

    // we first need to 'take' the leading part of the new change
    if( change->offset() < offset() ) {
        result += offset() - change->offset();
    }

    // we need to add the old data
    result += storedLength();

    // then we need to append the remainer

    // int delta = offset() - change->offset();
    // int remainerOffset = docLength() + delta;
    size_t remainerOffset = docLength();
    if (offset() > change->offset()) {
        remainerOffset += offset() - change->offset();
    } else {
        remainerOffset -= change->offset() - offset();
    }

    if (remainerOffset < change->storedLength()) {
        result += change->storedLength() - remainerOffset;
    }
    return result;
}


/// Per forms a memcopy or zero fills the given area
static void memcopy_or_zerofill(void* target, void* source, size_t size)
{
    if (source) {
        memcpy(target, source, size);
    } else {
        memset(target, 0, size);
    }
}


/// This method merges the data via a memcopy. I really don't like this way of merging the data.
/// though this is a way which support all kinds of arrays. I cannot use inheritance, because the types like QString don't have a common ancestor
///
/// @param targetData the target of the data, (be sure enough space is reserved!!
/// @param data pointer to the data of this change (this can be a 0 pointer!, which results in 0-filling the target)
/// @param changeData pointer to the of the other change data (this can be a 0 pointer!, which results in 0-filling the target)
/// @param change the other change of the data to merge
/// @param itemSize the size of single item
void AbstractRangedChange::mergeStoredDataViaMemcopy(void* targetData, void* data, void* changeData, AbstractRangedChange* change, size_t itemSize )
{
    char* target = (char*)targetData;

    // we first need to 'take' the leading part of the new change
    if (change->offset() < offset()) {
        size_t size = itemSize * (offset() - change->offset());
        memcopy_or_zerofill( target, changeData, size );
        target += size;
    }

    // copy the other text
    memcopy_or_zerofill(target, data, itemSize * storedLength());
    target += itemSize * storedLength() ;

    // then we need to append the remainer

    // int delta = offset() - change->offset();
    // int remainerOffset = docLength() + delta;
    size_t remainerOffset = docLength();
    if (offset() > change->offset()) {
        remainerOffset += offset() - change->offset();
    } else {
        remainerOffset -= change->offset() - offset();
    }

    if (remainerOffset < change->storedLength()) {
        memcopy_or_zerofill(target, changeData ? ((char*)changeData) + (remainerOffset * itemSize) : 0, (change->storedLength() - remainerOffset) * itemSize);
    }
}


/// merges a changes
/// @param document the document to merges
/// @param change the change change to merge
bool AbstractRangedChange::merge(AbstractRangedChange* change)
{
    // overlap is a bit harder
    if (isOverlappedBy(change) || isTouchedBy(change)) {

        // build the new sizes and offsets
        size_t newOffset = qMin(offset(), change->offset());
        size_t newLength = getMergedDocLength(change);

        // merge the data
        mergeStoredData(change);

        // when the data is meged assign the new dimensions
        setDocLength(newLength);
        setOffset(newOffset);
        delete change;
        return true;
    }
    return false;
}


/// Checks if this textchange is overlapped by the second textchange
/// overlapping is an exclusive overlap, which means the changes are really on top of eachother
/// to test if the changes are touching use isTouchedBy
/// @param secondChange the other change to compare it to
/// @return tue if the changes overlap
bool AbstractRangedChange::isOverlappedBy(AbstractRangedChange* secondChange)
{
    return
    (offset() < (secondChange->offset() + secondChange->docLength()) && secondChange->offset() < (offset() + docLength()))
      || (offset() < (secondChange->offset() + secondChange->storedLength()) && secondChange->offset() < (offset() + docLength()))
    ;
}


/// Touched ranges are ranges that are next to eachother
/// Touching means the end offset of one range is the start offset of the other range
/// @param secondChange the other change to match
/// @return true if the changes overlap
bool AbstractRangedChange::isTouchedBy(AbstractRangedChange* secondChange)
{
    return offset() == (secondChange->offset() + secondChange->docLength())
        || (offset() + docLength()) == secondChange->offset()
        // Delete operation should be supported
        || (secondChange->docLength() < secondChange->storedLength() &&  offset() == (secondChange->offset() + secondChange->storedLength()))
        // delete touch (Should we add those length < newlength condition!??)
        //|| ( ( length() < newLength() ) && (offset() + newLength()) == secondChange->offset() )
            ;
}


} // edbee
