/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QString>

#include "edbee/models/textchange.h"

namespace edbee {

/// This is the basic text change that's the base of the textchanges
///
/// This class re-uses the variables offset/length and text. Depending on the undo/redo state
/// these variables contain the new data or the changed data
class SingleTextChange : public TextChange
{
public:
    SingleTextChange(int offset, int length, const QString& text, bool executed=false );
    virtual ~SingleTextChange();

    virtual void execute(TextDocument* document);
    virtual void revert(TextDocument* document);

    virtual bool giveAndMerge(TextDocument *document, TextChange* textChange );
    virtual void applyOffsetDelta( int offset, int length, int newLength );

    virtual QString toString();

    int offset() const;
    void setOffset( int offset );
    void addOffset( int amount );

    int oldLength() const;
    int newLength() const;

    int length() const;
    void setLength( int len );

    QString oldText( TextDocument* doc ) const;
    QString newText( TextDocument* doc ) const;
    QString storedText() const;


    void setStoredText( const QString& text );
    void appendStoredText( const QString& text );
    const QString docText( TextDocument* doc ) const;

    QString testString();
    bool isOverlappedBy( SingleTextChange* secondChange );
    bool isTouchedBy( SingleTextChange* secondChange );

    bool isExecuted() const;

protected:

    void replaceText( TextDocument* document );

private:
    int offset_;            ///< The offset of the text
    int length_;            ///< the length of the text
    QString text_;          ///< The text
    bool executed_;         ///< The executed flag (has the change been executed?)
};

} // edbee
