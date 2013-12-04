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
