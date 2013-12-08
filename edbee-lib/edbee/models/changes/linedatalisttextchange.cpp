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


/// The line data text change constructor
/// @param manger the line data manager
/// @param line the starting line of the change
/// @param length the number of lines affected
/// @param newLength the new number of lines
LineDataListTextChange::LineDataListTextChange( TextLineDataManager* manager, int line, int length, int newLength )
    : managerRef_(manager)
    , offset_(line)
    , docLength_(newLength)
    , oldListList_(0)
    , contentLength_(length)
{
}


/// Destructs the linedata textchange
LineDataListTextChange::~LineDataListTextChange()
{
    if( oldListList_ ) {
        for( int i=0; i < contentLength_; ++i ) {
            TextLineDataList* list = oldListList_[i];
            if( list ) {
                list->destroy(managerRef_);
                delete list;
            }
        }
        delete[] oldListList_;
        oldListList_ = 0;
    }
}


/// Executes the line data changes
/// @param document the document to execute the change for
void LineDataListTextChange::execute(TextDocument* document)
{
    Q_UNUSED(document);
    // backup the old items
    for( int i=0; i<contentLength_; ++i ){
        TextLineDataList* list = managerRef_->takeList(offset_+i);  // take the ownership of the list
        if( list ) {
            // jit allocation of the list!
            if( !oldListList_ ) {
                oldListList_= new TextLineDataList*[contentLength_](); /// new X[]() => fills it with zero's
            }
            oldListList_[i] = list;
        }
    }

    // replace all old items with 0 values
    managerRef_->fillWithEmpty( offset_, contentLength_, docLength_ );
}


/// Reverts the line data change
/// @param doc the document to execute the change for
void LineDataListTextChange::revert(TextDocument* doc)
{
    Q_UNUSED(doc);
    if( oldListList_ ) {
        managerRef_->replace( offset_, docLength_, oldListList_, contentLength_ );
        delete oldListList_;
        oldListList_ = 0;
    } else {
        managerRef_->fillWithEmpty( offset_, docLength_, contentLength_ );
    }
}


/// This method merges the old data with the new data
/// @apram change the data to merge with
void LineDataListTextChange::mergeStoredData(AbstractRangedTextChange* change)
{
    LineDataListTextChange* lineTextChange = dynamic_cast<LineDataListTextChange*>(change);

    // calculate the new size
    int newOldListSize = getMergedStoredLength( change);// qlog_info() << "CALCULATED: " << newOldListSize ;

    // no old data, we don't need to store anthing
    if( this->oldListList_ == 0 && lineTextChange->oldListList_ == 0 ) {       
        contentLength_ = newOldListSize;    // also store the content list
        return;
    }

    // create the new list
    TextLineDataList**  newOldListList_ = new TextLineDataList*[newOldListSize];

    // merge the stuff
    mergeStoredDataViaMemcopy( newOldListList_, oldListList_, lineTextChange->oldListList_, change, sizeof(TextLineDataList*) );

    // we need to delete all items that aren't used anymore
    if( oldListList_ ) {
        for( int i=0; i<contentLength_; ++i ) {
            bool found=false;
            TextLineDataList* list = oldListList_[i];

            // find it in this change
            for( int j=0; j<newOldListSize && !found; ++j ) {
                found = ( list == newOldListList_[j] );
            }

            // delete the old list if not found
            if( !found ) {
//                list->destroy( // manager WTF !);
                delete list;
            }
        }
        delete[] oldListList_;
        oldListList_ = 0;
    }

    // same for other change
    if( lineTextChange->oldListList_ ) {
        for( int i=0; i<lineTextChange->contentLength_; ++i ) {
            bool found=false;
            TextLineDataList* list = lineTextChange->oldListList_[i];

            // find it in this change
            for( int j=0; j<newOldListSize && !found; ++j ) {
                found = ( list == newOldListList_[j] );
            }

            // delete the old list if not found
            if( !found ) {
//                list->destroy( // manager WTF !);
                delete list;
            }
        }
        delete[] lineTextChange->oldListList_;
        lineTextChange->oldListList_ = 0;
    }

/// TODO: Know how and what to delete
    oldListList_ = newOldListList_;
    contentLength_ = newOldListSize;    // also store the content list
}


/// Merges mutliple textline changes together
/// @param document the document the changes are fior
/// @param textChange the other textchange
/// @return true if the merge succeed, false if not
bool LineDataListTextChange::giveAndMerge(TextDocument* document, TextChange* textChange)
{
    Q_UNUSED(document);
    Q_UNUSED(textChange);
    LineDataListTextChange* change = dynamic_cast<LineDataListTextChange*>( textChange );
    if( change ) {
        return merge( change );
    }
    return false;
}


/// Converts this change to a string
QString LineDataListTextChange::toString()
{
    return QString("LineDataListTextChange(%1,%2,%3)").arg(offset_).arg(contentLength_).arg(docLength_);
}


/// Returns the line
int LineDataListTextChange::offset () const
{
    return offset_;
}


/// Sets the new offset
void LineDataListTextChange::setOffset(int value)
{
    offset_ = value;
}


/// Retursn the length in the document/data
int LineDataListTextChange::docLength() const
{
    return docLength_;
}


/// This method sets the old length
/// @param value the new old-length value
void LineDataListTextChange::setDocLength(int value)
{
    docLength_ = value;
}



/// The lengt of the content in this object
int LineDataListTextChange::storedLength() const
{
    return contentLength_;
}


/// returns the old list list
TextLineDataList**LineDataListTextChange::oldListList()
{
    return oldListList_;
}


/// retursn the length of th eold list list
int LineDataListTextChange::oldListListLength()
{
    if( oldListList_ ) {
        return contentLength_;
    }
    return 0;
}


} // edbee
