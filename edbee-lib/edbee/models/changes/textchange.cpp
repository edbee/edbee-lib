// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textchange.h"

#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"

#include "edbee/debug.h"

namespace edbee {


/// Constructs a single textchange
/// @param offset, the offset of the change
/// @param length, the length of the change
/// @param text , the new text
/// @param executed, a boolean (mainly used for testing) to mark this change as exected
TextChange::TextChange(size_t offset, size_t length, const QString& text)
    : offset_(offset)
    , length_(length)
    , text_(text)
{
}


/// undo's a single textchange
TextChange::~TextChange()
{
}


/// executes the given textchange
/// @param document the document to execute the change on
void TextChange::execute(TextDocument* document)
{
    replaceText(document);
}


/// Reverts the single textchange
/// @param document the document to execute the change on
void TextChange::revert(TextDocument* document)
{
    replaceText(document);
}


/// Merges the old data with the new data
/// @apram change the data to merge with
void TextChange::mergeStoredData(AbstractRangedChange* change)
{
    TextChange* singleTextChange = dynamic_cast<TextChange*>(change);

    QString newText;
    newText.resize(static_cast<qsizetype>(getMergedStoredLength(change)));
    mergeStoredDataViaMemcopy(newText.data(), text_.data(), singleTextChange->text_.data(), change, sizeof(QChar));
    text_ = newText;
}


/// This method gives the given change to this textchange. The changes will be merged
/// if possible. This method currently only works with executed changes!!!
///
/// @param document the document
/// @param textChange the textchange to mege
/// @return true on success else false
bool TextChange::giveAndMerge( TextDocument* document, Change* textChange)
{
    Q_UNUSED(document);
    TextChange* change = dynamic_cast<TextChange*>(textChange);
    if (change) {
        return merge(change);
    }
    return false;
}


/// converts the change to a string
QString TextChange::toString()
{
//    return "SingleTextChange";
    return QStringLiteral("SingleTextChange:%1").arg(testString());
}


/// Return the offset
/// @return the offset of the change
size_t TextChange::offset() const
{
    return offset_;
}


/// set the new offset
/// @param offset the new offset
void TextChange::setOffset(size_t offset)
{
    offset_ = offset;
}


/// This is the length in the document
size_t TextChange::docLength() const
{
    return length_;
}


/// The content length is the length that's currently stored in memory.
size_t TextChange::storedLength() const
{
    return static_cast<size_t>(text_.size());
}


/// Set the length of the change
/// @param len sets the length of the change
void TextChange::setDocLength(size_t len)
{
    length_ = len;
}

/// The text currently stored in this textchange
QString TextChange::storedText() const
{
    return text_;
}


/// Sets the text of this change
/// @param text the new text
void TextChange::setStoredText(const QString& text)
{
    text_ = text;
}


/// Appends the text to this change
void TextChange::appendStoredText(const QString& text)
{
    text_.append( text );
}


/// Rketurns the text currently in the document
const QString TextChange::docText(TextDocument* doc) const
{
    return doc->textPart( offset_, length_ );
}


/// Returns a string used for testing
QString TextChange::testString()
{
    return QStringLiteral("%1:%2:%3").arg(offset_).arg(length_).arg(QString(text_).replace("\n","§"));
}


/// replaces the text and stores the 'old' content
/// @param document the document to change it for
void TextChange::replaceText(TextDocument* document)
{
    TextBuffer* buffer = document->buffer();
    QString old = buffer->textPart(offset_, length_);

    buffer->replaceText(offset_, length_, text_);
    length_ = static_cast<size_t>(text_.length());
    text_ = old;
}


} // edbee
