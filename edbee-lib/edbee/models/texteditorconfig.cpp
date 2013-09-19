/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "texteditorconfig.h"

#include "debug.h"

namespace edbee {

TextEditorConfig::TextEditorConfig( QObject* parent )
    : QObject(parent)
    , changeInProgressLevel_(0)
    , changeCount_(0)
    , caretWidth_(2)
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

/// empty constructor :)
TextEditorConfig::~TextEditorConfig()
{
}

/// use this method to start a group of changes.
/// Default behaviour is to emit a configChanged signal if a setting is changed
/// When you call beginChanges this method will make sure the config surpresses the signals
/// and only a signal is fired after the last endChange.
/// You can nest multiple beginChanges, only the last endChanges fires a signal
void TextEditorConfig::beginChanges()
{
    ++changeInProgressLevel_;
}

/// use this
void TextEditorConfig::endChanges()
{
    Q_ASSERT(changeInProgressLevel_ > 0 );
    --changeInProgressLevel_;
    if( changeInProgressLevel_ == 0 && changeCount_ > 0 ) {
        notifyChange();
    }
}

/// Returns the caret width in pixels
int TextEditorConfig::caretWidth() const
{
    return caretWidth_;
}

/// Sets the caret redner width in pixels
void TextEditorConfig::setCaretWidth(int width)
{
    if( caretWidth_ != width ) {
        caretWidth_ = width;
        notifyChange();
    }
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
    if( caretBlinkingRate_ != rate ) {
        caretBlinkingRate_ = rate;
        notifyChange();
    }
}

/// Returns the indent size in the number of characters
int TextEditorConfig::indentSize() const
{
    return indentSize_;
}

/// Sets the indentsize in characters
void TextEditorConfig::setIndentSize(int size)
{
    if( indentSize_ != size ) {
        indentSize_ = size;
        notifyChange();
    }
}

/// Should the tab-character be used?
bool TextEditorConfig::useTabChar()
{
    return useTabChar_;
}

/// Should the tab-character be used?
void TextEditorConfig::setUseTabChar(bool enable)
{
    if( useTabChar_ != enable ) {
        useTabChar_ = enable;
        notifyChange();
    }
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
    if( charGroups_ != items ) {
        charGroups_ = items;
        notifyChange();
    }
}

/// Returns all white-space characters
const QString& TextEditorConfig::whitespaces() const
{
    return whitespaces_;
}

/// Sets the whitespace characters
void TextEditorConfig::setWhitespaces(const QString& value)
{
    if( whitespaces_ != value ) {
        whitespaces_ = value;
        notifyChange();
    }
}

/// Retuns the whitespace character without newlines
const QString& TextEditorConfig::whitespaceWithoutNewline() const
{
    return whitespaceWithoutNewline_;
}

void TextEditorConfig::setWhitespaceWithoutNewline(const QString& value)
{
    if( whitespaceWithoutNewline_ != value ) {
        whitespaceWithoutNewline_ = value;
        notifyChange();
    }
}

/// Returns the extra line spacing in pixels
int TextEditorConfig::extraLineSpacing() const
{
    return extraLineSpacing_;
}

/// Sets the extra line spacing between lines in pixels
void TextEditorConfig::setExtraLineSpacing(int value)
{
    if( useLineSeparator_ != value ) {
        extraLineSpacing_ = value;
        notifyChange();
    }
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
    if( useLineSeparator_ != value ) {
        useLineSeparator_ = value;
        notifyChange();
    }
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
    if( lineSeparatorPen_ != pen ) {
        lineSeparatorPen_ = pen;
        notifyChange();
    }
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
    if( undoGroupPerSpace_ != enable ) {
        undoGroupPerSpace_ = enable;
        notifyChange();
    }
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
    if( showCaretOffset_ != enable ) {
        showCaretOffset_ = enable;
        notifyChange();
    }
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
    if( themeName_ != name ) {
        themeName_ = name;
        notifyChange();
    }
}

/// returns the current font to use
QFont TextEditorConfig::font() const
{
    return font_;
}

/// Changes the font that's used by the editor
void TextEditorConfig::setFont(const QFont& font)
{
    if( font_ != font ) {
        font_ = font;
        notifyChange();
    }
}

/// This internal method is used to notify the listener that a change has happend
/// Thi smethod only emits a signal if there's no config group change busy
void TextEditorConfig::notifyChange()
{
    ++changeCount_;
    if( changeInProgressLevel_ == 0 ) {
        emit configChanged();
        changeCount_= 0;
    }
}




} // edbee
