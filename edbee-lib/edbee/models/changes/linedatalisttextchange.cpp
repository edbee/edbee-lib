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
    , line_(line)
    , length_(length)
    , newLength_(newLength)
    , oldListList_(0)
{
}


/// Destructs the linedata textchange
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


/// Executes the line data changes
/// @param document the document to execute the change for
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
//qlog_info() <<"Execute:";
    managerRef_->fillWithEmpty( line_, length_, newLength_ );
}


/// Reverts the line data change
/// @param doc the document to execute the change for
void LineDataListTextChange::revert(TextDocument* doc)
{
//qlog_info() <<"Revert:";
    Q_UNUSED(doc);
    if( oldListList_ ) {
        managerRef_->replace( line_, newLength_, oldListList_, length_ );
        delete oldListList_;
        oldListList_ = 0;
    } else {
        managerRef_->fillWithEmpty( line_, newLength_, length_ );
    }
}


/// Per forms a memcopy or zero fills the given area
static void memcopy_or_zerofill( void* target, void* source, size_t size)
{
    if( source ) {
        memcpy( target, source, size);
    } else {
        memset( target, 0, size);
    }
}


/// Merges mutliple textline changes together
/// @param document the document the changes are fior
/// @param textChange the other textchange
/// @return true if the merge succeed, false if not
bool LineDataListTextChange::giveAndMerge(TextDocument* document, TextChange* textChange)
{
    Q_UNUSED(document);
    Q_UNUSED(textChange);

    LineDataListTextChange* lineChange = dynamic_cast<LineDataListTextChange*>(textChange);
    if( lineChange ) {

        int delta1 = newLength() - lineChange->length();
        int delta2 = lineChange->newLength() - lineChange->length();

        //TODO:  simple merge (both elements are 0)
        bool filled = oldListList_ != 0 || lineChange->oldListList_ != 0;

//qlog_info() << "merge: " << toString() << " => " << textChange->toString();

        // when the lines match
        if( line_ + delta1 == lineChange->line_ ||
            line_ == lineChange->line_ + delta2 ||
            line_ == lineChange->line_ ) {
            int mergedLength = length_ + lineChange->length_;
            if( filled ) {
                TextLineDataList** old = oldListList_;

                // we need to store (and remember) the removed line items
                oldListList_ = new TextLineDataList*[mergedLength];

                if( line_ <= lineChange->line_ ) {
                    memcopy_or_zerofill( oldListList_, old, sizeof(TextLineDataList*)*length_ );
                    memcopy_or_zerofill( oldListList_ + length_ , lineChange->oldListList_, sizeof(TextLineDataList*)*lineChange->length_);

                } else {
                    memcopy_or_zerofill( oldListList_, lineChange->oldListList_, sizeof(TextLineDataList*)*lineChange->length_ );
                    memcopy_or_zerofill( oldListList_+lineChange->length_, old, sizeof(TextLineDataList*)*length_ );

                }
                delete old;
                delete lineChange->oldListList_;
                lineChange->oldListList_ = 0;
            }
            line_      = qMin(line_, lineChange->line_ );
            length_    = mergedLength;
            newLength_ += lineChange->newLength_;
//qlog_info() << " ok: " << toString();

            delete textChange;
            return true;
        }
    }

//qlog_info() << "=(c) " << this->toString();
    return false;
}


/// changes the line with the given delta
/// @param delta
void LineDataListTextChange::applyLineDelta(int line, int length, int newLength)
{
//qlog_info() << "moveLine("<<line<<"): " << toString();
    if( line <= line_ ) {
        line_ += newLength + length;
    }
//qlog_info() << "=>" << toString();
}


/// Converts this change to a string
QString LineDataListTextChange::toString()
{
    return QString("LineDataListTextChange(%1,%2,%3)").arg(line_).arg(length_).arg(newLength_);
}


/// Returns the line
int LineDataListTextChange::line()
{
    return line_;
}


/// returns the length
int LineDataListTextChange::length()
{
    return length_;
}


/// returns the new length
int LineDataListTextChange::newLength()
{
    return newLength_;
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
        return length_;
    }
    return 0;
}


/// Returns the length of the old list list


} // edbee
