/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "singletextchange.h"

#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"

#include "debug.h"

namespace edbee {


/// Constructs a single textchange
/// @param offset, the offset of the change
/// @param length, the length of the change
/// @param text , the new text
/// @param executed, a boolean (mainly used for testing) to mark this change as exected
SingleTextChange::SingleTextChange(int offset, int length, const QString& text, bool executed )
    : offset_(offset)
    , length_(length)
    , text_(text)
    , executed_(executed)
{
}

SingleTextChange::~SingleTextChange()
{
}

void SingleTextChange::execute(TextDocument* document)
{
    replaceText(document);
    executed_ = true;
}

void SingleTextChange::revert(TextDocument* document)
{
    replaceText(document);
    executed_ = false;
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
/// if possible. This method currently only works with executed changes!!!
///
/// @param document the document
/// @param textChange the textchange to mege
/// @return true on success else false
///
/// TODO: Split this method up in several small methods: (mergeText, mergeLength)
bool SingleTextChange::giveAndMerge( TextDocument* document, TextChange* textChange)
{
    Q_UNUSED( document );
    Q_ASSERT( executed_ );
    SingleTextChange* change = dynamic_cast<SingleTextChange*>( textChange );
    if( change ) {
        Q_ASSERT(change->executed_);

        // overlap is a bit harder
        if( isOverlappedBy(change) || isTouchedBy(change) ) {

            SingleTextChange* a = this;
            SingleTextChange* b = change;
//qlog_info() << "giveAndMerge----------";
//qlog_info() << "A:" << a->toString() << " | " << a->isExecuted();
//qlog_info() << "B:" << b->toString() << " | " << b->isExecuted();

            // build the new text
            QString newText;
            {
                // we first need to 'take' the leading part of the new change
                if( change->offset() < offset() ) {
                    newText.append( change->oldText(document).mid(0, offset() - change->offset() ) );
                }

                newText.append(oldText(document));

                // then we need to append the remainer
                int delta = offset()-change->offset();
                int remainerOffset = a->newLength() + delta;
                if( remainerOffset >= 0 ) {
                    if( remainerOffset < b->oldLength() ) {
                        //Q_ASSERT(false);    // need to figure out if this works
                        newText.append( b->oldText( document  ).mid(remainerOffset ) );
                    }
                }
            }

            // build the new length
            int len = b->newLength();
            {
                // add the prefix of the a length
                if( offset() < b->offset() ) { // && b->offset() < (offset() + length())   ) {
                    len += ( b->offset()-offset() );
                }

                // add the postfix of the length
                int aEnd = a->offset() + a->newLength();
                int bEnd = b->offset() + b->oldLength();
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


/// this method returns the old length of the change
int SingleTextChange::oldLength() const
{
    return isExecuted() ? text_.size() : length_;
}


/// This method returns the new length of the text
int SingleTextChange::newLength() const
{
    return isExecuted() ? length_ : text_.size();
}


/// Returns the length of the text currently in the doc depending on the undo state
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


/// This method returns the old text of the change
QString SingleTextChange::oldText(TextDocument* doc) const
{
    return isExecuted() ? text_ : doc->textPart(offset(), oldLength() );
}


/// This method returns the changed text
QString SingleTextChange::newText(TextDocument* doc) const
{
    return isExecuted() ? doc->textPart(offset(), length_ ) : text_;
}


/// The text currently stored in this textchange
QString SingleTextChange::storedText() const
{
    return text_;
}


/// Sets the text of this change
/// @param text the new text
void SingleTextChange::setStoredText(const QString& text)
{
    text_ = text;
}


/// Appends the text to this change
void SingleTextChange::appendStoredText(const QString& text)
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
    ( offset() < ( secondChange->offset() + secondChange->newLength() ) &&  secondChange->offset() < (offset() + newLength()) )
    || (  offset() < ( secondChange->offset() + secondChange->oldLength() ) && secondChange->offset() < (offset() + newLength())  )
    ;
}


/// Touched ranges are ranges that are next to eachother
/// Touching means the end offset of one range is the start offset of the other range
/// @param secondChange the other change to match
/// @return true if the changes overlap
bool SingleTextChange::isTouchedBy(SingleTextChange* secondChange)
{
    return offset() == (secondChange->offset() + secondChange->newLength() )
        || (offset() + newLength()) == secondChange->offset()
        // Delete operation should be supported
        || ( secondChange->newLength() < secondChange->oldLength() &&  offset() == (secondChange->offset() + secondChange->oldLength() ) )
        // delete touch (Should we add those length < newlength condition!??)
        //|| ( ( length() < newLength() ) && (offset() + newLength()) == secondChange->offset() )
            ;
}


/// This method returns true if the change has been executed
bool SingleTextChange::isExecuted() const
{
    return executed_;
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
