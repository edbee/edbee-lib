/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QString>

#include "edbee/models/textchange.h"

namespace edbee {

class SingleTextChange : public TextChange
{
public:
    SingleTextChange(int offset, int length, const QString& text );
    virtual ~SingleTextChange();

    virtual void execute(TextDocument* document);
    virtual void revert(TextDocument* document);

//    virtual bool canMerge( TextDocument* document, TextChange* textChange );
    virtual bool merge(TextDocument *document, TextChange* textChange );

    virtual void moveOffset( int delta );

    virtual QString toString();


    int offset() { return offset_; }
    void setOffset( int offset ) { offset_ = offset; }

    int length() { return length_; }
    void setLength( int len ) { length_ = len; }

    const QString& text() { return text_ ; }
    void setText( const QString& text ) { text_ = text; }
    void appendText( const QString& text ) { text_.append( text ); }
    const QString docText( TextDocument* doc );

    QString testString();

protected:

    void replaceText( TextDocument* document );

private:
    int offset_;            ///< The offset of the text
    int length_;            ///< the length of the text
    QString text_;          ///< The text
};

} // edbee
