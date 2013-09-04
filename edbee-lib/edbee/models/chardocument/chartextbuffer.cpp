/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include <QApplication>
#include<QThread>

#include "chartextbuffer.h"

#include "debug.h"

namespace edbee {

CharTextBuffer::CharTextBuffer(QObject *parent)
    : TextBuffer( parent )
    , rawAppendStart_(-1)
    , rawAppendLineStart_(-1)
{
}


int CharTextBuffer::length() const
{
    return buf_.length();
}

QChar CharTextBuffer::charAt(int offset) const
{
    Q_ASSERT(offset >= 0);
    Q_ASSERT(offset < buf_.length() );
    return buf_.at(offset);
}

QString CharTextBuffer::textPart(int pos, int length) const
{
    // do NOT use data here. Data moves the gap!
    // QString str( buf_.data() + pos, length );
    // return str;
    ///buf_.data()

    QString result = buf_.mid( pos, length );
    return result;
}

void CharTextBuffer::replaceText(int offset, int length, const QChar* buffer, int bufferLength )
{

    // make sure the length matches
    length = qMin( this->length()-offset, length );

    // make sure the position is correct
    if( offset > buf_.length() ) {
        offset = buf_.length();  // Qt doesn't append if the position > length
        length = 0;
    }

    TextBufferChange change( this, offset, length, buffer, bufferLength );

//    emit textAboutToBeReplaced( offset, length, buffer, bufferLength );
//    emit linesAboutToBeReplaced( change.line, change.lineCount, change.newLineCount );

    emit textAboutToBeChanged( change );

    // replace the text
    buf_.replace( offset, length, buffer, bufferLength );

    // replace the line data and offsets
    lineOffsetList_.applyChange( change );

//    emit linesReplaced( change.line, change.lineCount, change.newLineCount );
//    emit textReplaced( offset, length, buffer, bufferLength );
    emit textChanged( change );

}

//void CharTextBuffer::replaceText(int pos, int length, const QString& text)
//{
//}


/// Because tha editor requests the same offset MANY times results are cached
int CharTextBuffer::lineFromOffset(int offset )
{
//    int result = lineFromOffsetSearch(offset);
    int result = lineOffsetList_.findLineFromOffset(offset);
    return result;
}

/// This method returns the offset of the given line
int CharTextBuffer::offsetFromLine(int line)
{
//    const QList<int>& lofs = lineOffsets_;
    if( line < 0 ) return 0;    // at the start
//    if( line >= lofs.length() ) return length();    // at the end
//    return lofs.at(line);
    if( line >= lineOffsetList_.length()) { return length(); }
    return lineOffsetList_.at(line);

}

void CharTextBuffer::rawAppendBegin()
{
    Q_ASSERT(rawAppendStart_ == -1 );
    Q_ASSERT(rawAppendLineStart_ == -1 );
    rawAppendStart_ = length();
    rawAppendLineStart_ = lineCount();
}

void CharTextBuffer::rawAppend(const QChar* data, int dataLength)
{
    buf_.append( data, dataLength );
}

void CharTextBuffer::rawAppend(QChar c)
{
    buf_.append( c );
}

/// Ends the 'raw' appending of data
void CharTextBuffer::rawAppendEnd()
{
    Q_ASSERT(rawAppendStart_ >= 0 );
    Q_ASSERT(rawAppendLineStart_ >= 0 );


    // append all the newlines to the vector
    /*
    int oldLength = lineOffsetList_.length();
    for( int i=rawAppendStart_,len=length(); i<len; ++i ){
        if( charAt(i) == '\n' ) { lineOffsetList_.appendOffset( i+1 ); }
    }
    int linesAdded = lineOffsetList_.length() - oldLength;
    */

    //emit the about signal
    TextBufferChange change( this, rawAppendStart_, 0, buf_.data() + rawAppendStart_, buf_.length() - rawAppendStart_ );

    emit textAboutToBeChanged( change );
    lineOffsetList_.applyChange( change );
    emit textChanged( change );

    rawAppendLineStart_ = -1;
    rawAppendStart_     = -1;
}

/// This method returns the raw data pointer
/// WARNING calling this method moves the gap of the gapvector to the end. Which could involve a lot of data moving
QChar* CharTextBuffer::rawDataPointer()
{
    return buf_.data();
}


} // edbee
