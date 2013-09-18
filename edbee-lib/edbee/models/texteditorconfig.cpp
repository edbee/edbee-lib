/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "texteditorconfig.h"

#include "debug.h"

namespace edbee {

TextEditorConfig::TextEditorConfig()
    : caretWidth_(2)
    , caretBlinkingRate_(700)   // 0 means no blinking (default = 700)
    , indentSize_(4)
    , useTabChar_(true)
    , charGroups_()
    , whitespaces_("\n\t ")
    , whitespaceWithoutNewline_("\t ")
    , extraLineSpacing_(0)
    , useLineSeparator_(false)
    , lineSeparatorPen_( QColor(230,230,230), 1.5, Qt::DashLine )
    , undoGroupPerSpace_(true)
    , showCaretOffset_(true)
    , themeName_("Monokai")
{
    charGroups_.append( QString("./\\()\"'-:,.;<>~!@#$%^&*|+=[]{}`~?"));
}

/// Returns the caret width in pixels
int TextEditorConfig::caretWidth() const
{
    return caretWidth_;
}

/// Sets the caret redner width in pixels
void TextEditorConfig::setCaretWidth(int width)
{
    caretWidth_ = width;
}

/// Returns the caret blinking rate (delay) (lower means faster )
/// The blinking delay is in milliseconds
int TextEditorConfig::caretBlinkingRate() const
{
    return  caretBlinkingRate_;
}

/// Sets the caret blinking rate (delay) (lower means faster )
/// The blinking delay is in milliseconds
void TextEditorConfig::setCaretBlinkRate(int rate)
{
    caretBlinkingRate_ = rate;
}

/// Returns the indent size in the number of characters
int TextEditorConfig::indentSize() const
{
    return indentSize_;
}

/// Sets the indentsize in characters
void TextEditorConfig::setIndentSize(int size)
{
    indentSize_ = size;
}

/// Should the tab-character be used?
bool TextEditorConfig::useTabChar()
{
    return useTabChar_;
}

/// Should the tab-character be used?
void TextEditorConfig::setUseTabChar(bool enable)
{
    useTabChar_ = enable;
}

/// Returns the characters groups
/// Character groups are groups that considered the 'same' kind of characters.
/// For example skipping to the next word skips the characters of the same group
/// Currently spaces and word/id characters are hardcoded groups.
const QStringList& TextEditorConfig::charGroups() const
{
    return charGroups_;
}

/// Sets the character groups
/// @see TextEditorConfig::charGroups
void TextEditorConfig::setCharGroups(const QStringList &items)
{
    charGroups_ = items;
}

/// Returns all white-space characters
const QString& TextEditorConfig::whitespaces() const
{
    return whitespaces_;
}

/// Sets the whitespace characters
void TextEditorConfig::setWhitespaces(const QString& value)
{
    whitespaces_ = value;
}

/// Retuns the whitespace character without newlines
const QString& TextEditorConfig::whitespaceWithoutNewline() const
{
    return whitespaceWithoutNewline_;
}

void TextEditorConfig::setWhitespaceWithoutNewline(const QString& value)
{
    whitespaceWithoutNewline_ = value;
}

/// Returns the extra line spacing in pixels
int TextEditorConfig::extraLineSpacing() const
{
    return extraLineSpacing_;
}

/// Sets the extra line spacing between lines in pixels
void TextEditorConfig::setExtraLineSpacing(int value)
{
    extraLineSpacing_ = value;
}

/// Should we use a line seperator pen
/// @see TextEditorConfig::lineSeparatorPen
bool TextEditorConfig::useLineSeparator() const
{
    return useLineSeparator_;
}

/// Should we use a sline seperator?
/// @see TextEditorConfig::lineSeparatorPen
void TextEditorConfig::setUseLineSeparator(bool value)
{
    useLineSeparator_ = value;
}

/// The line sperator pen to use.When the setting useLineSeparator is set
/// this pen is used to draw lines between the text-lines
/// @see TextEditorConfig::useLineSeparator
const QPen& TextEditorConfig::lineSeparatorPen() const
{
    return lineSeparatorPen_;
}

/// This method sets the line seperator pen that used to paint
/// line between the text-lines
/// @see TextEditorConfig::useLineSeparator
void TextEditorConfig::setLineSeperatorPen(const QPen& pen)
{
    lineSeparatorPen_ = pen;
}

/// Should a space result in a new 'undo-group'
/// The default behaviour is that pressing a space character results in the
/// closing of an undo group. (This groups the entered characters into 1 undo operation)
bool TextEditorConfig::undoGroupPerSpace() const
{
    return undoGroupPerSpace_;
}

/// Sets the undo group per space value
/// @see TextEditorConfig::undoGroupPerSpace
void TextEditorConfig::setUndoGroupPerSpace(bool enable)
{
    undoGroupPerSpace_ = enable;
}

/// should the caret-offset been shown. The texteditor can signal a
/// statusbar text to a slot. This text can optionally contain the
/// current caret offset
bool TextEditorConfig::showCaretOffset() const
{
    return showCaretOffset_;
}

/// Enables / disables the passing of the character offset
/// @see TextEditorConfig::showCaretOffset
void TextEditorConfig::setShowCaretOffset(bool enable)
{
    showCaretOffset_ = enable;
}

/// returns the active theme name
/// (The theme name is the name without the file-extension of the file in the theme directory)
QString TextEditorConfig::themeName()
{
    return themeName_;
}

/// This method sets the active theme name
void TextEditorConfig::setThemeName(const QString& name)
{
    themeName_ = name;
}

/// returns the current font to use
QFont TextEditorConfig::font() const
{
    return font_;
}

void TextEditorConfig::setFont(const QFont& font)
{
    font_ = font;
}




} // edbee
