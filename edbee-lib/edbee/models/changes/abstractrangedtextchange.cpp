/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "abstractrangedtextchange.h"

#include "debug.h"

namespace edbee {

/// default destructor is empty
AbstractRangedTextChange::~AbstractRangedTextChange()
{
}


/// Adds the given amount to the offset
/// @param amount the offset to add
void AbstractRangedTextChange::addOffset(int amount)
{
    setOffset( offset() + amount );
}


/// Calculates the merged length
/// @param change the change that't being merged
int AbstractRangedTextChange::getMergedLength(AbstractRangedTextChange* change)
{
    int result = change->newLength();

// add the prefix of the a length
    if( offset() < change->offset() ) { // && b->offset() < (offset() + length())   ) {
        result += ( change->offset()-offset() );
    }

    // add the postfix of the length
    int aEnd = offset() + newLength();
    int bEnd = change->offset() + change->oldLength();
    if( bEnd < aEnd ) {
        result += aEnd - bEnd;
    }
    return result;
}


/// Calculates the merge data size, that's required for merging the given change
/// @param change the change to merge with this change
/// @return the size of this change
int AbstractRangedTextChange::calculateMergeDataSize(AbstractRangedTextChange* change)
{
    int result = 0;

    // we first need to 'take' the leading part of the new change
    if( change->offset() < offset() ) {
        result += offset() - change->offset();
    }

    // we need to add the old data
    result += oldLength();

    // then we need to append the remainer
    int delta = offset()-change->offset();
    int remainerOffset = newLength() + delta;
    if( 0 <= remainerOffset && remainerOffset < change->oldLength()  ) {
        result += change->oldLength() - remainerOffset;
    }
    return result;
}


/// This method merges the data. I really don't like this way of merging the data.
/// though this is a way which support all kinds of arrays. I cannot use inheritance, because the types like QString don't have a common ancestor
/// @param targetData the target of the data, (be sure enough space is reserved!!
/// @param data pointer to the data of this change
/// @param changeData pointer to the of the other change data
/// @param change the other change of the data to merge
/// @param itemSize the size of single item
void AbstractRangedTextChange::mergeData(void* targetData, void* data, void* changeData, AbstractRangedTextChange* change, int itemSize )
{
    char* target = (char*)targetData;

    // we first need to 'take' the leading part of the new change
    if( change->offset() < offset() ) {
        int size = itemSize * (offset() - change->offset() );
        memcpy( target, changeData, size );
        target += size;
    }

    // copy the other text
    memcpy( target, data, itemSize * oldLength() );
    target += itemSize * oldLength() ;

    // then we need to append the remainer
    int delta = offset()-change->offset();
    int remainerOffset = newLength() + delta;
    if( 0 <= remainerOffset && remainerOffset < change->oldLength()  ) {
        memcpy( target, ((char*)changeData) + (remainerOffset*itemSize), (change->oldLength()-remainerOffset) * itemSize  );
    }
}


/// This method checks if this textchange is overlapped by the second text change
/// overlapping is an exclusive overlap, which means the changes are really on top of eachother
/// to test if the changes are touching use isTouchedBy
/// @param secondChange the other change to compare it to
/// @return tue if the changes overlap
bool AbstractRangedTextChange::isOverlappedBy(AbstractRangedTextChange* secondChange)
{
    return
    ( offset() < ( secondChange->offset() + secondChange->newLength() ) &&  secondChange->offset() < (offset() + newLength()) )
    || (  offset() < ( secondChange->offset() + secondChange->oldLength() ) && secondChange->offset() < (offset() + newLength())  )
    ;
}


/// Touched ranges are ranges that are next to eachother
/// Touching means the end offset of one range is the start offset of the other range
/// @param secondChange the other change to match
/// @return true if the changes overlap
bool AbstractRangedTextChange::isTouchedBy(AbstractRangedTextChange* secondChange)
{
    return offset() == (secondChange->offset() + secondChange->newLength() )
        || (offset() + newLength()) == secondChange->offset()
        // Delete operation should be supported
        || ( secondChange->newLength() < secondChange->oldLength() &&  offset() == (secondChange->offset() + secondChange->oldLength() ) )
        // delete touch (Should we add those length < newlength condition!??)
        //|| ( ( length() < newLength() ) && (offset() + newLength()) == secondChange->offset() )
            ;
}


} // edbee
