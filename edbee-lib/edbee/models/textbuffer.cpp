// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textbuffer.h"

#include "edbee/models/textrange.h"
#include "edbee/util/lineoffsetvector.h"

#include "edbee/debug.h"

namespace edbee {


/// Initializes the textbuffer change
/// @param buffer when buffer is 0 NO line calculation is done
TextBufferChangeData::TextBufferChangeData(TextBuffer* buffer, size_t off, size_t len, const QChar *text, size_t textlen)
    : offset_(off)
    , length_(len)
    , newText_(text)
    , newTextLength_(textlen)
    , newLineOffsets_()
{
    Q_ASSERT(buffer);

    // decide which lines
    line_         = buffer->lineFromOffset(offset_);
    size_t endLine   = buffer->lineFromOffset(offset_ + length_);
    Q_ASSERT(endLine >= line_);

    lineCount_    = endLine - line_;

    // find the newlines in the text
    for (size_t i = 0; i < newTextLength_; ++i) {
        if (newText_[i] == '\n') {
            newLineOffsets_.append(offset_ + i + 1);    // +1 because it points to the start of the next line
        }
    }
}


/// Initializes the textbuffer change
/// @param buffer when buffer is 0 NO line calculation is done
TextBufferChangeData::TextBufferChangeData(LineOffsetVector* lineOffsets, size_t off, size_t len, const QChar *text, size_t textlen)
    : offset_(off)
    , length_(len)
    , newText_(text)
    , newTextLength_(textlen)
    , newLineOffsets_()
{
    Q_ASSERT(lineOffsets);

    // decide which lines
    line_          = lineOffsets->findLineFromOffset(offset_);
    size_t endLine = lineOffsets->findLineFromOffset(offset_ + length_);
    Q_ASSERT(endLine >= line_);

    lineCount_     = endLine - line_;

    // find the newlines in the text
    for (size_t i = 0; i < newTextLength_; ++i) {
        if( newText_[i] == '\n' ) {
            newLineOffsets_.append(offset_ + i + 1);    // +1 because it points to the start of the next line
        }
    }
}


TextBufferChange::TextBufferChange()
{
    d_ = new TextBufferChangeData((TextBuffer*)nullptr, 0, 0, 0, 0);
}


TextBufferChange::TextBufferChange(TextBuffer* buffer, size_t off, size_t len, const QChar* text, size_t textlen)
{
    d_ = new TextBufferChangeData(buffer, off, len, text, textlen);
}

TextBufferChange::TextBufferChange(LineOffsetVector* lineOffsets, size_t off, size_t len, const QChar *text, size_t textlen)
{
    d_ = new TextBufferChangeData(lineOffsets, off, len, text, textlen);
}


TextBufferChange::TextBufferChange(const TextBufferChange& other) : d_(other.d_)
{
}


const QString TextBufferChange::toDebugString() const
{
    QString s;
    s.append("TextBufferChange: ");
    s.append(QStringLiteral(" | (offset: %1, line: %2").arg(offset()).arg(line()));
    s.append(QStringLiteral(" | length: %1 => %2").arg(length()).arg(newTextLength()));
    s.append(QStringLiteral(" | lines: %1 => %2").arg(lineCount()).arg(newLineCount()));
    s.append(" | offsets: ");
    for (qsizetype i = 0, cnt = newLineOffsets().length(); i < cnt; ++i) {
        if (i) s.append(", ");
        s.append(QStringLiteral("%1").arg(newLineOffsets().at(i)));
    }
    s.append(QStringLiteral(" | text: %1").arg(newText()));
    return s;
}


//=====================================================


/// The textbuffer constructor
TextBuffer::TextBuffer(QObject *parent)
    : QObject(parent)
{
}


/// Replaces the given text
/// @param offset the offset to replace
/// @param length the of the text to replace
/// @param text the new text to insert
void TextBuffer::replaceText(size_t offset, size_t length, const QString& text)
{
    replaceText(offset, length, text.data(), static_cast<size_t>(text.length()));
}


/// Returns the full text as a QString
QString TextBuffer::text()
{
    return textPart(0, length());
}


/// A convenient method for directly filling the textbuffer with the given content
void TextBuffer::setText(const QString& text)
{
    replaceText(0, length(), text.data(), static_cast<size_t>(text.length()));
}


/// translates the given position to a column number.
/// @param offset the character offset
/// @param line the line index this position is on. (Use this argument for optimization if you already know this)
/// 			(default std::string::npos, use the current line index)
size_t TextBuffer::columnFromOffsetAndLine(size_t offset, size_t line)
{
    if (line == std::string::npos) {
        line = lineFromOffset(offset);
    }

    // const QList<int>& lofs = lineOffsets();
    if (line < lineCount()) {
        size_t lineOffset = offsetFromLine(line);
        if (lineOffset >= offset) {
            return 0;
        }
        return qMin(lineLength(line), offset - lineOffset);
    } else {
        return 0;
    }
}


/// Appends the given text to the textbuffer
/// @param text the text to appendf
void TextBuffer::appendText(const QString& text)
{
    replaceText(length(),0,text.data(), static_cast<size_t>(text.length()));
}


/// Returns the offset from the give line and column
/// If the column exceed the number of column the caret is placed just before the newline
size_t TextBuffer::offsetFromLineAndColumn(size_t line, size_t col)
{
    size_t offsetLine = offsetFromLine(line);
    size_t offsetNextLine = offsetFromLine(line + 1);
    size_t offset = offsetLine + col;
    if (offset >= offsetNextLine && offset < length()) { --offset; }
    return offset;
}


/// Returns the line at the given line position. This line INCLUDES the newline character (if it's there)
/// @param line the line to return
QString TextBuffer::line(size_t line)
{
    size_t off = offsetFromLine(line);
    size_t endOff = offsetFromLine(line + 1);
    return textPart(off, endOff - off); // skip the return
}


/// Returns the line without the newline character
QString TextBuffer::lineWithoutNewline(size_t line)
{
    size_t off = offsetFromLine(line);
    size_t removeNewlineCount = 1;
    if (line == lineCount() - 1) { removeNewlineCount = 0; }
    return textPart(off , offsetFromLine(line + 1) - off - removeNewlineCount); // skip the return
}


/// Returns the length of the given line. Also counting the trailing newline character if present
/// @param line the line to retrieve the length for
/// @return the length of the given line
size_t TextBuffer::lineLength(size_t line)
{
    return offsetFromLine(line + 1) - offsetFromLine(line);
}


/// Returns the length of the given line. Without counting a trailing newline character
/// @param line the line to retrieve the length for
/// @return the length of the given line
size_t TextBuffer::lineLengthWithoutNewline(size_t line)
{
    size_t removeNewlineCount = 1;
    if (line == lineCount() - 1) { removeNewlineCount = 0; }
    size_t lastOffset = offsetFromLine(line + 1) - removeNewlineCount;
    return lastOffset - offsetFromLine(line);
}


/// replace the texts
/// @param range the range to replace
/// @param text the text to insert at the given location
void TextBuffer::replaceText(const TextRange& range, const QString& text)
{
    return replaceText(range.min(), range.length(), text.data(), static_cast<size_t>(text.length()));
}


/// See documentation at findCharPosWithinRange
/// @param offset the offset to start searching
/// @parm direction the direction (left < 0, or right > 0 )
/// @param chars the chars to search
/// @param equals when setting to true if will search for the first given char. When false it will stop when another char is found
/// @returns the character position or std::string:npos if not found
size_t TextBuffer::findCharPos(size_t offset, int direction, const QString& chars, bool equals)
{
    return findCharPosWithinRange(offset, direction, chars, equals, 0, length());
}


/// Finds the find the first character position that equals the given char
///
/// @param offset the offset to search from. A negative offset means the CURRENT character isn't used
/// @param direction the direction to search. If the direction is multiple. the nth item is returned
/// @param chars the character direction
/// @param equals when setting to true if will search for the first given char. When false it will stop when another char is found
/// @param beginRange the start of the range to search in
/// @param endRange the end of the range to search in (exclusive)
/// @return the offset of the first character (Or std::string::npos if not found)
size_t TextBuffer::findCharPosWithinRange(size_t offset, int direction, const QString& chars, bool equals, size_t beginRange, size_t endRange)
{
    if (offset == std::string::npos) { return std::string::npos; }

    size_t charNumber = static_cast<size_t>(qAbs(direction));
    if (charNumber == 0) return std::string::npos;

    while (beginRange <= offset && offset < endRange) {
        if (chars.contains( charAt(offset) ) == equals) {
            if (--charNumber == 0) { return offset; }
        }
        if (direction < 0) {
            --offset;
        } else {
            ++offset;
        }
    }
    return std::string::npos;
}


/// See documentation at findCharPosWithinRange.
/// This method searches a char position within the given rang (from the given ofset)
size_t TextBuffer::findCharPosOrClamp(size_t offset, int direction, const QString& chars, bool equals)
{
    return findCharPosWithinRangeOrClamp(offset, direction, chars, equals, 0u, length());
}


/// See documentation at findCharPosWithinRange.
/// This method searches a char position within the given rang (from the given ofset)
/// @returns the given position or clamps at the begin/endrange (depending on the direction)
size_t TextBuffer::findCharPosWithinRangeOrClamp(size_t offset, int direction, const QString& chars, bool equals, size_t beginRange, size_t endRange)
{
    size_t pos = findCharPosWithinRange(offset, direction, chars, equals, beginRange, endRange);
    if (pos == std::string::npos) {
        if (direction < 0) return beginRange;
        if (direction > 0) return endRange;
    }
    return pos;
}



// This method converts the line offsets as a comma-seperated string (easy for debugging)
QString TextBuffer::lineOffsetsAsString()
{
    QString str;
    for (size_t idx = 0, cnt=lineCount(); idx < cnt; ++idx) {
        size_t offset = offsetFromLine(idx);
        if (!str.isEmpty()) str.append(',');
        str.append(QStringLiteral("%1").arg(offset));
    }
    return str;
}


} // edbee
