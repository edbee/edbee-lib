/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "singletextchange.h"

#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"

#include "debug.h"

namespace edbee {


///
SingleTextChange::SingleTextChange(int offset, int length, const QString &text )
    : offset_(offset)
    , length_(length)
    , text_(text)
{
}

SingleTextChange::~SingleTextChange()
{
}

void SingleTextChange::execute(TextDocument* document)
{
    replaceText(document);
}

void SingleTextChange::revert(TextDocument* document)
{
    replaceText(document);
}

//bool SingleTextChange::canMerge(TextDocument* document, TextChange* textChange)
//{
//    Q_UNUSED( document );
//    SingleTextChange* change = dynamic_cast<SingleTextChange*>( textChange );
//    if( change ) {
//        if( offset_ + length_ == change->offset_ ) return true;
//        if( change->offset_ + change->length_ == offset_ ) return true;
//    }
//    return false;
//}

/// This method gives the given change to this textchange. The changes will be merged
/// if possible
///
/// @param document the document
/// @param textChange the textchange to mege
/// @return true on success else false
///
/// TODO: Split this method up in several small methods: (mergeText, mergeLength)
bool SingleTextChange::giveAndMerge( TextDocument* document, TextChange* textChange)
{
    Q_UNUSED( document );
    SingleTextChange* change = dynamic_cast<SingleTextChange*>( textChange );
    if( change ) {

        // overlap is a bit harder
        if( isOverlappedBy(change) || isTouchedBy(change) ) {

            SingleTextChange* a = this;
            SingleTextChange* b = change;
//qlog_info() << "giveAndMerge----------";
//qlog_info() << "A:" << a->toString();
//qlog_info() << "B:" << b->toString();

            // build the new text
            QString newText;
            {
                // we first need to 'take' the leading part of the new change
                if( change->offset() < offset() ) {
                    newText.append( change->text().mid(0, offset() - change->offset() ) );
                }

                // when the b-change falls in the delete of the first change, we need to 'split' the old test
//                if( false &&  a->offset() < b->offset() && b->offset() < (a->offset()+a->newLength())   ) {
//                    int split = b->offset()-a->offset();

//                    newText.append( a->text().mid(0,split) );
//                    newText.append( b->text() );
//                    newText.append( a->text().mid(split) );
//                // default operation is to add the full text
//                } else {
                    newText.append(text());
//                }

                // then we need to append the remainer
                int delta = offset()-change->offset();
                int remainerOffset = a->length() + delta;
                if( remainerOffset >= 0 ) {
                    if( remainerOffset < b->text().length() ) {
                        //Q_ASSERT(false);    // need to figure out if this works
                        newText.append( b->text().mid(remainerOffset ) );
                    }
                }
            }

            // build the new length
            int len = b->length();
            {
                // add the prefix of the a length
                if( offset() < b->offset() ) { // && b->offset() < (offset() + length())   ) {
                    len += ( b->offset()-offset() );
                }

                // add the postfix of the length
                int aEnd = a->offset() + a->length();
                int bEnd = b->offset() + b->newLength();
                if( bEnd < aEnd ) {
                    len += aEnd-bEnd;
                }
            }

            // now assign the new length
            length_ = len;

            // set the text
            text_ = newText;

            /// the new offset is the minimum offset
            offset_ = qMin( offset_, b->offset_ );

            delete change;
            return true;
        }

    }
    return false;
}


/// converts the change to a string
QString SingleTextChange::toString()
{
//    return "SingleTextChange";
    return QString("SingleTextChange:%1").arg(testString());
}


/// Return the offset
/// @return the offset of the change
int SingleTextChange::offset() const
{
    return offset_;
}


/// set the new offset
/// @param offset the new offset
void SingleTextChange::setOffset(int offset)
{
    offset_ = offset;
}


/// Adds the given amount to the offset
/// @param amount the offset to add
void SingleTextChange::addOffset(int amount)
{
    offset_ += amount;
}


/// returns the length of the change
int SingleTextChange::length() const
{
    return length_;
}


/// Set the length of the change
/// @param len sets the length of the change
void SingleTextChange::setLength(int len)
{
    length_ = len;
}


/// Returns the new length which essentially is the length of the text variable
int SingleTextChange::newLength() const
{
    return text_.length();
}


/// Returns a const reference to the text
const QString& SingleTextChange::text() const
{
    return text_ ;
}


/// Sets the text of this change
/// @param text the new text
void SingleTextChange::setText(const QString& text)
{
    text_ = text;
}


/// Appends the text to this change
void SingleTextChange::appendText(const QString& text)
{
    text_.append( text );
}


/// This method returns the text currently in the document
const QString SingleTextChange::docText(TextDocument* doc) const
{
    return doc->textPart( offset_, length_ );
}


/// execute the a spatial change
void SingleTextChange::applyOffsetDelta(int offset, int length, int newLength)
{
    if( offset <= offset_ ) {
        offset_ += ( newLength - length );
    }
}


/// This method returns a string used for testing
QString SingleTextChange::testString()
{
    return QString("%1:%2:%3").arg(offset_).arg(length_).arg(QString(text_).replace("\n","§"));
}


/// This method checks if this textchange is overlapped by the second text change
/// overlapping is an exclusive overlap, which means the changes are really on top of eachother
/// to test if the changes are touching use isTouchedBy
/// @param secondChange the other change to compare it to
/// @return tue if the changes overlap
bool SingleTextChange::isOverlappedBy(SingleTextChange* secondChange)
{
    return
    ( offset() < ( secondChange->offset() + secondChange->length() ) &&  secondChange->offset() < (offset() + length()) )
    || (  offset() < ( secondChange->offset() + secondChange->newLength() ) && secondChange->offset() < (offset() + length())  )
    || (
        false
//         secondChange->length() < secondChange->newLength()  &&
//         offset() < ( secondChange->offset() ) && secondChange->offset() < (offset() + newLength())
       )
            ;
}


/// Touched ranges are ranges that are next to eachother
/// Touching means the end offset of one range is the start offset of the other range
/// @param secondChange the other change to match
/// @return true if the changes overlap
bool SingleTextChange::isTouchedBy(SingleTextChange* secondChange)
{
    return offset() == (secondChange->offset() + secondChange->length() )
        || (offset() + length()) == secondChange->offset()
        // Delete operation should be supported
        || ( secondChange->length() < secondChange->newLength() &&  offset() == (secondChange->offset() + secondChange->newLength() ) )
        // delete touch (Should we add those length < newlength condition!??)
        //|| ( ( length() < newLength() ) && (offset() + newLength()) == secondChange->offset() )
        ;
}


/// replaces the text and stores the 'old' content
/// @param document the document to change it for
void SingleTextChange::replaceText(TextDocument* document)
{
    TextBuffer* buffer = document->buffer();
    QString old = buffer->textPart( offset_, length_ );

    buffer->replaceText( offset_, length_, text_ );
    length_ = text_.length();
    text_ = old;
}

} // edbee
