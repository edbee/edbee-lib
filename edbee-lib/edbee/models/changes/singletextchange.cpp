/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "singletextchange.h"

#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"

#include "debug.h"

namespace edbee {

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

bool SingleTextChange::merge( TextDocument* document, TextChange* textChange)
{
    Q_UNUSED( document );
    SingleTextChange* change = dynamic_cast<SingleTextChange*>( textChange );
    if( change ) {
        // merge the data
        if( offset_ + length_ == change->offset_ ) {
            text_.append( change->text() );
            length_ += change->length();
            return true;
        }
        if( change->offset_ + change->text().length() == offset_ ) {
            text_.prepend( change->text() );
            offset_ =  change->offset_; //change->length();
            length_ += change->length();
            return true;
        }

    }
    return false;
}

QString SingleTextChange::toString()
{
//    return "SingleTextChange";
    return QString("SingleTextChange:%1").arg(testString());
}

/// This method returns the text currently in the document
const QString SingleTextChange::docText(TextDocument* doc)
{
    return doc->textPart( offset_, length_ );
}

/// execute the a spatial change
void SingleTextChange::applyOffsetDelta(int offset, int length, int newLength)
{
    if( offset <= offset ) {
        offset_ += ( newLength - length );
    }
}

/// This method returns a string used for testing
QString SingleTextChange::testString()
{
    return QString("%1:%2:%3").arg(offset_).arg(length_).arg(QString(text_).replace("\n","§"));
}

/// replaces the text and stores the 'old' content
void SingleTextChange::replaceText(TextDocument *document)
{
    TextBuffer* buffer = document->buffer();

    QString old = buffer->textPart( offset_, length_ );
    buffer->replaceText( offset_, length_, text_ );
    length_ = text_.length();
    text_ = old;
}

} // edbee
