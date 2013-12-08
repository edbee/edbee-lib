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
SingleTextChange::SingleTextChange(int offset, int length, const QString& text)
    : offset_(offset)
    , length_(length)
    , text_(text)
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
}


/// Reverts the single textchange
/// @param document the document to execute the change on
void SingleTextChange::revert(TextDocument* document)
{
    replaceText(document);
}


/// This method merges the old data with the new data
/// @apram change the data to merge with
void SingleTextChange::mergeStoredData(AbstractRangedTextChange* change)
{
    SingleTextChange* singleTextChange = dynamic_cast<SingleTextChange*>(change);

    QString newText;
    newText.resize( getMergedStoredLength( change) );
    mergeStoredDataViaMemcopy( newText.data(), text_.data(), singleTextChange->text_.data(), change, sizeof(QChar) );
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


/// This method gives the given change to this textchange. The changes will be merged
/// if possible. This method currently only works with executed changes!!!
///
/// @param document the document
/// @param textChange the textchange to mege
/// @return true on success else false
bool SingleTextChange::giveAndMerge( TextDocument* document, TextChange* textChange)
{
    Q_UNUSED( document );
    SingleTextChange* change = dynamic_cast<SingleTextChange*>( textChange );
    if( change ) {
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


/// This is the length in the document
int SingleTextChange::docLength() const
{
    return length_;
}


/// The content length is the length that's currently stored in memory.
int SingleTextChange::storedLength() const
{
    return text_.size();
}


/// Set the length of the change
/// @param len sets the length of the change
void SingleTextChange::setDocLength(int len)
{
    length_ = len;
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


/// This method returns a string used for testing
QString SingleTextChange::testString()
{
    return QString("%1:%2:%3").arg(offset_).arg(length_).arg(QString(text_).replace("\n","§"));
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
