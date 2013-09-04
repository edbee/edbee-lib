/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "linedatalisttextchange.h"

#include "edbee/models/textdocument.h"
#include "edbee/models/textlinedata.h"

#include "debug.h"

namespace edbee {

class TextLineDataManager;


LineDataListTextChange::LineDataListTextChange( TextLineDataManager* manager, int line, int length, int newLength )
    : managerRef_(manager)
    , line_(line)
    , length_(length)
    , newLength_(newLength)
    , oldListList_(0)
{
}

LineDataListTextChange::~LineDataListTextChange()
{
    if( oldListList_ ) {
        for( int i=0; i < length_; ++i ) {
            TextLineDataList* list = oldListList_[i];
            if( list ) {
                list->destroy(managerRef_);
                delete list;
            }
        }
        delete oldListList_;
        oldListList_ = 0;
    }
}


void LineDataListTextChange::execute(TextDocument* document)
{
    Q_UNUSED(document);
    // backup the old items
    for( int i=0; i<length_; ++i ){
        TextLineDataList* list = managerRef_->takeList(line_+i);  // take the ownership of the list
        if( list ) {
            // jit allocation of the list!
            if( !oldListList_ ) {
                oldListList_= new TextLineDataList*[length_](); /// new X[]() => fills it with zero's
            }
            oldListList_[i] = list;
        }
    }

    // replace all old items with 0 values
    managerRef_->fillWithEmpty( line_, length_, newLength_ );
}

void LineDataListTextChange::revert(TextDocument* doc)
{
    Q_UNUSED(doc);
    if( oldListList_ ) {
        managerRef_->replace( line_, newLength_, oldListList_, length_ );
        delete oldListList_;
        oldListList_ = 0;
    } else {
        managerRef_->fillWithEmpty( line_, newLength_, length_ );
    }
}


static void memcopy_or_zerofill( void* target, void* source, size_t size)
{
    if( source ) {
        memcpy( target, source, size);
    } else {
        memset( target, 0, size);
    }
}


bool LineDataListTextChange::merge(TextDocument* document, TextChange* textChange)
{
    Q_UNUSED(document);
    Q_UNUSED(textChange);

    LineDataListTextChange* lineChange = dynamic_cast<LineDataListTextChange*>(textChange);
    if( lineChange ) {

        //TODO:  simple merge (both elements are 0)
        bool filled = oldListList_ != 0 || lineChange->oldListList_ != 0;

        if( line_ + newLength_ == lineChange->line_ ) {
            int mergedLength = length_ + lineChange->length_;
            if( filled ) {
                TextLineDataList** old = oldListList_;
                oldListList_ = new TextLineDataList*[mergedLength];
                memcopy_or_zerofill( oldListList_, old, sizeof(TextLineDataList*)*length_ );
                memcopy_or_zerofill( oldListList_ + length_ , lineChange->oldListList_, sizeof(TextLineDataList*)*lineChange->length_);
                delete old;
                delete lineChange->oldListList_;
                lineChange->oldListList_ = 0;
            }
            length_    = mergedLength;
            newLength_ += lineChange->newLength_;
            return true;

        } else if( lineChange->line_ + lineChange->newLength_ == line_ ) {
            int mergedLength = length_ + lineChange->length_;
            if( filled ) {
                TextLineDataList** old = oldListList_;
                oldListList_ = new TextLineDataList*[mergedLength];
                memcopy_or_zerofill( oldListList_, lineChange->oldListList_, sizeof(TextLineDataList*)*lineChange->length_ );
                memcopy_or_zerofill( oldListList_+lineChange->length_, old, sizeof(TextLineDataList*)*length_ );
                delete old;
                delete lineChange->oldListList_;
                lineChange->oldListList_ = 0;
            }
            line_      = lineChange->line_;
            length_    = mergedLength;
            newLength_ += lineChange->newLength_;
            return true;
        }
    }
    return false;
}

/// changes the line with the given delta
void LineDataListTextChange::moveLine(int delta)
{
    line_ += delta;
}


QString LineDataListTextChange::toString()
{
    return QString("LineDataListTextChange(%1,%2,%3)").arg(line_).arg(length_).arg(newLength_);
}


} // edbee
