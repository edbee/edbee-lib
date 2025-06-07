// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QObject>
#include <QVector>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>

namespace edbee {

class TextBuffer;
class TextBufferChange;
class TextRange;
class TextLineData;
class LineOffsetVector;


class EDBEE_EXPORT TextBufferChangeData : public QSharedData
{
public:
    TextBufferChangeData(TextBuffer* buffer, size_t off, size_t len, const QChar* text, size_t textlen);
    TextBufferChangeData(LineOffsetVector* lineOffsets, size_t off, size_t len, const QChar* text, size_t textlen);

    // text information
    size_t offset_;             ///< The offset in the buffer
    size_t length_;             ///< The number of chars to replaced
    const QChar* newText_;      ///< The reference to a new text
    size_t newTextLength_;      ///< The length of this text

    // line informationm
    size_t line_;                    ///< The line number were the change occured
    size_t lineCount_;               ///< the number of lines that are involved.
    QVector<size_t> newLineOffsets_; ///< A list of new line offset (offsets within the document!!)
};


/// This clas represents a text buffer change and is used to pass around between events
/// This is a shareddata object so the data can be thrown between different threads (delayed emit-support)_
/// TODO: Still problematic maybe the QChar* text pointer. It is possible that this pointer is being freed.
class EDBEE_EXPORT TextBufferChange {
public:
    TextBufferChange();
    TextBufferChange(TextBuffer* buffer, size_t off, size_t len, const QChar* text, size_t textlen);
    TextBufferChange(LineOffsetVector* lineOffsets, size_t off, size_t len, const QChar* text, size_t textlen);
    TextBufferChange(const TextBufferChange& other);

    size_t offset() const { return d_->offset_; }
    size_t length() const { return d_->length_; }
    const QChar* newText() const  { return d_->newText_; }
    size_t newTextLength() const { return d_->newTextLength_; }
    size_t line() const { return d_->line_; }
    size_t lineCount() const { return d_->lineCount_; }
    inline size_t newLineCount() const { return static_cast<size_t>(d_->newLineOffsets_.size()); }
    const QVector<size_t>& newLineOffsets() const { return d_->newLineOffsets_; }

    const QString toDebugString() const;


private:
    QExplicitlySharedDataPointer<TextBufferChangeData> d_;
};

/// This class represents the textbuffer of the editor
class EDBEE_EXPORT TextBuffer : public QObject
{
Q_OBJECT

public:
    TextBuffer(QObject* parent = 0);

// Minimal abstract interface to implement

    ///  returns the number of 'characters'.
    virtual size_t length() const = 0;

    /// returns a single char
    virtual QChar charAt(size_t offset) const = 0;

    /// return the given text.
    virtual QString textPart(size_t offset, size_t length) const = 0;

    /// replaces the given text and fire a 'text-replaced' signal
    virtual void replaceText(size_t offset, size_t length, const QChar* buffer, size_t bufferLength) = 0;

    /// this method should return an array with all line offsets. A line offset pointsto the START of a line
    /// So it does NOT point to a newline character, but it points to the first character AFTER the newline character
    virtual size_t lineCount() = 0; // { return lineOffsets().length(); }
    virtual size_t lineFromOffset(size_t offset) = 0;
    virtual size_t offsetFromLine(size_t line) = 0;

// raw loading methods

    /// starts raw appending
    virtual void rawAppendBegin() = 0;

    /// append the given character to the buffer
    virtual void rawAppend(QChar c) = 0;

    /// raw append the given character string
    virtual void rawAppend(const QChar* data, size_t dataLength) = 0;

    /// the end raw append method should bring the document in a consistent state and
    /// emit the correct "replaceText" signals
    ///
    /// WARNING the textAboutToBeReplaced signals are given but at that moment the text is already replaced
    /// And the newlines are already added to the newline list!
    virtual void rawAppendEnd() = 0;

    /// returns the raw data buffer.
    /// WARNING this method CAN be slow because when using a gapvector the gap is moved to the end to make a full buffer
    /// Modifying the content of the data will mess up the line-offset-vector and other dependent classes. For reading it's ok :-)
    virtual QChar* rawDataPointer() = 0;


// easy functions

    /// Replace the given text.
    virtual void replaceText(size_t offset, size_t length, const QString& text);

    QString text();
    void setText(const QString& text);
    virtual size_t columnFromOffsetAndLine(size_t offset, size_t line = std::string::npos);
    virtual void appendText(const QString& text);
    virtual size_t offsetFromLineAndColumn(size_t line, size_t col);
    virtual QString line(size_t line);
    virtual QString lineWithoutNewline(size_t line);
    
    virtual size_t lineLength(size_t line);
    virtual size_t lineLengthWithoutNewline(size_t line);
    virtual void replaceText(const TextRange& range, const QString& text);

    virtual size_t findCharPos(size_t offset, int direction, const QString& chars, bool equals);
    virtual size_t findCharPosWithinRange(size_t offset, int direction, const QString& chars, bool equals, size_t beginRange, size_t endRange);
    virtual size_t findCharPosOrClamp(size_t offset, int direction, const QString& chars, bool equals);
    virtual size_t findCharPosWithinRangeOrClamp(size_t offset, int direction, const QString& chars, bool equals, size_t beginRange, size_t endRange);

    virtual QString lineOffsetsAsString();

 signals:

    void textAboutToBeChanged(edbee::TextBufferChange change);
    void textChanged(edbee::TextBufferChange change, QString oldText = QString());

};

} // edbee

// needs to be OUTSIDE the namespace!!
Q_DECLARE_METATYPE(edbee::TextBufferChange)


