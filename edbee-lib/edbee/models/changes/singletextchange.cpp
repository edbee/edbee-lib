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


/// undo's a single textchange
SingleTextChange::~SingleTextChange()
{
}


/// executes the given textchange
/// @param document the document to execute the change on
void SingleTextChange::execute(TextDocument* document)
{
    replaceText(document);
    executed_ = true;
}


/// Reverts the single textchange
/// @param document the document to execute the change on
void SingleTextChange::revert(TextDocument* document)
{
    replaceText(document);
    executed_ = false;
}


/// This method merges the old data with the new data
/// @apram change the data to merge with
void SingleTextChange::mergeOldData(AbstractRangedTextChange* change)
{
    SingleTextChange* singleTextChange = dynamic_cast<SingleTextChange*>(change);

    QString newText;
    newText.resize( calculateMergeDataSize( change) );
    mergeData( newText.data(), text_.data(), singleTextChange->text_.data(), change, sizeof(QChar) );
    /*
      QString newText;
      // we first need to 'take' the leading part of the new change
      if( change->offset() < offset() ) {
          newText.append( change->oldText(document).mid(0, offset() - change->offset() ) );
      }

      newText.append(oldText(document));

      // then we need to append the remainer
      int delta = offset()-change->offset();
      int remainerOffset = newLength() + delta;
      if( remainerOffset >= 0 ) {
          if( remainerOffset < change->oldLength() ) {
              //Q_ASSERT(false);    // need to figure out if this works
              newText.append( change->oldText( document  ).mid(remainerOffset ) );
          }
      }

  */

    text_ = newText;
}


/// This method merges the change
/// @param document the document to merges
/// @param change the change change to merge
bool SingleTextChange::merge( AbstractRangedTextChange* change )
{
    // overlap is a bit harder
    if( isOverlappedBy(change) || isTouchedBy(change) ) {

        // build the new sizes and offsets
        int newOffset = qMin( offset(), change->offset() );
        int newLength = getMergedLength(change);

        // merge the data
        mergeOldData( change );

        // when the data is meged assign the new dimensions
        length_ = newLength;
        offset_ = newOffset;
        delete change;
        return true;
    }
    return false;
}



/// This method gives the given change to this textchange. The changes will be merged
/// if possible. This method currently only works with executed changes!!!
///
/// @param document the document
/// @param textChange the textchange to mege
/// @return true on success else false
bool SingleTextChange::giveAndMerge( TextDocument* document, TextChange* textChange)
{
    Q_UNUSED( document );
    Q_ASSERT( executed_ );
    SingleTextChange* change = dynamic_cast<SingleTextChange*>( textChange );
    if( change ) {
        Q_ASSERT(change->executed_);
        return merge( change );
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
void SingleTextChange::setOldLength(int len)
{
    Q_ASSERT(executed_);
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
