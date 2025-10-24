// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include <QApplication>
#include<QThread>

#include "chartextbuffer.h"

#include "edbee/debug.h"

namespace edbee {

/// The constructor of the textbuffer
/// @param a reference to the parent
CharTextBuffer::CharTextBuffer(QObject *parent)
    : TextBuffer( parent )
    , rawAppendStart_(std::string::npos)
    , rawAppendLineStart_(std::string::npos)
{
}


/// Returns the length of the buffer
/// @return the length of the given text
size_t CharTextBuffer::length() const
{
    return buf_.length();
}


/// Returns the character at the given character
/// @param offset the offset of the given character
/// @return the character at the given offset
QChar CharTextBuffer::charAt(size_t offset) const
{
    Q_ASSERT(offset < buf_.length() );
    return buf_.at(offset);
}


/// Returns the text part
/// @param pos the position of the given text
/// @param length the length of the text to get
/// @return returns a part of the text
QString CharTextBuffer::textPart(size_t pos, size_t length) const
{
    // do NOT use data here. Data moves the gap!
    // QString str( buf_.data() + pos, length );
    // return str;
    ///buf_.data()

    QString result = buf_.mid(pos, length);
    return result;
}


/// replaces the given text
/// @param offset the offset of the text to replace
/// @param length the length of the text to replace
/// @param buffer a pointer to a buffer with data
/// @param bufferLenth the length of the buffer
void CharTextBuffer::replaceText(size_t offset, size_t length, const QChar* buffer, size_t bufferLength )
{
    // make sure the length matches
    length = qMin(this->length()-offset, length);

    // make sure the position is correct
    if(offset > buf_.length()) {
        offset = buf_.length();  // Qt doesn't append if the position > length
        length = 0;
    }

    TextBufferChange change(this, offset, length, buffer, bufferLength);

    emit textAboutToBeChanged(change);

    // replace the text
    QString oldText = buf_.mid(offset, length);

    buf_.replace(offset, length, buffer, bufferLength);

    // replace the line data and offsets
    lineOffsetList_.applyChange(change);

    emit textChanged(change, oldText);
}


/// Returns the line position at the given offset
/// @param offset the offset to retreive the line from
/// @return the line from the given offset
size_t CharTextBuffer::lineFromOffset(size_t offset)
{
    //  int result = lineFromOffsetSearch(offset);
    size_t result = lineOffsetList_.findLineFromOffset(offset);
    return result;
}


/// This method returns the offset of the given line
/// @param lin the line to retrieve the offset from
/// @return the offset of the given line
size_t CharTextBuffer::offsetFromLine(size_t line)
{
    if( line >= lineOffsetList_.length()) {
        return length();
    }

    return lineOffsetList_.at(line);
}


/// Starts raw data appending to the buffer
void CharTextBuffer::rawAppendBegin()
{
    Q_ASSERT(rawAppendStart_ == std::string::npos );
    Q_ASSERT(rawAppendLineStart_ == std::string::npos );
    rawAppendStart_ = length();
    rawAppendLineStart_ = lineCount();
}


/// Appends a buffer of text to the document
/// @param data the data to append
/// @param dataLength the number of bytes availble by the data pointer
void CharTextBuffer::rawAppend(const QChar* data, size_t dataLength)
{
    buf_.append(data, dataLength);
}


/// Append a single character to the buffer in raw mode
/// @param c the character to append
void CharTextBuffer::rawAppend(QChar c)
{
    buf_.append( c );
}


/// Ends the 'raw' appending of data
void CharTextBuffer::rawAppendEnd()
{
    Q_ASSERT(rawAppendStart_ != std::string::npos);
    Q_ASSERT(rawAppendLineStart_ != std::string::npos);

    // emit the about signal
    TextBufferChange change(this, rawAppendStart_, 0, buf_.data() + rawAppendStart_, buf_.length() - rawAppendStart_);

    emit textAboutToBeChanged( change );
    lineOffsetList_.applyChange( change );
    emit textChanged( change, QString() );

    rawAppendLineStart_ = std::string::npos;
    rawAppendStart_     = std::string::npos;
}


/// This method returns the raw data pointer
/// WARNING calling this method moves the gap of the gapvector to the end. Which could involve a lot of data moving
QChar* CharTextBuffer::rawDataPointer()
{
    return buf_.data();
}

} // edbee
