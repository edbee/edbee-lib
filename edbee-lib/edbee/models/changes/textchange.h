// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QString>

#include "edbee/models/changes/abstractrangedchange.h"

namespace edbee {

/// This is the basic text change that's the base of the textchanges
///
/// This class re-uses the variables offset/length and text. Depending on the undo/redo state
/// these variables contain the new data or the changed data
class EDBEE_EXPORT TextChange : public AbstractRangedChange
{
public:
    TextChange(int offset, int length, const QString& text );
    virtual ~TextChange();

    virtual void execute(TextDocument* document);
    virtual void revert(TextDocument* document);

protected:
    virtual void mergeStoredData( AbstractRangedChange* change );

public:
    virtual bool giveAndMerge(TextDocument *document, Change* textChange );

    virtual QString toString();

    int offset() const;
    void setOffset( int offset );
    virtual int docLength() const;
    virtual int storedLength() const;

    void setDocLength( int len );

    QString storedText() const;
    void setStoredText( const QString& text );
    void appendStoredText( const QString& text );
    const QString docText( TextDocument* doc ) const;

    QString testString();

protected:
    void replaceText( TextDocument* document );

private:
    int offset_;            ///< The offset of the text
    int length_;            ///< the length of the change in the document
    QString text_;          ///< The text data
};

} // edbee
