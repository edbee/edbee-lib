// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include "edbee/models/textbuffer.h"
#include "edbee/util/gapvector.h"
#include "edbee/util/lineoffsetvector.h"

namespace edbee {


/// This textbuffer implementation uses QChars for storing the data.
class EDBEE_EXPORT CharTextBuffer : public TextBuffer
{
public:
    CharTextBuffer(QObject* parent=0);

    virtual size_t length() const;
    virtual QChar charAt(size_t offset) const;
    virtual QString textPart(size_t offset, size_t length) const;

    virtual void replaceText(size_t offset, size_t length, const QChar* buffer, size_t bufferLength);

    virtual size_t lineCount() { return lineOffsetList_.length(); }

    virtual size_t lineFromOffset(size_t offset);
    virtual size_t offsetFromLine(size_t line);

    virtual void rawAppendBegin();
    virtual void rawAppend(QChar c);
    virtual void rawAppend(const QChar* data, size_t dataLength);
    virtual void rawAppendEnd();

    virtual QChar* rawDataPointer();

    /// TODO: Temporary debug method. REMOVE!!
    LineOffsetVector& lineOffsetList() { return lineOffsetList_; }

protected slots:

    void emitTextChanged( edbee::TextBufferChange* change, QString oldText = QString());

private:
    QCharGapVector buf_;                     ///< The textbuffer
    LineOffsetVector lineOffsetList_;        ///< The line offset vector

    size_t rawAppendStart_;                     ///< The start offset of raw appending. std::string::npos means no appending is happening
    size_t rawAppendLineStart_;                 ///< The line start. std::string::npos no appending is happening
};

} // edbee
