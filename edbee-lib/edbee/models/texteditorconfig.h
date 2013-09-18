/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QFont>
#include <QPen>
#include <QString>
#include <QStringList>

namespace edbee {

/// General configuration settings of the text editor
class TextEditorConfig
{
public:
    TextEditorConfig();

    int caretWidth() const;
    void setCaretWidth( int width );

    int caretBlinkingRate() const;
    void setCaretBlinkRate( int rate );

    int indentSize() const;
    void setIndentSize( int size );

    bool useTabChar();
    void setUseTabChar( bool enable );

    const QStringList& charGroups() const;
    void setCharGroups( const QStringList& items );

    const QString& whitespaces() const;
    void setWhitespaces( const QString& value );

    const QString& whitespaceWithoutNewline() const;
    void setWhitespaceWithoutNewline( const QString& );

    int extraLineSpacing() const;
    void setExtraLineSpacing( int value );

    bool useLineSeparator() const;
    void setUseLineSeparator( bool value );

    const QPen& lineSeparatorPen() const;
    void setLineSeperatorPen( const QPen& pen );

    bool undoGroupPerSpace() const;
    void setUndoGroupPerSpace( bool enable );

    bool showCaretOffset() const;
    void setShowCaretOffset( bool enable );

    QString themeName();
    void setThemeName( const QString& name );

    QFont font() const ;
    void setFont( const QFont& font );


private:

    int caretWidth_;                    ///< The caretWith in pixels
    int caretBlinkingRate_;             ///< The caret blinking rate
    int indentSize_;                    ///< The ident-size in characters
    bool useTabChar_;                   ///< Should the tab character be used?
    QStringList charGroups_;            ///< Character groups
    QString whitespaces_;               ///< All whitespaces
    QString whitespaceWithoutNewline_;  ///< Whitespace characters without newline
    int extraLineSpacing_;              ///< The extra space to place between lines
    bool useLineSeparator_;             ///< Should we draw a line-seperator between lines?
    QPen lineSeparatorPen_;             ///< A line seperator pen
    bool undoGroupPerSpace_;            ///< An undogroup per space?
    bool showCaretOffset_;              ///< Show the caret offset?
    QString themeName_;                 ///< The active theme name
    QFont font_;                        ///< The font to used
};

} // edbee
