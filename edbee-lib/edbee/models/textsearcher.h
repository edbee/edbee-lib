/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QObject>

#include "edbee/models/textrange.h"

namespace edbee {


class RegExp;
class TextEditorWidget;

/// Search options for the text
class TextSearcher : public QObject
{
    Q_OBJECT
public:

    enum SyntaxType {
        SyntaxPlainString,
        SyntaxRegExp
    };

    explicit TextSearcher(QObject *parent = 0);
    virtual ~TextSearcher();

    QString searchTerm() { return searchTerm_; }
    void setSearchTerm( const QString& term );

    SyntaxType syntax() { return syntax_; }
    void setSyntax( SyntaxType syntax);

    bool isCaseSensitive() { return caseSensitive_; }
    void setCaseSensitive( bool sensitive );

    bool isWrapAroundEnabled() { return wrapAround_; }
    void setWrapAround( bool on );

    bool isReverse() { return reverse_; }
    void setReverse( bool on ) { reverse_ = on; }

    TextRange findNextRange( TextEditorWidget* widget );

public slots:

    void findNext( TextEditorWidget* widget );
    void findPrev( TextEditorWidget* widget );
    void selectNext( TextEditorWidget* widget );
    void selectPrev( TextEditorWidget* widget );
    void selectAll( TextEditorWidget* widget );

protected:

    void setDirty();
    RegExp* createRegExp();

private:

    QString searchTerm_;        ///< The current search term
    SyntaxType syntax_;         ///< The syntax-type
    bool caseSensitive_;        ///< case sensitive?
    bool wrapAround_;           ///< should the search wrap around?
    bool reverse_;              ///< search in the reverse direction

    RegExp* regExp_;            ///< The current regexp

};

} // edbee
