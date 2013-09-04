/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QString>
#include <QStringList>
#include <QPen>

namespace edbee {

/// General configuration settings of the text editor
/// Currently hardcoded but, by supplying an interface it will be posible to make it adjustable later
class TextEditorConfig
{
public:
    TextEditorConfig();

    /// Returns the caret width in pixels
    int caretWidth() const { return caretWidth_; }

    /// Returns the caret blinking rate (the delay, how lower how faster)
    int caretBlinkingRate() const { return  caretBlinkingRate_; }

    /// Returns the indent size
    int indentSize() const { return indentSize_; }

    /// Should the tab-character be used?
    bool useTabChar() { return useTabChar_; }

    /// Returns the word seperator character
    const QStringList& charGroups() const { return charGroups_; }

    /// Returns all 'white-spaces'
    const QString& whitespaces() const { return whitespaces_; }

    /// Retuns the whitespace character without newlines
    const QString& whitespaceWithoutNewline() const { return whitespaceWithoutNewline_; }

    /// Returns the extra line spacing in pixels
    int extraLineSpacing() const { return extraLineSpacing_; }

    /// should we use a line seperator color
    bool useLineSeparator() const { return useLineSeparator_; }

    /// The line sperator pen to use
    const QPen& lineSeparatorPen() const { return lineSeparatorPen_; }

    bool undoGroupPerSpace() const { return undoGroupPerSpace_; }

    bool showCaretOffset() const { return showCaretOffset_;}

    QString themeName() { return themeName_; }

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
};

} // edbee
